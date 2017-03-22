#ifndef _ABOUT_H_
#define _ABOUT_H_

#include <stdio.h>
#include <stdlib.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "Bitmap.h"
#include "video_gr.h"
#include "keyboard.h"
#include "timer.h"
#include "Menu.h"

/**
 * @brief About Menu Struct
 */
typedef struct {
Bitmap *wallpapper;
Bitmap **buttons;
Bitmap **info;
short *background;
Mouse *mse;
int selected,above;
int option;
} About;

/**
 * @brief Create a new About.
 * @return Return NULL if fail, else return pointer to new About.
 */
About* create_about();

/**
 * @brief Delete About about.
 * @param about Pointer to About
 */
void delete_about(About *about);

/**
 * @brief Draw About in VRAM by button selected.
 * @param about Pointer to About
 * @param button Button selected by Mouse or Keyboard
 */
void draw_about(About *about,int button);

/**
 * @brief Change About Selected by Mouse position.
 * @param about Pointer to About
 * @param selected Pointer to About Selected
 */
void about_mouse_option(About *about,int *selected);

/**
 * @brief Change About Selected by Keyboard.
 * @param about Pointer to About
 * @param selected Pointer to About Selected
 */
void about_keyboard_option(About *about,int *selected);

/**
 * @brief Start new About.
* @return Return 0 upon success and non-zero otherwise.
 */
int start_about();

#endif
