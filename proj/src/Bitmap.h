#ifndef __BITMAP__H
#define __BITMAP__H

#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include "video_gr.h"
#include "vbe.h"

/**
 * @brief bitmap file header
 */
typedef struct {
    unsigned short type; // specifies the file type
    unsigned int size; // specifies the size in bytes of the bitmap file
    unsigned int reserved; // reserved; must be 0
    unsigned int offset; // specifies the offset in bytes from the bitmapfileheader to the bitmap bits
} BitmapFileHeader;

/**
 * @brief bitmap info header
 */
typedef struct {
    unsigned int size; // specifies the number of bytes required by the struct
    int width; // specifies width in pixels
    int height; // specifies height in pixels
    unsigned short planes; // specifies the number of color planes, must be 1
    unsigned short bits; // specifies the number of bit per pixel
    unsigned int compression; // specifies the type of compression
    unsigned int imageSize; // size of image in bytes
    int xResolution; // number of pixels per meter in x axis
    int yResolution; // number of pixels per meter in y axis
    unsigned int nColors; // number of colors used by the bitmap
    unsigned int importantColors; // number of colors that are important
} BitmapInfoHeader;

/**
 * @brief bitmap struct
 */
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;
    short* bitmapData;
    int x,y;
} Bitmap;

/**
 * @brief Loads a bmp image
 *
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draw bitmap in buffer, starting in position (X,Y).
 * @param bmp Pointer to Ball
 * @param x X
 * @param y Y
 * @param buffer Buffer
 */
void drawBitmap(Bitmap* bmp, int x, int y,short *buffer);

/**
 * @brief Draw bitmap,without rgb(0,255,0) pixels, in buffer, starting in position (X,Y).
 * @param bmp Pointer to Ball
 * @param x X
 * @param y Y
 * @param buffer Buffer
 */
void drawBitmap_green(Bitmap* bmp, int x, int y,short *buffer);

/**
 * @brief Check if ball is above player.
 * @param buffer Pixel Color
 * @param shoot Shoot ID
 * @return Return 0 upon success (not above) and non-zero otherwise (above).
 */
int isball_aboveplayer(short buffer,int shoot);

/**
 * @brief Draw Ball Bitmap in buffer,without rgb(0,255,0) pixels, starting in position (X,Y).
 * @param bmp Pointer to Ball
 * @param x X
 * @param y Y
 * @param buffer Buffer Array of Shoot/Save
 * @param shoot Shoot ID
 * @return Return 1 if collision with GoalKeeper, 0 if not.
 */
int drawBitmap_ball(Bitmap* bmp, int x, int y,short *buffer,int shoot);

/**
 * @brief Draw Area Bitmap,without rgb(0,255,0) pixels, in buffer, starting in position (X,Y).
 * @param bmp Pointer to Ball
 * @param x X
 * @param y Y
 * @param buffer Buffer
 */
void drawBitmap_area(Bitmap* bmp, int x, int y,short *buffer);

/**
 * @brief Draw Number,without rgb(0,255,0) pixels, in buffer, starting in position (X,Y).
 * @param bmp Pointer to Ball
 * @param str User or Com
 * @param number Number to draw
 * @param buffer Buffer
 */
void drawBitmap_number(Bitmap* bmp,const char *str, int number,short *buffer);

/**
 * @brief Delete Bitmap bmp.
 * @param bmp Pointer to Ball
 */
void deleteBitmap(Bitmap* bmp);

#endif /* __BITMAP_H */
