#include "video_gr.h"

#define VRAM_PHYS_ADDR	0xF0000000

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

int draw_bitmap(Bitmap *bmp,unsigned short x,unsigned short y,int i,short *background){

if(bmp == NULL || vram_size == 0)
	return 1;

short* aux_buffer = calloc(vram_size,sizeof(short));
bmp->x = x;
bmp->y = y;
if(!i) {drawBitmap(bmp,x,y,aux_buffer);
	memcpy(video_mem, (char*)aux_buffer, vram_size);
	memcpy((char *)background,video_mem,vram_size);}

else if (i==1){  memcpy((char *) aux_buffer, (char*)background, vram_size);
	drawBitmap_green(bmp,x,y,aux_buffer);
	memcpy(video_mem, (char*)aux_buffer, vram_size);}
else if (i==2) { drawBitmap_green(bmp,x,y,background);
		 memcpy(video_mem,(char*)background,vram_size);}
else if (i==3)
	drawBitmap_green(bmp,x,y,background);
else if(i==4) {
	memcpy((char *) aux_buffer, (char*)background, vram_size);
	drawBitmap_area(bmp,x,y,aux_buffer);
	memcpy(video_mem, (char*)aux_buffer, vram_size);
}
free(aux_buffer); 
return 0;
}

void draw_buffer(short *background){
	memcpy(video_mem,(char*)background,vram_size);
}

int draw_ball(Bitmap *bmp,unsigned short x,unsigned short y,short *background,int shoot){
if(bmp == NULL || vram_size == 0)
	return 1;

int collision;
short* aux_buffer = calloc(vram_size,sizeof(short));
memcpy((char *) aux_buffer, (char*)background, vram_size);
collision = drawBitmap_ball(bmp,x,y,aux_buffer,shoot);
memcpy(video_mem, (char*)aux_buffer, vram_size);
free(aux_buffer);
return collision;
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

unsigned int rgb565(unsigned char r, unsigned char g, unsigned char b){
	//return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
	return ((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11));
}
