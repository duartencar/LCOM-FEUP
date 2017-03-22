#include "rtc.h"

void wait_valid_rtc(void) {
	unsigned long regA = 0;

	do {

		sys_outb(RTC_ADDR_REG, RTC_REG_A);

		sys_inb(RTC_DATA_REG, &regA);

	} while (regA & RTC_UIP);
}

unsigned short read_register(int resgister) {
	unsigned long is_ok, valor;

	wait_valid_rtc();
	is_ok = sys_outb(RTC_ADDR_REG, resgister);

	if (is_ok != OK) {
		printf("ERROR:: Can not read register %d  !\n", resgister);

		return 1;
	}

	is_ok = sys_inb(RTC_DATA_REG, &valor);

	if (is_ok != OK) {
		printf("ERROR:: Can not write register %d  !\n", resgister);

		return 1;
	}

	return valor;
}

void rtc_get_date() { 

	hour_min *tempo = (hour_min *) malloc(sizeof(hour_min));

	tempo->min = read_register(RTC_MIN);
	tempo->hours = read_register(RTC_HOUR);

	tempo->hours = ((tempo->hours >> 4) * 10) + (tempo->hours & 0x0F);
	tempo->min = ((tempo->min >> 4) * 10) + (tempo->min & 0x0F);

	free(tempo);
}

int rtc_get_hour(){
int hour;
hour = read_register(RTC_HOUR);
hour = ((hour >> 4) * 10) + (hour & 0x0F);
return hour;
}
