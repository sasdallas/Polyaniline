/**
 * @file include/polyaniline/interfaces/keyboard.h
 * @brief Keyboard interface
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_INTERFACES_KEYBOARD_H
#define POLYANILINE_INTERFACES_KEYBOARD_H

/**** INCLUDES ****/
#include <stdint.h>

/**** DEFINITIONS ****/

/* Basic keyboard scancodes */

#define KEYBOARD_DOWN       0xAA // If it wasn't obvious these aren't actual PS/2 scancodes
#define KEYBOARD_UP         0xBB
#define KEYBOARD_LEFT       0xCC
#define KEYBOARD_RIGHT      0xDD
#define KEYBOARD_ENTER      0xEE
#define KEYBOARD_ESC        0xEF

/**** FUNCTIONS ****/

/**
 * @brief Read a keyboard key with a specific timeout
 * @param timeout Timeout in seconds or 0 seconds if not needed
 */
int platform_readKeyboard(int timeout);

#endif