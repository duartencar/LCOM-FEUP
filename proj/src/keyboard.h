#ifndef __KEYBOARD_LAB3_H
#define __KEYBOARD_LAB3_H


#define ESC 0x81
#define TAB 0x8F
#define ENTER 0x1C
#define TWO_BYTES 0xE0
#define UP_ARROW 0xE048
#define LEFT_ARROW 0xE04B
#define DOWN_ARROW 0xE050
#define RIGHT_ARROW 0xE04D 

/**
 * @brief Subscribe Keyboard Interrupts.
 * @return Return positive upon success and negative otherwise.
 */
int kbd_subscribe();

/**
 * @brief Unsubscribe Keyboard Interrupts.
 * @return Return 0 upon success and negative otherwise.
 */
int kbd_unsubscribe();

/**
 * @brief Read Keyboard Interrupt.
 * @return Return key upon success and negative otherwise.
 */
unsigned long ler_tecla();

#endif
