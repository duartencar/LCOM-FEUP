#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "lab4_mouse.h"
#include "i8042.h"
#include <stdlib.h>

int hook_mouse = 7;

int escrever_mouse_2(unsigned char cmd) {
	unsigned long status,info;
	/* loop while 8042 input buffer is not empty */
	while (1) {
		if (sys_inb(STAT_REG, &status) != OK)
			return 1; 	//Receber o Status do Keyboard
		if ((status & IBFULL) == 0) { //Se o Input Buffer Full estiver vazio
			if (sys_outb(IN_BUF, cmd) == OK)
				return 0;
						}
		tickdelay(micros_to_ticks(DELAY_US));
		}		
	return 1;
}

int escrever_mouse_1(){
	unsigned long status;
	/* loop while 8042 input buffer is not empty */
	while (1) {
		if (sys_inb(STAT_REG, &status) != OK)
			return 1; 	//Receber o Status do Keyboard
		if ((status & IBFULL) == 0) { //Se o Input Buffer Full estiver vazio
			if (sys_outb(STAT_REG, WRITE_MOUSE) == OK)
				return 0;
						}
		tickdelay(micros_to_ticks(DELAY_US));
		}
	return 1;
}

int ler_mouse(unsigned long *info){
	unsigned long status;
	while(1) {
		sys_inb(STAT_REG, &status); 
		if (status & OBFULL) {
			if (sys_inb(OUT_BUF,info) != OK)
				return -1;
			if ((status & (PAR_ERR | TO_ERR)) == 0)
				return 0;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int mouse_subscribe(void) {
	int id = hook_mouse;

	if (sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE,&hook_mouse)!= OK)
		return -1;
	if (sys_irqenable(&hook_mouse) != OK) 
		return -1;
	
	return id;
}

int mouse_unsubscribe(void) {
	unsigned long info;	
	
	if (sys_irqdisable(&hook_mouse) != OK)
		return -1;	
	if (sys_irqrmpolicy(&hook_mouse) != OK)
		return -1;
	
	return 0;
}


int mouse_handler(unsigned char packet[]){

static int contador = 0; //Não altera quando a função é chamada
unsigned long info;

sys_inb(OUT_BUF,&info);
if(info == ACK && !contador) //Se for ACK, não conta como packet
	return -1;



if (contador == 0){
	if( ((char) info) & FIRST_BYTE_PACKET) {
		packet[0] = (char) info;
		contador++;
		return 1; }}
else if (contador == 1){
	packet[1] = (char) info;
	contador++;
	return 1; }	
else if (contador == 2){
	packet[2] = (char) info;
	contador = 0;
	return 0;}
return -1;
}	

int bit_char(unsigned char byte,int bit){
return (byte & BIT(bit)) >> bit;
}

void print_packet(unsigned char packet[]){
printf("B1=0x%02X ",packet[0]); 
printf("B2=0x%02X ", packet[1]); 
printf("B3=0x%02X ", packet[2]); 
printf("LB=%d ",bit_char(packet[0],0)); 
printf("MB=%d ",bit_char(packet[0],2)); 
printf("RB=%d ", bit_char(packet[0],1)); 
printf("XOV=%d ", bit_char(packet[0],6)); 
printf("YOV=%d ",bit_char(packet[0],7));
printf("X=%d ",xy_final('X',packet));
printf("Y=%d\n",xy_final('Y',packet)); 
}

short xy_final(char x_y,unsigned char packet[]){
//Verificar Sinal de X e Y ( Positivo ou Negativo )
if (x_y == 'X'){

	if (bit_char(packet[0],4)) 
		return packet[1] - 256;
	else
		return packet[1];
		}
if (x_y == 'Y'){

	if (bit_char(packet[0],5)) 
		return packet[2] - 256;
	else
		return packet[2];
		}
}

typedef enum {RB_SOLTO,RB_PREMIDO} estados;
int gesture_machine(unsigned char packet[],short length,int negativo){
static estados gesture = RB_SOLTO;
static short y_slope=0;
short y_atual;

y_atual = xy_final('Y',packet); //Atualiza o valor de Y contido no packet[]
if (negativo) y_atual *= -1; // Se o length introduzido for negativo

switch(gesture){
case RB_SOLTO: y_slope = 0; //Deslocamento
	if(bit_char(packet[0],1)) 
		gesture = RB_PREMIDO; 
	return 1; //Retorna 1 para continuar a ler packets
	break;

case RB_PREMIDO: if(bit_char(packet[0],1) && y_atual > 0) //Garantir que RB é pressionado e que o movimento é contínuo
		y_slope += y_atual; 
		else
		gesture = RB_SOLTO;		
		if(y_slope >= length)
			return 0; //Retorna 0 para sair do while(1)
		return 1;
		break;
		}

}
