#include <minix/sysutil.h>
#include "test3.h"
#include <minix/syslib.h>
#include <minix/drivers.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	/* DO NOT FORGET TO initialize service */
/* Initialize service */
  sef_startup();
  sys_enable_iop(SELF);

	if (argc == 1)
  {
		print_usage(argv); //Prints usage of the program if no arguments are passed

    return 0;
	}
	else
    return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"kbscan_test <0-C,1-assembly>\"\n"
			"\t service run %s -args \"kbscan_timer <tempo em segundos>\"\n"
			"\t service run %s -args \"testar_leds <0-Scroll Lock,1-Num Lock,2-Caps Lock>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short tipo,tempo,led, *vetor,elementos=0;

  int i;

	if (strncmp(argv[1], "kbscan_test", strlen("kbscan_test")) == 0)
  {
		if (argc != 3)
    {
			printf("kbd: wrong no. of arguments for kbd_test_scan()\n");

      return 1;
		}

    tipo = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */

    if (tipo == ULONG_MAX)
			return 1;

    printf("kbd::kbd_test_scan(%lu)\n", tipo);

    return kbd_test_scan(tipo);
	}
	else if (strncmp(argv[1], "kbscan_timer", strlen("kbscan_timer")) == 0)
  {
		if (argc != 3)
    {
			printf("kbd: wrong no. of arguments for kbd_test_timed_scan()\n");

      return 1;
		}

    tempo = parse_ulong(argv[2], 10);

    if (tempo == ULONG_MAX)
			return 1;

    printf("kbd::kbd_test_timed_scan(%lu)\n", tempo);

    return kbd_test_timed_scan(tempo);
	}
	else if (strncmp(argv[1], "testar_leds", strlen("testar_leds")) == 0)
  {
		if (argc < 3)
    {
			printf("kbd: wrong no of arguments for kbd_test_leds()\n");

      return 1;
		}

    elementos = argc - 2; // Número de argumentos depois do diretório e do comando

    //Alocar memória para o vetor
    vetor = (unsigned short*) malloc(elementos*sizeof(unsigned short));

    for(i = 0; i < elementos; i++)
    {
			if(parse_ulong(argv[i+2], 10) == ULONG_MAX)
        return 1;
			else
        vetor[i] = parse_ulong(argv[i+2], 10); // Guardar no vetor os argumentos
    }

		//Testar se todos os argumentos são 0,1 ou 2
		for(i = 0; i< elementos; i++)
    {
      if(vetor[i] < 0 || vetor[i] > 2)
      {
				printf("%d is not a valid LED!\n", vetor[i]);

        return 1;
      }
		}

		printf("kdb::kbd_test_leds(%d,'",elementos);

		for(i = 0; i < elementos; i++)
    {
      if(i==elementos-1)
        printf("%u')\n",vetor[i]);
			else
        printf("%u,",vetor[i]);
    }

		return kbd_test_leds(elementos,vetor);
	}
	else
  {
		printf("kbd: %s - no valid function!\n", argv[1]);

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
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0))
  {
		perror("strtoul");

    return ULONG_MAX;
	}

	if (endptr == str)
  {
		printf("timer: parse_ulong: no digits were found in %s\n", str);

    return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
