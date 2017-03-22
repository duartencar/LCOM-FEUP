#ifndef _GOALKEEPER_H_
#define _GOALKEEPER_H_

#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "Bitmap.h"
#include "Mouse.h"
#include "video_gr.h"
#include "keyboard.h"
#include "timer.h"

/**
 * @brief GoalKeeper Struct
 */
typedef struct {
int x,y;
int *keys,key_n;
int save_area;
Bitmap **area;
Bitmap **save;
} GoalKeeper;

/**
 * @brief Create a new GoalKeeper.
 * @return Return NULL if fail, else return pointer to new GoalKeeper.
 */
GoalKeeper* create_goalkeeper();

/**
 * @brief Delete GoalKeeper gk.
 * @param gk Pointer to GoalKeeper
 */
void delete_goalkeeper(GoalKeeper *gk);

/**
 * @brief Draw atual Save Area in Buffer.
 * @param gk Pointer to GoalKeeper
 * @param background Buffer
 */
void print_area(GoalKeeper *gk,short *background);

/**
 * @brief Read Keyboard Interrupt and Update GoalKeeper Area array.
 * @param gk Pointer to GoalKeeper
 * @return Return -1 if ESC, 0 if continue and 1 upon success.
 */
int choose_area(GoalKeeper *gk);

/**
 * @brief Update GoalKeeper Save Area.
 * @param gk Pointer to GoalKeeper
 */
void area(GoalKeeper *gk);

/**
 * @brief Randomize Save Area to gk.
 * @param gk Pointer to GoalKeeper
 */
void random_area(GoalKeeper *gk);

/**
 * @brief Reset GoalKeeper gk Save Area and Area array.
 * @param gk Pointer to GoalKeeper
 */
void reset_gk(GoalKeeper *gk);

/**
 * @brief Read Keyboard Interrupt and Update GoalKeeper Area array.
 * @param gk Pointer to GoalKeeper
 * @param background Buffer Array of Shoot/Save
 * @param reaction Buffer of Goal/Fail Reaction
 */
void update_goalkeeper(GoalKeeper *gk,short **background,short *reaction);

#endif
