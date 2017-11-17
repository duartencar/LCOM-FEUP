#include <minix/sysutil.h>
#include "test3.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include <string.h>

#define BIT(n) (0x01<<(n))
#define DELAY_US 20000
#define STAT_REG    0x64
#define KBC_CMD_REG 0x64
#define IN_BUF 0x60
#define OUT_BUF 0x60
#define PAR_ERR BIT(7)
#define TO_ERR  BIT(5)
#define M_or_B BIT(7)
#define ACK 0xFA
#define Resend 0xFE
#define ERROR 0xFC
#define COMANDO_LED 0xED
unsigned long lerTeclaAss();

int contador = 0;
int hook_kbd = 7;
int hook_n = 10;

//TIMER0 FUNCTIONS FROM OUR LAB2 //

int timer_subscribe_int(void)
{
	/*	Subscribes and enables Timer 0 interrupts.
	 Returns bit order in interrupt mask; negative value on failure */

	// Guardar o valor de hook_n para retornar caso não haja falhas.
	int id = hook_n;

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_n) != 0)
		return -1;

	if (sys_irqenable(&hook_n) != 0)
		return -2; //No caso de sys_irqsetpolicy() não ligar

	return id;
}

int timer_unsubscribe_int()
{
	// Unsubscribes Timer 0 interrupts.
	//Return 0 upon success and non-zero otherwise

	if (sys_irqdisable(&hook_n) != 0)
		return 1;

	if (sys_irqrmpolicy(&hook_n) != 0)
		return 2;

	return 0;
}

void timer_int_handler()
{
	//Timer 0 interrupt handler
	//Increments counter
	contador++;
}

// ---------------------------------- //

// KBD FUNCTIONS //
int kbd_subscribe()
{
	//Note that the keyboard's IRQ number is 1.

	int id = hook_kbd; //Para nao perder o id original

	if (sys_irqsetpolicy(1, IRQ_REENABLE | IRQ_EXCLUSIVE, &id) != OK)
		return -1;

	if (sys_irqenable(&id) != OK)
		return -2; //No caso de sys_irqsetpolicy() não ligar

	return BIT(hook_kbd); //retorna o bit da interrupt
}

int kbd_unsubscribe()
{
	int hook_kbd_temp = hook_kbd;

	if (sys_irqdisable(&hook_kbd_temp) != OK)
		return 1;

	if (sys_irqrmpolicy(&hook_kbd_temp) != OK)
		return 2;

	return 0;
}

unsigned long ler_tecla()
{
	unsigned long estado, tecla;

	int tentativas = 0;

	while (tentativas < 7)
	{
		if (sys_inb(0x64, &estado) != OK)
			return -1; //Ler o status do Registo do KB
		//0 indica que o buffer do kb está vazio, 1 indica que o buffer tem informação mas ainda não foi lida
		if (estado & BIT(0))
		{
			if (sys_inb(0x60, &tecla) != OK)
				return -1;
			if ((estado & (PAR_ERR | TO_ERR)) == 0)
				return tecla;
			else
				return -1;
		}

		tickdelay(micros_to_ticks(DELAY_US));

		tentativas++;
	}
}

int kbd_test_scan(unsigned short ass)
{
	//Depending on whether its argument is zero or not,
	//it should use the IH written in C or the IH written in assembly.
	// Need to subscribe the KBC interrupts
	//read the scancode from the OUT_BUF
	//the IH Must be disabled to prevent it from reading the OUT_BUF before your handler does it
	// Use not only the IRQ_REENABLE but also the IRQ_EXCLUSIVE policy in sys_irqsetpolicy(),
	// i.e. use IRQ_REENABLE|IRQ_EXCLUSIVE
	//Usar driver_receive() loop
	//Interrupt handler reads the bytes from the KBC’s OUT_BUF
	//Só deve ler um byte de cada vez

	int ipc_status;

	message msg;

	int r, byte = 0;

	unsigned int irq_set;

	unsigned long key, code;

	irq_set = kbd_subscribe();

	if (irq_set < 0)
	{
		printf("Erro ao ligar o interrupt\n");

		return 1;
	}

	while (key != 0x81) //Enquanto o code for diferente de ESC
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);

			continue;
		}

		if (is_ipc_notify(ipc_status)) /* received notification */
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) /* subscribed interrupt */
				{
					if (!ass)
						key = ler_tecla();
					else
						key = lerTeclaAss();

					if (key == 0xe0 && byte == 0)
					{ //Verificar se a tecla tem 2 ou 1 byte
						printf("Tecla com 2 bytes & ");

						byte++;

						code = 0;

						code = key << 8;
					}
					else
					{
						byte = 0;

						code |= key;

						if (key & M_or_B) //Verificar se a tecla é pressionada ou largada
							printf("Breakcode -> 0x%02x\n", code);

						else
							printf("Makecode -> 0x%02x\n", code);

						code = 0;
					}
				}
				break;
			default:
				break;
			}/* no other notifications expected: do nothing */
		}
		else
		{
			/* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	kbd_unsubscribe(); // Unsubscribes Keyboard Interrupts

	printf("ESC\n");

	return 0;
}

int escrever_kbc(unsigned char cmd)
{
	unsigned long status;
	/* loop while 8042 input buffer is not empty */

	while (1)
	{
		if (sys_inb(STAT_REG, &status) != OK)
			return 1; 	//Receber o Status do Keyboard
		if ((status & BIT(1)) == 0) //Se o Input Buffer Full estiver vazio
		{
			if (sys_outb(OUT_BUF, cmd) != OK)
				return 1;
			else
				return 0;
		}

		tickdelay(micros_to_ticks(DELAY_US));
	}

	return 1;
}

int escrever_comando(unsigned char cmd)
{
	unsigned long resposta = 0;
	//Enquanto a resposta não for ACK e escrever o comando no respetivo LED, repete o ciclo.

	while (1)
	{
		if (escrever_kbc(COMANDO_LED) != 0)
			return 1; //Enviar o byte 0xED
		if (sys_inb(OUT_BUF, &resposta) != OK)
			return 1;
		if (resposta == ACK)
		{
			escrever_kbc(cmd); //Enviar o byte correspondente ao led pretendido

			return 0;
		}
	}

	return 1;
}

int kbd_test_leds(unsigned short n, unsigned short *leds)
{
	/*Cada elemento do vetor leds especifica o led que deve estar ligado*/

	/*Valores aceitáveis: 0 1 2 */
	// 0 - Scroll lock
	// 1 - Numeric lock
	// 2 - Caps lock

	char locks[3][15];

	strcpy(locks[0], "SCROLL LOCK");

	strcpy(locks[1], "NUM LOCK");

	strcpy(locks[2], "CAPS LOCK");

	/*Temos de guardar o estado atual do Led para saber se o desligamos ou ligamos
	usar a função do timer do lab2 para quando passar um segundo,
	em vez de imprimir os segundos que passaram, processar um dos valores do vetor */

	int ipc_status;

	message msg;

	unsigned long byte;

	int id;

	int r;

	unsigned int irq_set;

	int i = 0; //Índice do vetor leds

	id = timer_subscribe_int();

	if (id < 0)
	{
		printf("Erro ao ligar o interrupt\n");

		return 1;
	}

	irq_set = BIT(id);

	while (contador < n * 60) // o tempo depende do número de elementos do vetor leds
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);

			continue;
		}
		if (is_ipc_notify(ipc_status)) /* received notification */
		{
			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) /* subscribed interrupt */
				{
					contador++; // Incrementa o contador

					if ((contador % 60) == 0)
					{
						//Processar os LED'S
						byte = 0;

						byte |= BIT(leds[i]);

						if (escrever_comando(byte) != 0)
							printf("Erro ao Ligar/Desligar o %s\n", locks[leds[i]]);
						else
							printf("%s Ligado/Desligado com sucesso!\n", locks[leds[i]]);

						i++; //Incrementar o índice
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else
		{
			/* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	timer_unsubscribe_int(); // Unsubscribes Timer 0 interrupts.

	free(leds);

	return 0;
}

int kbd_test_timed_scan(unsigned short n)
{
	/* should exit not only when the user releases the ESC key,
	but also if it does not receive a scancode for a number seconds equal to its argument,
	 n. */

	int ipc_status;

	message msg;

	int r, byte = 0;

	unsigned int irq_set_kbd, irq_set_timer;

	unsigned long key, code;

	int segundos = n;

	irq_set_kbd = kbd_subscribe();

	irq_set_timer = BIT(timer_subscribe_int());

	if (irq_set_kbd < 0 || irq_set_timer < 0)
	{
		printf("Erro ao ligar o interrupt\n");

		return 1;
	}

	while ((key != 0x81) && (contador < n * 60)) //Enquanto o code for diferente de ESC e o tempo diferente de n segundos
	{
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed with: %d", r);

			continue;
		}
		if (is_ipc_notify(ipc_status)) /* received notification */
		{
			if (msg.NOTIFY_ARG & irq_set_timer) //Notificacao do timer0
				timer_int_handler(); //Incrementa contador

			switch (_ENDPOINT_P(msg.m_source))
			{
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_kbd) /* subscribed interrupt */
				{
					key = ler_tecla();

					if (key == 0xe0 && byte == 0)//Verificar se a tecla tem 2 ou 1 byte
					{
						printf("Tecla com 2 bytes & ");

						byte++;

						code = 0;

						code = key << 8;
					}
					else
					{
						byte = 0;

						code |= key;

						if (key & M_or_B) //Verificar se a tecla é pressionada ou largada
							printf("Breakcode -> 0x%02x\n", code);

						else
						{
							printf("Makecode -> 0x%02x\n", code);

							segundos = n;

							contador = 0;
						}

						code = 0;
					}
				}
				break;
			default:
				break;
			}/* no other notifications expected: do nothing */
			if (contador % 60 == 0 && contador != 0)
			{
				printf("%ds\n", segundos);

				segundos--;
			}
		} else
		{
			/* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if (key == 0x81)
		printf("ESC -> Sair\n");
	else
		printf("0s -> Sair\n");

	kbd_unsubscribe(); // Unsubscribes Keyboard Interrupts

	timer_unsubscribe_int(); // Unsubscribes Timer0 Interrupts

	return 0;
}
