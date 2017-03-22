#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "i8254.h"
#include "Menu.h"
#include "Mouse.h"
#include "Bitmap.h"
#include "vbe.h"
#include "lmlib.h"
#include "video_gr.h"
#include "keyboard.h"
#include "timer.h"
#include "rtc.h"
#include "game.h"
#include "game_multi.h"
#include "player.h"
#include "Ball.h"
#include "goalkeeper.h"
#include "About.h"
#include "serial.h"
#include <time.h>

static unsigned long parse_ulong(char *str, int base);

int main(int argc, char **argv) {
sef_startup();
int player;
time_t t;
srand((unsigned) time(&t));
player = (int) parse_ulong(argv[1], 10);

	if (subscribe_all() != 0) 
		return 1;
	vg_init(GRAPHICS_MODE);
	start_menu(player);
	vg_exit();
	if (unsubscribe_all() != 0) 
		return 1;
	return 0;							
}

static unsigned long parse_ulong(char *str, int base){
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);			

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("penaltix: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
