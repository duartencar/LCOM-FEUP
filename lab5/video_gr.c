#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "vbe.h"
#include "vbe_lab5.h"
#include "timer.h"
#include "read_xpm.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

static unsigned int vram_base;  /* VRAM's physical addresss */
static unsigned int vram_size;  /* VRAM's size, but you can use
	                            the frame-buffer size, instead */

void *vg_init(unsigned short mode) {
	struct reg86u reg86;
	vbe_mode_info_t info_vbe;
	reg86.u.b.intno = BIOS_VBE; 
	reg86.u.w.ax = SET_VBE_MODE;
	reg86.u.w.bx = BIT(14) | mode;
	
	 if( sys_int86(&reg86) != OK ) {
     		printf("\tvg_init(): sys_int86() failed \n");
		return NULL; }

	if (vbe_get_mode_info(mode, &info_vbe) != 0)
		return NULL;

	h_res = info_vbe.XResolution;
	v_res = info_vbe.YResolution;
	bits_per_pixel = info_vbe.BitsPerPixel;

	int r;
	struct mem_range mr;
	
	void *video_memory;         /* frame-buffer VM address */ 

	/* Allow memory mapping */
	vram_base = info_vbe.PhysBasePtr;
	vram_size = h_res * v_res * bits_per_pixel/8;
	mr.mr_base = (phys_bytes) vram_base;	
	mr.mr_limit = mr.mr_base + vram_size;  
	
	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
	   panic("sys_privctl (ADD_MEM) failed: %d\n", r);
	
	/* Map memory */	
	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
		
	if(video_mem == MAP_FAILED)
	   panic("couldn't map video memory");	

	video_memory = video_mem;
	return video_memory;
}

void vg_pixel(unsigned short x, unsigned short y, unsigned long color) {
	//Endereço do pixel a que vamos atribuir a cor
	*(video_mem + y*h_res + x) = color;
						}

void vg_pixel_buffer(unsigned short x, unsigned short y, unsigned long color,char *aux_buffer) {
	//Endereço do pixel a que vamos atribuir a cor
	//Escreve no buffer para depois ser copiado para a VRAM
	*(aux_buffer + y*h_res + x) = color;
						}
	
int vg_square_draw(unsigned short x, unsigned short y, unsigned short size, unsigned long color){
int i,j;

if (x < 0 || x + size > h_res || y < 0 || y + size > v_res) 
		return 1;

for (i = x; i < x+size; i++) {
	for (j = y; j < y+size; j++) 		
		vg_pixel(i,j,color);
				}
return 0;
}

int vg_line_draw(unsigned short x, unsigned short y, unsigned short x_2,unsigned short y_2, unsigned long color){
/* Y = M*X + B
Pi = (x,y)
Pf = (x_2,y_2)
P = Pf-Pi = (x_2 - x,y_2 - y)
M = y(P) / x(P)
B = Y - M*X
Delta X = x_2 - x;
*/

if (x < 0 || x_2 > h_res || y < 0 || y_2 > v_res) 
		return 1;

int i,prints;
int py,px;
float x_m,y_m;
float xx=x,yy=y;
px = x_2 -x;
py = y_2 -y;

//Se o deslocamento de X for superior a Y, desenha em função da posição de X, caso contrário desenha em função de Y para não ficar um traço descontínuo, pois sao precisos mais passos na coordenada y
if(abs(px) >= abs(py))
	prints = abs(px);
else
	prints = abs(py);
//Calcula o valor a incrementar em cada passo em x e y
x_m = (float) px / prints;
y_m = (float) py / prints;

//Imprime o pixel inicial
vg_pixel(x,y,color);

//Imprime a reta
for(i=0;i<prints;i++){
	xx+= x_m;
	yy+= y_m;
	vg_pixel(xx,yy,color);
	}
return 0; 
}

int vg_xmp_draw(char *xmp_map,unsigned short x, unsigned short y,int width, int height){
int i,j,n=0;

if (x < 0 || x + width > h_res || y < 0 || y + height > v_res) 
		return 1;
//Alocar memoria num buffer auxiliar e inicializa a 0
char* aux_buffer = (char *) malloc(vram_size);
memset(aux_buffer,0,vram_size);

//Como os primeiros 1024 valores de xmp_map correspondem a y=0, "escreve-se" na lateral a medida que se desce
for (j = y; j < y+height; j++) {
	for (i = x; i < x+width; i++){
		vg_pixel_buffer(i,j,xmp_map[n],aux_buffer);
		n++;
		}
	}
//Copia para a VRAM e desenha a figura
memcpy(video_mem, aux_buffer, vram_size);
//Liberta a memoria usada no buffer auxiliar
free(aux_buffer);
return 0;
}

int vg_reset_area(unsigned short x, unsigned short y,int width, int height){
int i,j;
for (j = y; j < y + height; j++) {
	for (i = x; i < x + width; i++)
		vg_pixel(i,j,0);
				}
return 0;
}

int vg_exit() {
  struct reg86u reg86;
  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}
