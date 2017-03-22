#include <minix/sysutil.h>
#include "keyboard_lab3.h"
#include "timer.h"
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
#define ESC 0x81 
#define TWO_BYTES 0xe0 

int hook_kbd = 7;

int kbd_subscribe() {
	//Note that the keyboard's IRQ number is 1.

	int id = hook_kbd; //Para nao perder o id original

	if (sys_irqsetpolicy(1, IRQ_REENABLE | IRQ_EXCLUSIVE, &id) != OK)
		return -1;

	if (sys_irqenable(&id) != OK)
		return -2; //No caso de sys_irqsetpolicy() não ligar

	return BIT(hook_kbd); //retorna o bit da interrupt
}

int kbd_unsubscribe() {

	int hook_kbd_temp = hook_kbd;

	if (sys_irqdisable(&hook_kbd_temp) != OK)
		return 1;

	if (sys_irqrmpolicy(&hook_kbd_temp) != OK)
		return 2;

	return 0;
}

unsigned long ler_tecla() {
	unsigned long estado, tecla;
	int tentativas = 0;

	while (tentativas < 7) {
		if (sys_inb(STAT_REG, &estado) != OK)
			return -1; //Ler o status do Registo do KB
		if (estado & BIT(0)) { //0 indica que o buffer do kb está vazio, 1 indica que o buffer tem informação mas ainda não foi lida
			if (sys_inb(OUT_BUF, &tecla) != OK)
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

int kbd_out_esc() { //Depending on whether its argument is zero or not, it should use the IH written in C or the IH written in assembly.
	// Need to subscribe the KBC interrupts
	//read the scancode from the OUT_BUF
	//the IH Must be disabled to prevent it from reading the OUT_BUF before your handler does it
	// Use not only the IRQ_REENABLE but also the IRQ_EXCLUSIVE policy in sys_irqsetpolicy() , i.e. use IRQ_REENABLE|IRQ_EXCLUSIVE
	//Usar driver_receive() loop
	//Interrupt handler reads the bytes from the KBC’s OUT_BUF
	//Só deve ler um byte de cada vez

	int ipc_status;
	message msg;
	int r, byte = 0;
	unsigned int irq_set;
	unsigned long key, code;

	irq_set = kbd_subscribe();

	if (irq_set < 0) {
		printf("Erro ao ligar o interrupt\n");
		return 1;
	}

	while (key != ESC) { //Enquanto o code for diferente de ESC
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
						key = ler_tecla();
				}
				break;
			default:
				break;
			}/* no other notifications expected: do nothing */
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	kbd_unsubscribe(); // Unsubscribes Keyboard Interrupts
	return 0;
}
