#ifndef _MENU_H_
#define _MENU_H_

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
#include "rtc.h"
#include "game.h"
#include "game_multi.h"
#include "About.h"

#define BIT(n) (0x01<<(n))
#define IRQ_MOUSE_HOOK BIT(7)
#define IRQ_KBD_HOOK BIT(8)
#define IRQ_TIMER_HOOK BIT(10)
#define IRQ_SERIAL_HOOK BIT(3)

#define DAY(x) (x>=8 && x<=16)
#define SUNSET(x) (x>=17 && x<=20)
#define NIGHT(x) ((x>=21 && x<=23) || (x>=0 && x<=7))

#define DAY_MODE 1
#define SUNSET_MODE 2
#define NIGHT_MODE 3

/**
 * @brief Menu Option Machine
 */
typedef enum {
	MENU_START,MENU_PLAY,MENU_ABOUT,MENU_EXIT,PLAY_SINGLE,PLAY_MULTI,PLAY_BACK
	} state_option;

/**
 * @brief Menu State Machine
 */
typedef enum {
	MENU_NORMAL,MENU_GAME
	} state_menu;

/**
 * @brief Menu Struct
 */
typedef struct {
Bitmap *wallpapper;
Mouse *mse;
state_option option;
state_menu game;
int selected,above;
short *background;
Bitmap **buttons;
} MenuInfo;

/**
 * @brief Subscribe Mouse, Keyboard and Timer.
 * @return Return 0 upon success and non-zero otherwise.
 */
int subscribe_all();

/**
 * @brief Unsubscribe Mouse, Keyboard and Timer.
 * @return Return 0 upon success and non-zero otherwise.
 */
int unsubscribe_all();

/**
 * @brief Create a new MenuInfo.
 * @return Return NULL if fail, else return pointer to new MenuInfo.
 */
MenuInfo* create_menu();

/**
 * @brief Delete MenuInfo menu.
 * @param menu Pointer to MenuInfo
 */
void delete_menu(MenuInfo *menu);

/**
 * @brief Draw MenuInfo in VRAM.
 * @param menu Pointer to MenuInfo
 */
void draw_menu(MenuInfo *menu);

/**
 * @brief Equalize MenuInfo Option and MenuInfo Selected .
 * @param menu Pointer to MenuInfo
 * @param selected Pointer to MenuInfo Selected
 */
void selected_state(MenuInfo *menu,int *selected);

/**
 * @brief Change MenuInfo Selected by Mouse position.
 * @param menu Pointer to MenuInfo
 * @param selected Pointer to MenuInfo Selected
 */
void menu_mouse_option(MenuInfo *menu,int *selected);

/**
 * @brief Change MenuInfo Selected by Keyboard.
 * @param menu Pointer to MenuInfo
 * @param selected Pointer to MenuInfo Selected
 */
void menu_keyboard_option(MenuInfo *menu,int *selected);

/**
 * @brief Start new MenuInfo.
* @return Return 0 upon success and non-zero otherwise.
 */
int start_menu();

#endif
