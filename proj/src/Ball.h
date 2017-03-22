#ifndef _BALL_H_
#define _BALL_H_
#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "Bitmap.h"
#include "video_gr.h"
#include "goalkeeper.h"

/**
 * @brief Ball Sprite Struct
 */
typedef struct {
float x, y; 
float xspeed, yspeed; 
Bitmap **bmp;
int atual;
int ticks;
int interrupts;
int xf,yf;
int collision;
} Ball;

/**
 * @brief Create a new Ball Sprite.
 * @param team Blue or Red
 * @return Return NULL if fail, else return pointer to new Ball.
 */
Ball* create_ball(const char *team);

/**
 * @brief Delete Ball Sprite ball.
 * @param ball Pointer to Ball
 */
void delete_ball(Ball *ball);

/**
 * @brief Initializes ball in start position.
 * @param ball Pointer to Ball
 * @param background Buffer
 * @return Return 0 upon success and non-zero otherwise.
 */
int set_ball_pos(Ball *ball,short *background);

int round_number(double number);

/**
 * @brief Update Ball final position and sprite speed.
 * @param ball Pointer to Ball
 * @param xf X coordinate of final position
 * @param yf Y coordinate of final position
 */
void update_ball(Ball *ball,int xf,int yf);

/**
 * @brief Check if ball->atual is between limits.
 * @param ball Pointer to Ball
 */
void check_ball_atual(Ball *ball);

/**
 * @brief Move the ball in the right buffer.
 * @param ball Pointer to Ball
 * @param background Buffer Array of Shoot/Save
 * @return Return 0 upon success and non-zero otherwise.
 */
int move_ball(Ball *ball, short **background);

/**
 * @brief Randomize X coordinate.
 * @return Return Random Number.
 */
int random_goal_x();

/**
 * @brief Randomize Y coordinate.
 * @return Return Random Number.
 */
int random_goal_y();

/**
 * @brief Check if was goal or save.
 * @param ball Pointer to Ball
 * @return Return 0 upon success (goal) and non-zero otherwise (save).
 */
int goal_or_fail(Ball *ball);

/**
 * @brief Check if slope coordinates are between the limits.
 * @param slope Short Array with X and Y coordinates.
 */
void update_slope(short *slope);

/**
 * @brief Draw ball in buffer without writing in VRAM.
 * @param ball Pointer to Ball
 * @param background Buffer
 */
void draw_ball_buffer(Ball *ball,short *background);

/**
 * @brief Draw ball in buffer.
 * @param ball Pointer to Ball
 * @param background Buffer
 */
void draw_ball_waiting(Ball *ball,short *background);

#endif
