#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "Bitmap.h"
#include "Mouse.h"
#include "video_gr.h"
#include "goalkeeper.h"
#include "Ball.h"

/**
 * @brief Player Struct
 */
typedef struct {
Bitmap *stop;
Bitmap **run;
int atual;
Bitmap *shoot;
Bitmap **goal;
Bitmap *miss;
int goals,attempts;
short *slope;
GoalKeeper *GK;
Ball *ball;
int x,y;
} Player;

/**
 * @brief Create a new Player.
 * @param name Blue or Red
 * @return Return NULL if fail, else return pointer to new Player.
 */
Player* create_player(const char *name);

/**
 * @brief Delete Player player.
 * @param player Pointer to Player
 */
void delete_player(Player *player);

/**
 * @brief Initializes Player player in start position.
 * @param player Pointer to Player
 * @param background Buffer
 */
void set_player_stop(Player *player, short *background);

/**
 * @brief Initializes Player player in shoot position.
 * @param player Pointer to Player
 * @param background Buffer Array of Shoot/Save
 */
void set_player_shoot(Player *player, short **background);

/**
 * @brief Moves Player player int background.
 * @param player Pointer to Player
 * @param background Buffer
 * @return Return 1 upon success and zero otherwise.
 */
int move_player(Player *player, short *background);

/**
 * @brief Reaction of Player after shoot.
 * @param player Pointer to Player
 * @param background Buffer
 * @param goal Goal/Fail
 * @return Return 0 upon success and non-zero otherwise.
 */
int reaction_player(Player *player,short *background,int goal);

#endif
