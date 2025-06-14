/**
 * @file include/polyaniline/terminal.h
 * @brief Terminal header file
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_TERMINAL_H
#define POLYANILINE_TERMINAL_H

/**** INCLUDES ****/
#include <stdint.h>
#include <polyaniline/video.h>


/**** VARIABLES ****/

/* X and Y */
extern int terminal_x;
extern int terminal_y;

/* Width and height */
extern int terminal_width;
extern int terminal_height;

/* Colors */
extern color_t terminal_fg;
extern color_t terminal_bg;

/**** FUNCTIONS ****/

/**
 * @brief Initialize the terminal system
 * @param vidinfo The video information set by the bootloader
 */
int terminal_init(video_info_t vidinfo);

/**
 * @brief Clear the terminal screen
 * @param fg The foreground to clear with
 * @param bg The background to clear with
 */
void terminal_clearScreen(color_t fg, color_t bg);

/**
 * @brief Put a character in the terminal system
 * @param ch The character to place
 */
void terminal_putCharacter(int ch);

/**
 * @brief Set foreground color
 * @param fg The foreground color
 */
void terminal_setForeground(color_t fg);

/**
 * @brief Set background color
 * @param bg The background color
 */
void terminal_setBackground(color_t bg);

/**
 * @brief Set X/Y
 */
void terminal_setXY(int x, int y);

/**
 * @brief Draw the Polyaniline test tube
 */
void terminal_drawTestTube(color_t liquid_color);

/**
 * @brief Draw a centered string
 * @param fmt The formatted, center string
 */
void terminal_printCentered(char *fmt, ...);



#endif