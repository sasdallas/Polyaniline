/**
 * @file include/polyaniline/video.h
 * @brief Common video library
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_VIDEO_H
#define POLYANILINE_VIDEO_H

/**** INCLUDES ****/
#include <stdint.h>
#include <polyaniline/font.h>

/**** TYPES ****/

// Common video information
typedef struct _video_info {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
} video_info_t;


// This type is for any color. You can use the definitions or macros to interact, or make your own.
typedef union _color {
    unsigned long rgb;
    struct {
        unsigned char b;
        unsigned char g;
        unsigned char r;
    } c;
} color_t;

/**** MACROS ****/

// RGB manipulation functions
#define RGB_R(color) (color.c.r & 255)
#define RGB_G(color) (color.c.g & 255)
#define RGB_B(color) (color.c.b & 255)
#define RGB(red, green, blue) (color_t){.c.r = red, .c.g = green, .c.b = blue}

/**** DEFINITIONS ****/

// Defines for VGA text mode graphics converted to RGB
#define COLOR_BLACK             RGB(0, 0, 0)
#define COLOR_BLUE              RGB(0, 0, 170)
#define COLOR_GREEN             RGB(0, 170, 0)
#define COLOR_CYAN              RGB(0, 170, 170)
#define COLOR_RED               RGB(170, 0, 0)
#define COLOR_PURPLE            RGB(170, 0, 170)
#define COLOR_BROWN             RGB(170, 85, 0)
#define COLOR_GRAY              RGB(170, 170, 170)
#define COLOR_DARK_GRAY         RGB(85, 85, 85)
#define COLOR_LIGHT_BLUE        RGB(85, 85, 255)
#define COLOR_LIGHT_GREEN       RGB(85, 255, 85)
#define COLOR_LIGHT_CYAN        RGB(85, 255, 255)
#define COLOR_LIGHT_RED         RGB(255, 85, 85)
#define COLOR_LIGHT_PURPLE      RGB(255, 85, 255)
#define COLOR_YELLOW            RGB(255, 255, 85)
#define COLOR_WHITE             RGB(255, 255, 255)

// Default colors
#define BOOT_DEFAULT_BG     RGB(0, 0, 0)
#define BOOT_DEFAULT_FG     RGB(255, 255, 255)

#define BOOT_LIQUID_NORMAL     RGB(138, 226, 52)
#define BOOT_LIQUID_ERROR      RGB(145, 12, 14)


/**** FUNCTIONS ****/

/**
 * @brief Plot a pixel on X, Y
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color of the pixel
 */
void platform_putPixel(int x, int y, color_t color);

/**
 * @brief Clear the screen
 * @param color The color to clear with
 */
void platform_clearScreen(color_t color);

/**
 * @brief Draw a character to the screen
 * @param ch The character to draw
 * @param x The X coordinate
 * @param y The Y coordinate
 * @param fg The fg of the character
 * @param bg The bg of the character
 */
void platform_drawCharacter(int ch, int x, int y, color_t fg, color_t bg);


#endif
