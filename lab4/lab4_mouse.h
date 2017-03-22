#ifndef __LAB4_MOUSE_H
#define __LAB4_MOUSE_H
#include "i8042.h"

int escrever_mouse(unsigned char cmd);

int ler_mouse(unsigned long *info);

int mouse_subscribe();

int mouse_unsubscribe();

int mouse_handler(unsigned char packet[]);

int bit_char(unsigned char byte,int bit);

void print_packet(unsigned char packet[]);

short xy_final(char x_y,unsigned char packet[]);

int gesture_machine(unsigned char packet[],short length,int negativo);

#endif
