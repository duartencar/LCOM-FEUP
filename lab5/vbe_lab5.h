#ifndef __vbe_lab5_H
#define __vbe_lab5_H


#define BIT(n) (0x01<<(n))
#define DELAY_US 20000
#define SET_VBE_MODE 0x4F02
#define GRAPHICS_MODE 0x105
#define INFORMATION_MODE 0x4F01
#define CONTROLLER_MODE 0x4F00
#define BIOS_VBE 0x10
#define VRAM_PHYS_ADDR	0xF0000000
#define HRES             1024
#define VRES		  768
#define BITS_PER_PIXEL	  8


#endif
