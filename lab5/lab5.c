#include <minix/drivers.h>
#include "test5.h"
#include <sys/mman.h>
#include "i8254.h"
#include "timer.h"
#include "i8042.h"
#include "i8254.h"
#include "lmlib.h"
#include "vbe.h"
#include "video_gr.h"
#include <stdio.h>
#include <string.h>
#include "read_xpm.h"
#include "pixmap.h"
#include "sprite.h"

static unsigned short parse_ushort(char *str, int base);
static unsigned long parse_ulong(char *str, int base);
static int proc_args(int argc, char **argv);
static void print_usage(char **argv);
char** find_pixmap(char *string);

int main (int argc, char **argv) {
	/* Initialize service */
  	sef_startup();
  	if ( argc == 1 ) 
  	{
		print_usage(argv);
		return 0;
	}
	
	else return proc_args(argc, argv);

}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"test_init <unsigned short mode> <unsigned short delay>\"\n"
			"\t service run %s -args \"test_square <unsigned short x> <unsigned short y> <tamanho> <cor>\"\n"
			"\t service run %s -args \"test_line <unsigned short xi> <unsigned short yi> <unsigned short xf> <unsigned short yf> <cor>)\"\n"
			"\t service run %s -args \"test_xpm <unsigned short xi> <unsigned short yi> <char *xpm[]>\"\n"
			"\t service run %s -args \"test_move <unsigned short xi> <unsigned short yi> <char *xpm[]> <unsigned short hor> <short delta> <unsigned short time>))\"\n"
			"\t service run %s -args \"test_controller\"\n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	if(strncmp(argv[1], "test_init", strlen("test_init")) == 0)
	{
		unsigned short mode, delay;
		if(argc != 4)
		{
			printf("ERROR: wrong no. of arguments for test_init()\n");
			return 1;
		}
		mode = parse_ulong(argv[2], 16);		
		delay = parse_ushort(argv[3], 10);		

		printf("Graphic::test_init(%lu,%lu)\n", mode,delay);
		test_init(mode,delay);
		return 0;
	}

	else if(strncmp(argv[1], "test_square", strlen("test_square")) == 0)
	{
		unsigned short x, y, tamanho;

		unsigned long  color;

		if(argc != 6)
		{
			printf("ERROR: wrong no. of arguments for test_square()\n");
			return 1;
		}

		x = parse_ulong(argv[2], 10);		
		y = parse_ulong(argv[3],10);		
		tamanho = parse_ulong(argv[4],10);		
		color = parse_ulong(argv[5],10);
		

		printf("Graphic::test_square(%lu,%lu,%lu,%lu)\n", x,y,tamanho,color);
		return test_square(x,y,tamanho,color);
	}

	else if(strncmp(argv[1], "test_line", strlen("test_line")) == 0)
	{
		unsigned short xi, yi, xf, yf;
		unsigned long color;

		if(argc != 7)
		{
			printf("ERROR: wrong no. of arguments for test_line()\n");
			return 1;
		}

		xi = parse_ushort(argv[2],10);
		yi = parse_ushort(argv[3],10);
		xf = parse_ushort(argv[4],10);
		yf = parse_ushort(argv[5],10);
		color = parse_ulong(argv[6],10);
		
		printf("Graphic::test_line(%lu, %lu, %lu, %lu, %lu)\n",xi,yi,xf,yf,color);
		return test_line(xi,yi,xf,yf,color);
	}

	else if(strncmp(argv[1], "test_xpm", strlen("test_xpm")) == 0)
	{
		if(argc != 5)
		{
			printf("ERROR: wrong no. of arguments for test_xpm()\n");
			return 1;
		}

		unsigned short xi, yi;
		char **pixmap_xmp;

		xi = parse_ushort(argv[2],10);
		yi = parse_ushort(argv[3],10);

		//Find argv[4] in pixmap.h
		pixmap_xmp = find_pixmap(argv[4]);
		if(pixmap_xmp == NULL){
			printf("Failed to find pixmap!\n");
			return 1;
			}
		
		printf("Graphic::test_xpm(%lu, %lu, %s)\n",xi,yi,argv[4]);
		return test_xpm(xi,yi,pixmap_xmp);
	}

	else if(strncmp(argv[1], "test_move", strlen("test_move")) == 0)
	{
		if(argc != 8)
		{
			printf("ERROR: wrong no. of arguments for test_move()\n");
			return 1;
		}

		unsigned short xi, yi, hor, time;
		short delta;
		char **pixmap_xmp;

		xi = parse_ushort(argv[2],10);
		yi = parse_ushort(argv[3],10);		
	
		//Find argv[4] in pixmap.h
		pixmap_xmp = find_pixmap(argv[4]);
		if(pixmap_xmp == NULL){
			printf("Failed to find pixmap!\n");
			return 1;
			}
		
		hor = parse_ushort(argv[5],10);
		delta = (short) parse_ushort(argv[6],10);
		time = parse_ushort(argv[7],10);	

		printf("Graphic::test_move(%lu, %lu, %s, %lu, %d, %lu)\n", xi, yi, argv[4], hor, delta, time);
		return test_move( xi, yi, pixmap_xmp, hor, delta, time);

	}
	
	else if(strncmp(argv[1], "test_controller", strlen("test_controller")) == 0)
	{
		printf("Graphic::test_controller()\n");
		return test_controller();
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
		printf("graphic: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned short parse_ushort(char *str, int base) 
{
	char *endptr;
	unsigned long val;

	val = (unsigned short) strtoul(str, &endptr, base);

	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("graphic: parse_ushort: no digits were found in %s \n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

char** find_pixmap(char *string){
//pic1, pic2, cross, pic3 and penguin maps
char **xpm;
if (strncmp(string, "pic1", strlen("pic1")) == 0) {
	xpm = pic1;
		} 
else if (strncmp(string, "pic2", strlen("pic2")) == 0) {
	xpm = pic2;
		} 
else if (strncmp(string, "cross", strlen("cross")) == 0) {
	xpm = cross;
		} 
else if (strncmp(string, "pic3", strlen("pic3")) == 0) {
	xpm = pic3;
		} 
else if (strncmp(string, "penguin", strlen("penguin")) == 0) {
	xpm = penguin;
		} 
else {
	printf("Graphic::pixmap not found!\n");
	return NULL;
		}
return xpm;
}
