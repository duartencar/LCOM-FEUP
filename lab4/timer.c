#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"

#define FQ 60
int contador = 0;
int hook_n = 10;

int timer_set_square(unsigned long timer, unsigned long freq) {
	unsigned char Control_W = 0; //Comando p/ programar o Timer
	unsigned char Timer_CR = 0; //Count Register do Timer (Porta 0x**)
	unsigned char status = 0;
	unsigned int Tempo = TIMER_FREQ / freq;

	timer_get_conf(timer, &status);

	if (timer == 0) {
		Timer_CR = TIMER_0; // Porta 0x40
		Control_W = Control_W | TIMER_SEL0;
	} // BIT 7 e 6 correspondem a 00
	else if (timer == 1) {
		Timer_CR = TIMER_1; // Porta 0x41
		Control_W = Control_W | TIMER_SEL1;
	} // BIT 7 e 6 correspondem a 01
	else if (timer == 2) {
		Timer_CR = TIMER_2; // Porta 0x42
		Control_W = Control_W | TIMER_SEL2;
	} // BIT 7 e 6 correspondem a 10

	Control_W = Control_W | TIMER_LSB_MSB; // Tipo de Acesso -> LSB followed by MSB
	Control_W = Control_W | TIMER_SQR_WAVE; //Modo 3

	if (sys_outb(TIMER_CTRL, Control_W | (status & BIT(0))) != 0)
		return 1; //Enviar o CW para o Controlo de Registo 0x43
	if (sys_outb(Timer_CR, Tempo & 0xff) != 0)
		return 1; // LSB no Timer
	if (sys_outb(Timer_CR, (Tempo >> 8) & 0xff) != 0)
		return 1; //MSB no Timer
	return 0;
}

int timer_subscribe_int(void) {
	/*	Subscribes and enables Timer 0 interrupts.
	 Returns bit order in interrupt mask; negative value on failure */

	int id = hook_n; // Guardar o valor de hook_n para retornar caso não haja falhas.
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_n) != 0)
		return -1;
	if (sys_irqenable(&hook_n) != 0)
		return -2; //No caso de sys_irqsetpolicy() não ligar

	return id;
}

int timer_unsubscribe_int() {
	// Unsubscribes Timer 0 interrupts. 
	//Return 0 upon success and non-zero otherwise  
	if (sys_irqdisable(&hook_n) != 0)
		return 1;
	if (sys_irqrmpolicy(&hook_n) != 0)
		return 2;
	return 0;
}

void timer_int_handler() {
	//Timer 0 interrupt handler
	//Increments counter
	contador++;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	//Ler configuração do Timer

	//TIMER_RB_SEL(n)   BIT((n)+1)
	//TIMER_RB_CMD   (BIT(7)|BIT(6)) 1 1 

	unsigned char Timer_CR = 0; //Count Register do Timer
	if (timer == 0)
		Timer_CR = TIMER_0;
	else if (timer == 1)
		Timer_CR = TIMER_1;
	else if (timer == 2)
		Timer_CR = TIMER_2;

	// 2 MSB são 1, (11 -> Read-Back Command)
	// Colocar o COUNT a 1 para negar o COUNT, assim apenas obtemos o STATUS

	/*	int sys_inb(port_t port, unsigned long *byte);
	 int sys_outb(port_t port, unsigned long byte);
	 Need to write to the control register before accessing any of the timers */

	if (sys_outb(TIMER_CTRL,
			TIMER_RB_SEL(timer) | TIMER_RB_CMD | TIMER_RB_COUNT_) != 0)
		return 1; //Enviar o RB para o Controlo de Registo 0x43
	if (sys_inb(Timer_CR, (unsigned long*) st) != 0)
		return 1; // Ler o Status do Timer
	return 0;
}

int timer_display_conf(unsigned char conf) {
	//Imprimir Status do Timer contido em "conf"

	//BIT(n)   (0x01<<(n)) 
	//Fazer conf & BIT(n)

	//BIT 7 -> Output
	if (conf & BIT(7))
		printf("Output -> Ligado\n", conf & BIT(7));
	else
		printf("Output -> Desligado\n");

	//BIT 6 -> Null Count
	printf("NULL Count -> %d\n", conf & BIT(6));

	//BIT 5/4/3/2/1/0 -> Control Word

	//BIT 5/4 -> Modo de Acesso
	if ((conf & (BIT(5) | BIT(4))) == TIMER_LSB)
		printf("Tipo de Acesso -> LSB\n");
	else if ((conf & (BIT(5) | BIT(4))) == TIMER_MSB)
		printf("Tipo de Acesso -> MSB\n");
	else if ((conf & (BIT(5) | BIT(4))) == TIMER_LSB_MSB)
		printf("Tipo de Acesso -> LSB followed by MSB\n");
	else
		printf("Tipo de Acesso Desconhecido\n");

	//BIT 3/2/1 -> Modo de Operação

	switch (conf & (BIT(3) | BIT(2) | BIT(1))) {

	case 0x00:
		printf("Modo -> 0\n");
		break;

	case BIT(1):
		printf("Modo -> 1\n");
		break;

	case BIT(2):
		printf("Modo -> 2\n");
		break;

	case (BIT(3) | BIT(2)):
		printf("Modo -> 2\n");
		break;

	case (BIT(2) | BIT(1)):
		printf("Modo -> 3\n");
		break;

	case (BIT(3) | BIT(2) | BIT(1)):
		printf("Modo -> 3\n");
		break;

	case BIT(3):
		printf("Modo -> 4\n");
		break;

	case (BIT(3) | BIT(1)):
		printf("Modo -> 5\n");
		break;
	}

	//BIT 0 -> BCD ou Binário
	if (conf & BIT(0))
		printf("Counting Mode -> BCD\n");
	else
		printf("Counting Mode -> Binary\n");
	return 0;
}

int timer_test_square(unsigned long freq) {
	//Function timer_test_square() needs only to call timer_set_square() with the appropriate arguments.
	return timer_set_square(0, freq);

}

int timer_test_int(unsigned long time) {

	int ipc_status;
	message msg;
	int id;
	int r;
	unsigned int irq_set;
	int segundos = 0;
	id = timer_subscribe_int();
	if (id < 0) {
		printf("Erro ao ligar o interrupt\n");
		return 1;
	}

	irq_set = BIT(id);

	while (contador < (FQ * time)) { /* You may want to use a different condition */ //Enquanto o contador for menor que 60 (default) vezes o tempo
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					timer_int_handler(); // Incrementa o contador
					if ((contador % FQ) == 0) {
						printf("%d\n", segundos + 1); // Se o contador estiver num multiplo de 60, estamos num novo segundo
						segundos++;
					} // Incrementa o contador de segundos
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	timer_unsubscribe_int(); // Unsubscribes Timer 0 interrupts. 
	return 0;
}

int timer_test_config(unsigned long timer) {
	if (timer == 0 || timer == 1 || timer == 2) {
		unsigned char config = 0;
		timer_get_conf(timer, &config);
		timer_display_conf(config);
		return 0;
	}
	return 1;
}

