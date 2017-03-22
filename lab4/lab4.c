#include <minix/drivers.h>
#include "test4.h"
#include "lab4_mouse.h"
#include "i8254.h"
#include "timer.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);
static unsigned short parse_ushort(char *str, int base);

int main(int argc, char **argv)
{	
	/* Initialize service */
  	sef_startup();

	if ( argc == 1 ) {
		print_usage(argv);
		return 0;
	} else {
		proc_args(argc, argv);
	}
	return 0;
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"test_packet <unsigned short cnt>\"\n"
			"\t service run %s -args \"test_async <unsigned short idle_time>\"\n"
			"\t service run %s -args \"test_config\"\n"
			"\t service run %s -args \"test_gesture <short length>\"\n",
			argv[0], argv[0], argv[0],argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short cnt,idle_time;
	short length;
	int i;
	
	if (strncmp(argv[1], "test_packet", strlen("test_packet")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_packet()\n");
			return 1;
		}
		cnt = parse_ushort(argv[2], 10);
		if (cnt == USHRT_MAX)
			return 1;
		printf("mouse::test_packet(%lu)\n", cnt);		
		return test_packet(cnt);
	} 
	else if (strncmp(argv[1], "test_async", strlen("test_async")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_async()\n");
			return 1;
		}
		idle_time = parse_ushort(argv[2], 10);
		if (cnt == USHRT_MAX)
			return 1;
		printf("mouse::test_async(%lu)\n", idle_time);
		return test_async(idle_time);
	}
	else if (strncmp(argv[1], "test_config", strlen("test_config")) == 0) {
		if (argc != 2) {
			printf("mouse: wrong no of arguments for test_config()\n");
			return 1;
		}
		printf("mouse::test_config()\n");	
		return test_config();
	}
	else if (strncmp(argv[1], "test_gesture", strlen("test_gesture")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no of arguments for test_gesture()\n");
			return 1;
		}
		length= (short) parse_ushort(argv[2],10);
		if (length == SHRT_MAX)
			return 1;
		printf("mouse::test_gesture(%d)\n", length);	
		return test_gesture(length);
	}
	else {
		printf("mouse: %s - no valid function!\n", argv[1]);
		return 1;
	} 
}

static unsigned long parse_ulong(char *str, int base)
{
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
		printf("mouse: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned short parse_ushort(char *str, int base) {
	char *endptr;
	unsigned long val;

	val = (unsigned short) strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("mouse: parse_ushort: no digits were found in %s \n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}
