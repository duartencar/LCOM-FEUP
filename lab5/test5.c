#include <stdio.h>
#include "vbe_lab5.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "vbe.h"
#include "timer.h"
#include "keyboard_lab3.h"
#include "read_xpm.h"
#include "sprite.h"
#include "i8042.h"
	
void *test_init(unsigned short mode, unsigned short delay) {
	
	void* init = (void *) vg_init(mode);
	if (init != NULL) {
		timer_test_int(delay);
		vg_exit();
		printf("Adress -> 0x%x\n",init);
				}
	else {
		printf("Failed to vg_init() !\n");
		vg_exit();
		return NULL;
		}
	return init;
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

vg_init(GRAPHICS_MODE);
if(vg_square_draw(x,y,size,color)){
	vg_exit();
	printf("Failed to draw square! Check X,Y and SIZE values!\n");
	return 1;
} else { 
	kbd_out_esc();
	vg_exit();
	return 0;
	}
}	

int test_line(unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color) {

vg_init(GRAPHICS_MODE);
if(vg_line_draw(xi,yi,xf,yf,color)){
	vg_exit();
	printf("Failed to draw line! Check XI,YI,XF and YF values!\n");
	return 1;
} else { 
	kbd_out_esc();
	vg_exit();
	return 0;
		} 
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {	
int width, height;
char *map;

vg_init(GRAPHICS_MODE);
map = read_xpm(xpm, &width, &height);
if(map == NULL) {
	vg_exit();
	printf("Failed to read_xmp() !\n");
	return 1;
} else {
	if(vg_xmp_draw(map,xi,yi,width,height)){
		vg_exit();
		printf("Failed to vg_xmp_draw() !\n");
		return 1; 
	} else {
		kbd_out_esc();
		vg_exit();
		return 0;
		}
	}	
}	

int test_move(unsigned short xi, unsigned short yi, char *xpm[], unsigned short hor, short delta, unsigned short time) {

//X e Y inicial
if (xi < 0 || yi < 0 || delta == 0 || time <= 0){
	printf("Check XI,YI,DELTA and TIME values!\n");
	return 1; 
		}
//PARA CIMA
if (delta < 0 && !hor){
	if(yi-abs(delta) < 0){
		printf("Check YI and DELTA values!\n");
		return 1;
			}
		}
//PARA BAIXO
if (delta > 0 && !hor){
	if(yi+abs(delta) > VRES){
		printf("Check YI and DELTA values!\n");
		return 1;
			}
		}
//PARA A ESQUERDA
if (delta < 0 && hor){
	if(xi-abs(delta) < 0){
		printf("Check XI and DELTA values!\n");
		return 1;
			}
		}
//PARA A DIREITA
if (delta > 0 && hor){
	if(xi+abs(delta) > HRES){
		printf("Check XI and DELTA values!\n");
		return 1;
			}
		}

Sprite *movement;
int x_speed,y_speed;
float draw_speed;
// Velocidade = Distancia/Tempo
// Frequencia do Timer = 60
draw_speed = (float) delta / (60*time);
//Arredondar para a velocidade nao ser 0
if (delta<0 && abs(draw_speed)<=1) draw_speed = -1;
if (delta>0 && abs(draw_speed)<=1) draw_speed = 1;

//se "hor" for 0 -> vertical, senao horizontal
if(!hor){
	x_speed = 0;
	y_speed = draw_speed;
}
else {
	x_speed = draw_speed;
	y_speed = 0;
}

vg_init(GRAPHICS_MODE);
movement = create_sprite(xpm, xi, yi, x_speed, y_speed);

if(movement == NULL){
	vg_exit();
	printf("Failed to create_sprite() !\n");
	return 1;
		}
else {
	vg_xmp_draw(movement->map,movement->x,movement->y,movement->width,movement->height);
	//Interrupts do timer necessarios por cada frame
	float frame = (60*time) / abs(delta);
	if(frame < 1) frame = 1;
	int interrupts=0;
	int ipc_status;
	message msg;
	int r, byte = 0;
	unsigned int irq_set_kbd, irq_set_timer;
	unsigned long key, code;
	int timer=0;

	irq_set_kbd = kbd_subscribe();
	irq_set_timer = BIT(timer_subscribe_int());

	if (irq_set_kbd < 0 || irq_set_timer < 0) {
		vg_exit();
		printf("Erro ao ligar o interrupt\n");
		return 1;
		}

	while ((key != ESC) && (timer < 60*time)) { //Enquanto o code for diferente de ESC e o tempo diferente de "time" segundos
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_kbd) //Notificacao do keyboard
					key = ler_tecla();
				if (msg.NOTIFY_ARG & irq_set_timer){ //Notificacao do timer0
					if(interrupts == frame){
						//vg_reset_area(movement->x,movement->y,movement->width,movement->height);
						movement->x += movement->xspeed;
						movement->y += movement->yspeed;
						vg_xmp_draw(movement->map,movement->x,movement->y,movement->width,movement->height);
						interrupts=0;
						}
					timer++; //Incrementa contador
					interrupts++;
				}
				break;
			default:
				break;
			}
		} 
	}
	kbd_unsubscribe(); // Unsubscribes Keyboard Interrupts
	timer_unsubscribe_int(); // Unsubscribes Timer0 Interrupts
	destroy_sprite(movement);
	vg_exit();
	return 0;
	}	
}					

int test_controller() {	
	vbe_controller_info_t *vbe_info;
	vbe_info = malloc(sizeof(vbe_controller_info_t));
	
	void *information = (void *) vbe_info_controller(GRAPHICS_MODE,vbe_info);
	if(0){
		printf("Failed to vbe_info_controller() !\n");
		return 1;
	}
	else {
		//Imprimir -> Capabilities, Total Memory e VideoModePtr (Modos)
		printf("\n");
		printf("Capabilities -> ");
		if ( vbe_info->Capabilities[0] & BIT(0) ) 
			printf("DAC width is switchable to 8 bits per primary color\n");
		else 
			printf("DAC is fixed width, with 6 bits per primary color\n");

		if ( vbe_info->Capabilities[0] & BIT(1) ) 
			printf("                Controller is not VGA compatible\n");
		else 
			printf("                Controller is VGA compatible\n");

		if ( vbe_info->Capabilities[0] & BIT(2) ) 
			printf("                Large Blocks to RAMDAC\n");
		else 
			printf("                Normal RAMDAC operation\n");
	
		//Total Memory = Number of 64KB memory blocks
		printf("Total Memory -> %d KB\n",vbe_info->TotalMemory * 64);
		
		//Modos Suportados pelo VBE
		printf("VideoModePtr Modes -> ");
		//info do lm_init() + BASE + OFFSET
		short* VideoModes = information + ( ((int) vbe_info->VideoModePtr >> 12) & 0xF0000 ) + ( (int)vbe_info->VideoModePtr & 0x0FFFF );		
		//Termina quando for -1 (0FFFFh)
		while (*VideoModes != -1) 
			printf("0x%x  |  ",*VideoModes++);
		printf("\n");
		free(vbe_info);
		return 0;
	}	
}
