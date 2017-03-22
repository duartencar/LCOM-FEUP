#ifndef GAME_MULTI_H
#define GAME_MULTI_H

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
#include "player.h"
#include "Menu.h"
#include "Ball.h"
#include "goalkeeper.h"
#include "serial.h"

#define SEC(x) 60*x

/**
 * @brief State Multiplayer Game Machine
 */
typedef enum{
P1,P2,
GAMEOVER_P1,GAMEOVER_P2,
WAIT_SHOOT_P1,WAIT_SHOOT_P2,
SHOOT_P1,SHOOT_P2,
WAITING_M,
RUN_P1,RUN_P2,
REACTION_P1,REACTION_P2,
WAIT_CONNECTION,READY,
DISCONNECTED
} state_game_multi;

/**
 * @brief State Connection Multiplayer Machine
 */
typedef enum{
PLAYING,WAIT_CON
} state_shoot_multi;

/**
 * @brief Mode Multiplayer Game Machine
 */
typedef enum {
	NORMAL_M,SUDDEN_DEATH_M
	} state_mode_multi;

/**
 * @brief Multiplayer Game Struct
 */
typedef struct {
state_game_multi penalty;
state_mode_multi mode;
state_shoot_multi shooting;
Mouse *mse;
short *slope;
Bitmap **wall_wait;
Bitmap *wallpapper;
short *background,*background_stop,*background_run,**background_shoot,*background_reaction;
Bitmap *numbers;
Bitmap **score;
Bitmap **gameover;
Bitmap **referee;
Player *p1;
Player *p2;
GoalKeeper *gk;
unsigned long key;
} Game_Multi;

/**
 * @brief Create new Game_Multi.
 * @return Return NULL if fail, else return pointer to new Game_Multi.
 */
Game_Multi* create_game_multi();

/**
 * @brief Delete Game_Multi game.
 * @param game Pointer to Game_Multi
 */
void delete_game_multi(Game_Multi *game);

/**
 * @brief Change Referee bmp.
 * @param game Pointer to Game_Multi
 */
void whistle_referee_multi(Game_Multi *game);

/**
 * @brief Draw Game Wallpapper and all game buffers.
 * @param game Pointer to Game_Multi
 * @param state State of Game_Multi
 * @param player Player 1 or 2
 */
void draw_gamewall_multi(Game_Multi *game,int state,int player);

/**
 * @brief Reset Scoreboard Goal green/red lights.
 * @param game Pointer to Game_Multi
 */
void reset_gamewall_multi(Game_Multi *game);

/**
 * @brief Update Game (STATE AND MODE).
 * @param game Pointer to Game_Multi
 */
void update_game_multi(Game_Multi *game);

/**
 * @brief Start New Multiplayer Game (against other player).
 * @param player Player 1 or 2
 * @return Return 0 upon success and non-zero otherwise.
 */
int start_game_multi(int player);

#endif
