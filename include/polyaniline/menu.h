/**
 * @file include/polyaniline/menu.h
 * @brief Polyaniline boot menu
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_MENU_H
#define POLYANILINE_MENU_H

/**** INCLUDES ****/
#include <stdint.h>
#include <polyaniline/video.h>

/**** DEFINITIONS ****/

/* Graphical setting to show how large our "user box" (i.e. user interaction box) should be */
#define USERBOX_WIDTH           80
#define USERBOX_HEIGHT          20

/* Max big options */
#define MAX_OPTIONS             20-2
#define MAX_PAGES               4

/* Option types */
#define OPTION_TYPE_CHECKBOX    1
#define OPTION_TYPE_SELECT      2

/* Default titlebar stuff */
#define TITLEBAR_DEFAULT_COLOR  RGB(0, 0, 125)

/**** TYPES ****/

typedef struct option {
    const char *name;           // Option name
    const char *subtitle1;      // Subtitle used for help text (line 1)
    const char *subtitle2;      // Subtitle used for help text (line 2)
    int type;                   // Type
    int enabled;                // (Checkbox) Enabled/disabled
    void *data;                 // Custom data
    int inv;                    // Cosmetic. Inverts the actual look of the checkbox
} option_t;

/**** MACROS ****/

#define OPTIONS_START() option_t options[MAX_PAGES][MAX_OPTIONS] = { NULL }; int optcount = 0; int pages = 0; int selected = 0; int selected_page = 0;
#define OPTION_SELECT(n, s1, s2) { options[pages][optcount].name = " " n; options[pages][optcount].subtitle1 = s1; options[pages][optcount].subtitle2 = s2; options[pages][optcount].type = OPTION_TYPE_SELECT; optcount++; if (optcount >= MAX_OPTIONS) { optcount = 0; pages++; }}
#define OPTION_CHECKBOX(n, s1, s2, default) { options[pages][optcount].name = n; options[pages][optcount].subtitle1 = s1; options[pages][optcount].subtitle2 = s2; options[pages][optcount].type = OPTION_TYPE_CHECKBOX; options[pages][optcount].enabled = default; optcount++; if (optcount >= MAX_OPTIONS) { optcount = 0; pages++; }}
#define OPTIONS_LOOP() { \
    _options_loop: \
    while (1) { \
        int len = 0; \
        terminal_setXY(ub_offset_x, ub_offset_y + 2); \
        for (int i = 0; i < MAX_OPTIONS; i++) { \
            if (options[selected_page][i].type) { menu_renderOption(&options[selected_page][i], (i == selected), selected_page, pages); len++; } \
            else { terminal_setXY(ub_offset_x, terminal_y); for (int i = ub_offset_x; i < terminal_width - ub_offset_x; i++) terminal_putCharacter(' '); terminal_y++;  } \
        }\
        int k = platform_readKeyboard(0); \
        if (k == KEYBOARD_DOWN && selected < len-1) selected++; \
        if (k == KEYBOARD_UP && selected > 0) selected--; \
        if (k == KEYBOARD_LEFT && selected_page > 0) selected_page--; \
        if (k == KEYBOARD_RIGHT && selected_page < pages) selected_page++; \
        if (k == KEYBOARD_ENTER) {\
            if (options[selected_page][selected].type == OPTION_TYPE_CHECKBOX) options[selected_page][selected].enabled ^= 1; \
            else break; \
        }\
        if (k == KEYBOARD_ESC) { selected = -1; break; } \
        }}

#define UB_PRINT(...) { terminal_setXY(ub_offset_x, terminal_y); printf(__VA_ARGS__); }

/**** VARIABLES ****/

/**
 * @brief Userbox offset (X)
 */
extern int ub_offset_x;

/**
 * @brief Userbox offset (Y)
 */
extern int ub_offset_y;

/**** FUNCTIONS ****/

/**
 * @brief Start the Polyaniline boot menu
 */
void polyaniline_menu();

/**
 * @brief Helper function to render an option
 * @param opt The option
 * @param selected 1 if this option is selected
 * @param page The current page
 * @param pages The total amount of pages
 */
void menu_renderOption(option_t *opt, int selected, int page, int pages);

/**
 * @brief Helper function to draw a titlebar
 * @param bg The background to use for the titlebar 
 */
void menu_drawTitleBar(color_t bg, char *str);


#endif