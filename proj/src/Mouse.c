#include "Mouse.h"

int hook_mouse = 7;

Mouse* create_mouse(const char *type){
Mouse *mse;
mse = malloc(sizeof(Mouse));
if(mse == NULL)
	return NULL;

if(!strcmp(type,"menu") || !strcmp(type,"about") || !strcmp(type,"game"))
	mse->cursor = loadBitmap("/home/lcom/proj/src/images/mse.bmp");
else {
	free(mse);
	return NULL;
}

if(mse->cursor == NULL) {
	free(mse);
	return NULL;
	}
mse->X = HRES/2;
mse->Y = VRES/2;
return mse;
}

void delete_mouse(Mouse *mse){
deleteBitmap(mse->cursor);
free(mse);
mse = NULL;
}

int ismouse_aboveme(int x_start,int y_start,Mouse *mse){
int x,y;
x = mse->X;
y = mse->Y;

	if ( (x >= x_start) && (x <= x_start+290) && (y >= y_start) && (y <= y_start+88) )
		return 1;
return 0;
}

void draw_mouse(Mouse *mse,short *background){
int x,y;
x = mse->X - 30;
y = mse->Y - 30;
if(x <=0)
	x = 0; 
if(y <=0)
	y = 0;
draw_bitmap(mse->cursor,x,y,1,background);
}

void mouse_update(Mouse *mse){

mse->LB = bit_char(mse->packet[0],0);
mse->MB = bit_char(mse->packet[0],2);
mse->RB = bit_char(mse->packet[0],1);
mse->XOV = bit_char(mse->packet[0],6);
mse->YOV = bit_char(mse->packet[0],7);
mse->X += xy_final('X',mse->packet);
mse->Y += xy_final('Y',mse->packet);

if ( (int)(mse->X + xy_final('X',mse->packet)) >= HRES)
	mse->X = HRES;
else if ( (int)(mse->X + xy_final('X',mse->packet)) <= 0)
	mse->X = 0;

if ( (int)(mse->Y + xy_final('Y',mse->packet)) >= VRES)
	mse->Y = VRES;
else if ( (int)(mse->Y + xy_final('Y',mse->packet)) <= 0)
	mse->Y = 0;
}

int write_mouse_2(unsigned char cmd){
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

int write_mouse_1(){
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

int read_mouse(unsigned long *info){
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
	unsigned long info;

	if (sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE,&hook_mouse)!= OK)
		return -1;
	if (sys_irqenable(&hook_mouse) != OK) 
		return -1;

while(1){
	write_mouse_1();
	write_mouse_2(ENABLE_DATA);
	read_mouse(&info);
	if (info == ACK) 
		break; }
	
	return BIT(id);
}

int mouse_unsubscribe(void) {
	unsigned long info,tmp;

	while(1){
	write_mouse_1();
	write_mouse_2(DISABLE_DATA);
	write_mouse_1();
	write_mouse_2(ENABLE_STREAM_MODE);
	read_mouse(&info);
	if (info == ACK) 
		break; }	
	
	if (sys_irqdisable(&hook_mouse) != OK)
		return -1;	
	if (sys_irqrmpolicy(&hook_mouse) != OK)
		return -1;

	sys_inb(OUT_BUF, &tmp);	
	return 0;
}


int mouse_handler(unsigned char* packet){

static int contador = 0;
unsigned long info;

sys_inb(OUT_BUF,&info);
if(info == ACK && !contador)
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

short xy_final(char x_y,unsigned char* packet){
if (x_y == 'X'){

	if (bit_char(packet[0],4)) 
		return packet[1] - 256;
	else
		return packet[1];
		}

if (x_y == 'Y'){

	if (bit_char(packet[0],5)) 
		return -(packet[2] - 256);
	else 
		return -packet[2];
}
}

int gesture_machine(unsigned char packet[],short *slope){
static state_mouse gesture = RB_FREE;
static short y_slope=0,x_slope=0;
short y_packet,x_packet;

//Update 
y_packet = xy_final('Y',packet);
x_packet = xy_final('X',packet);

//STATE MACHINE
switch(gesture){
	case RB_FREE: 
		y_slope = 0;
		x_slope = 0;		
		if(bit_char(packet[0],1)) 
			gesture = RB_PRESSED; 
		return 1;
		break;
	case RB_PRESSED: 
		if(bit_char(packet[0],1)){
			y_slope += y_packet;
			x_slope += x_packet;
		}
		else{
			gesture = RB_FREE;
			slope[0] = x_slope;
			slope[1] = y_slope;
			return 0;
		}
		return 1;
		break;
}
}
