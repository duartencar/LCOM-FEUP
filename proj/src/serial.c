#include "serial.h"

int serial_hook;

int serial_subscribe(){
	serial_hook = 3;

	//PROGRAM LINE CONTROL REGISTER TO PENALTIX
	unsigned long r,enable=0;
	sys_inb(COM1_PORT + LINE_CONTROL, &r);

	r &= BREAK_CONTROL;
	r |= DLAB;
	r |= EIGHT;
	r |= ONE_BIT;
	r |= NO_PARITY;

	sys_outb(COM1_PORT + LINE_CONTROL, r);
	serial_set_bitrate(SERIAL_BITRATE);

	//SUBSCRIBE INTERRUPTS
	if (sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_hook) != 0)
		return -1;

	if (sys_irqenable(&serial_hook) != 0)
		return -1;
	
	//ENABLE RECEIVED DATA INTERRUPT AND RECEIVER LINE STATUS INTERRUPT
	sys_inb(COM1_PORT + INTERRUPT_ENABLE, &r);
	enable = ENABLE_INTERRUPT_DATA | ENABLE_INTERRUPT_LS;
	r |= enable;
	sys_outb(COM1_PORT + INTERRUPT_ENABLE, r);
	tickdelay(micros_to_ticks(DELAY));
	return 0;
}

int serial_unsubscribe(){
	
	//UNSUBSCRIBE INTERRUPTS
	if (sys_irqdisable(&serial_hook) != 0)
		return -1;

	if (sys_irqrmpolicy(&serial_hook) != 0)
		return -1;
	
	//DISABLE ALL INTERRUPTS FROM UART
	if (sys_outb(COM1_PORT + INTERRUPT_ENABLE, 0) != OK)
		return -1;
	return 0;
}

void serial_set_bitrate(unsigned long b){
	/* Bit-rate is configured by loading the 16-bit DL register with the value given by 115200/b, where b is the bit-rate. Also, as already mentioned this value has to be split in its MSB and its LSB, and these bytes written to the DLM and DLL registers, respectively. Again, to access these registers you must first set the DLAB of the LCR to 1. After configuring the bit-rate you should reset the DLAB (to zero). */

unsigned long f = SERIAL_BASE_FREQ / b;
unsigned long msb = (f >> 8) & 0xFF;
unsigned long lsb = f & 0xFF;
unsigned long r = 0;

sys_inb(COM1_PORT + LINE_CONTROL, &r);
//SET DLAB of LCR to 1
r |= DLAB;
sys_outb(COM1_PORT + LINE_CONTROL, r);
//WRITE DLM and DLL
sys_outb(COM1_PORT + DIVISOR_LATCH_LSB, lsb);
sys_outb(COM1_PORT + DIVISOR_LATCH_MSB, msb);
//SET DLAB to 0 ( r XOR BIT(7) )
r ^= DLAB;
sys_outb(COM1_PORT + LINE_CONTROL, r);
}

/**
 * @brief Check if Transmitter Holding Register is empty in Line Status Register and send character.
 * @param character 8 Bit character
 */
void serial_send_char(unsigned char character){
	unsigned long status=0;
		//EMPTY UART TO BE ABLE TO SEND CHAR
		while(!(status & TH_EMPTY))
			sys_inb(COM1_PORT + LINE_STATUS_READ, &status);
	sys_outb(COM1_PORT + TRANSMITTER_HOLDING_WRITE, character);
	tickdelay(micros_to_ticks(DELAY));
}

/**
 * @brief Check in Line Status Register if is data available to read in Receiver Buffer Register, and read.
 * @return Return 8 Bit Character.
 */
unsigned long serial_read_char(){
	unsigned long status,r;
	sys_inb(COM1_PORT + LINE_STATUS_READ, &status);	
	//IF DATA_AVAILABLE -> READ
	if (status & RECEIVER_DATA){
		sys_inb(COM1_PORT + RECEIVER_BUFFER_READ, &r);
		return (unsigned char)(r & 0xFF);
	}
	return 1;
}

void serial_send_area(int area){
//SEND GOALKEEPER SAVE AREA
	serial_send_char(AREA);
	serial_send_char((unsigned char) area);
}

/**
 * @brief Read Save Area by handling char received.
 * @param game Pointer to Game_Multi
 * @param key 8 Bit Character Received
 */
void serial_read_area(Game_Multi *game,unsigned long key){
//READ GOALKEEPER SAVE AREA
	if (key == AREA)
		return;
	game->gk->save_area = (int) key;
}

void serial_send_shoot(short *slope){
//SEND SHOOT COORDINATES
int x,y,xx=0,yy=0;
x = (int) slope[0];
y = (int) slope[1];

serial_send_char(Y_SHOOT);
while(y > 0){
	if(y > 255){
		yy=255;
		y-=255;
	}
	else{
		yy=y;
		y=0;
	}
	serial_send_char((unsigned char) yy);
}
serial_send_char(END_Y_SHOOT);

serial_send_char(X_SHOOT);
while(x > 0){
	if(x > 255){
		xx=255;
		x-=255;
	}
	else{
		xx=x;
		x=0;
	}
	serial_send_char((unsigned char) xx);
}
serial_send_char(END_X_SHOOT);
}

/**
 * @brief Read Shoot Coordinates by handling char received.
 * @param game Pointer to Game_Multi
 * @param ball Pointer to Ball
 * @param key 8 Bit Character Received
 * @return Return 0 upon success and non-zero otherwise.
 */
int serial_read_shoot(Game_Multi *game,Ball *ball,unsigned long key){
//READ SHOOT COORDINATES
static unsigned int i=0,x=0,y=0;
	if (key == Y_SHOOT)
		return 1;
	else if(key == END_Y_SHOOT || key == X_SHOOT){
		i=1;
		return 1;
	}
	else if(key == END_X_SHOOT){	
		update_ball(ball,x,y);
		i=0;
		x=0;
		y=0;
		return 0;
	}

	if(!i)
		y += (unsigned int) key;
	else
		x += (unsigned int) key;
	return 1;
}

/**
 * @brief UART handling interrupt notification.
 * @param game Pointer to Game_Multi
 */
void serial_handle(Game_Multi *game){
	unsigned long d = 0;
	unsigned long r = 0;
		
		//GET INTERRUPT INFORMATION
		sys_inb(COM1_PORT + INTERRUPT_ID_READ, &r);
		if (r & NO_INTERUPTS)		
			return;
		else{
			//PRIORITY -> RECEIVER LINE STATUS (ERROR) / RECEIVED DATA READY (DATA) / TH EMPTY READY / MODEM STATUS
			if (r & RL_STATUS){
				sys_inb(COM1_PORT + LINE_STATUS_READ, &d);
				printf("error -> 0x%x\n", (unsigned long) (d & 0xFF));
			}
			if (r & DATA_AVAILABLE){
				d = serial_read_char();
				if (d){
					game->key = (unsigned long) (d & 0xFF);
					printf("data -> 0x%x\n", game->key);
				}
			}
		}
}
