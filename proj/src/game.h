#ifndef _GAME_H_
#define _GAME_H_

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

#define SEC(x) 60*x

/**
 * @brief State Game Machine
 */
typedef enum{
USER,COM,
GAMEOVER_USER,GAMEOVER_COM,
WAIT_SHOOT_USER,WAIT_SHOOT_COM,
SHOOT_USER,SHOOT_COM,
WAITING,
RUN_USER,RUN_COM,
REACTION_USER,REACTION_COM
} state_game;

/**
 * @brief Mode Game Machine
 */
typedef enum {
	NORMAL,SUDDEN_DEATH
	} state_mode;

/**
 * @brief Game Struct
 */
typedef struct {
state_game penalty;
state_mode mode;
Mouse *mse;
short *slope;
Bitmap *wallpapper;
short *background,*background_stop,*background_run,**background_shoot,*background_reaction;
Bitmap *numbers;
Bitmap **score;
Bitmap **gameover;
Bitmap **referee;
Player *user;
Player *com;
GoalKeeper *gk;
} Game;

/**
 * @brief Create new Game.
 * @return Return NULL if fail, else return pointer to new game.
 */
Game* create_game();

/**
 * @brief Delete Game game.
 * @param game Pointer to Game
 */
void delete_game(Game *game);

/**
 * @brief Change Referee bmp.
 * @param game Pointer to Game
 */
void whistle_referee(Game *game);

/**
 * @brief Draw Game Wallpapper and all game buffers.
 * @param game Pointer to Game
 * @param state State of Game
 */
void draw_gamewall(Game *game,int state);

/**
 * @brief Reset Scoreboard Goal green/red lights.
 * @param game Pointer to Game
 */
void reset_gamewall(Game *game);

/**
 * @brief Update Game (STATE AND MODE).
 * @param game Pointer to Game
 */
void update_game(Game *game);

/**
 * @brief Start New Game (against COM).
 * @return Return 0 upon success and non-zero otherwise.
 */
int start_game();

#endif
