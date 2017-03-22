#include <minix/sysutil.h>
#include "keyboard.h"
#include "timer.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include "i8042.h"
#include <string.h>

int hook_kbd = 8;

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
		return -1; 

	if (sys_irqrmpolicy(&hook_kbd_temp) != OK)
		return -2;

	return 0;
}

unsigned long ler_tecla() {
	unsigned long estado, tecla;
	int tentativas = 0;

	while (tentativas < 7) {
		if (sys_inb(STAT_REG, &estado) != OK)
			return -1;
		if (estado & BIT(0)) { 
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
