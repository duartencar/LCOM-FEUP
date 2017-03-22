#ifndef __MOUSE_H
#define __MOUSE_H
#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "i8042.h"
#include "i8254.h"
#include "Bitmap.h"
#include "video_gr.h"

/**
 * @brief RB Mouse Machine
 */
typedef enum {RB_FREE,RB_PRESSED} state_mouse;

/**
 * @brief Mouse Struct
 */
typedef struct {
Bitmap *cursor;
unsigned int X;
unsigned int Y;
unsigned char packet[3];
unsigned int LB;
unsigned int MB;
unsigned int RB;
unsigned int XOV;
unsigned int YOV;
} Mouse;

/**
 * @brief Create a new Mouse.
 * @return Return NULL if fail, else return pointer to new Mouse.
 */
Mouse* create_mouse();

/**
 * @brief Delete Mouse mse.
 * @param mse Pointer to Mouse
 */
void delete_mouse(Mouse *mse);

/**
 * @brief Check if mouse is above button.
 * @param x_start X start position of button
 * @param y_start Y start position of button
 * @param mse Pointer to Mouse
 * @return Return 1 upon success and zero otherwise.
 */
int ismouse_aboveme(int x_start,int y_start,Mouse *mse);

/**
 * @brief Draw Mouse mse in Buffer.
 * @param mse Pointer to Mouse
 * @param background Buffer
 */
void draw_mouse(Mouse *mse,short *background);

/**
 * @brief Update Mouse Information.
 * @param mse Pointer to Mouse
 */
void mouse_update(Mouse *mse);

/**
 * @brief Write command in mouse after first command.
 * @param cmd Command
 * @return Return 0 upon success and non-zero otherwise.
 */
int write_mouse_2(unsigned char cmd);

/**
 * @brief Write WRITE_MOUSE command in mouse.
 * @return Return 0 upon success and non-zero otherwise.
 */
int write_mouse_1();

/**
 * @brief Read mouse.
 * @param info Pointer to information
 * @return Return 0 upon success and non-zero otherwise.
 */
int read_mouse(unsigned long *info);

/**
 * @brief Subscribe Mouse.
 * @return Return 0 upon success and negative otherwise.
 */
int mouse_subscribe(void);

/**
 * @brief Unsubscribe Mouse.
 * @return Return 0 upon success and negative otherwise.
 */
int mouse_unsubscribe(void);

/**
 * @brief Handler mouse packets.
 * @param packet Pointer to Mouse Packet
 * @return Return 0 upon success and negative otherwise.
 */
int mouse_handler(unsigned char* packet);

/**
 * @brief Get value of Bit bit in byte.
 * @param byte Byte
 * @param bit Bit
 * @return Return 0 upon success and negative otherwise.
 */
int bit_char(unsigned char byte,int bit);

/**
 * @brief Get value of X and Y of Mouse Packet.
 * @param x_y X or Y
 * @param packet Pointer to Mouse Packet
 * @return Return value of X/Y.
 */
short xy_final(char x_y,unsigned char* packet);

/**
 * @brief Get Slope of Mouse while RB PRESSED.
 * @param packet Pointer to Mouse Packet
 * @param slope Array with X and Y coordinates
 * @return Return 0 upon success and negative otherwise.
 */
int gesture_machine(unsigned char packet[],short *slope);

#endif
