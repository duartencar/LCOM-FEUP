#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "vbe.h"
#include "Bitmap.h"

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

static unsigned int vram_base;  /* VRAM's physical addresss */
static unsigned int vram_size;  /* VRAM's size, but you can use
	                            the frame-buffer size, instead */
/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

/**
 * @brief Draw Bitmap bmp in Buffer background in mode i.
 * @param bmp Pointer to Bitmap Image
 * @param x X
 * @param y Y
 * @param i Mode
 * @param background Buffer
 * @return Return 0 upon success and non-zero otherwise.
 */
//int draw_bitmap(Bitmap *bmp,unsigned short x,unsigned short y,int i,short *background);

/**
 * @brief Draw Ball Bitmap bmp in Buffer background.
 * @param bmp Pointer to Ball Bitmap Image
 * @param x X
 * @param y Y
 * @param background Buffer
 * @param shoot Shoot ID
 * @return Return collision value.
 */
//int draw_ball(Bitmap *bmp,unsigned short x,unsigned short y,short *background,int shoot);

/**
 * @brief Draw Buffer background in VRAM.
 * @param background Buffer
 */
void draw_buffer(short *background);

/**
 * @brief RGB565 of (r,g,b).
 * @param r R
 * @param g G
 * @param b B
 * @return Return rgb565 value.
 */
unsigned int rgb565(unsigned char r, unsigned char g, unsigned char b);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
