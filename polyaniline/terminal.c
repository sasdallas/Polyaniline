/**
 * @file polyaniline/terminal.c
 * @brief Small terminal implementation that can be initialized before the rest of Polyaniline
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/terminal.h>
#include <stdio.h>
#include <string.h>

/* X and Y */
int terminal_x = 0;
int terminal_y = 0;

/* Video info */
video_info_t info = { 0 };

/* Width and height */
int terminal_width = 0;
int terminal_height = 0;

/* Colors */
color_t terminal_fg;
color_t terminal_bg;



/**
 * @brief Initialize the terminal system
 * @param vidinfo The video information set by the bootloader
 */
int terminal_init(video_info_t vidinfo) {
    // Setup terminal variables
    terminal_width = vidinfo.width / FONT_CELL_WIDTH;
    terminal_height = vidinfo.height / FONT_CELL_HEIGHT;
    terminal_x = 0;
    terminal_y = 0;
    info = vidinfo; 

    // Clear screen
    terminal_clearScreen(BOOT_DEFAULT_FG, BOOT_DEFAULT_BG);

    return 0;
}

/**
 * @brief Clear the terminal screen
 * @param fg The foreground to clear with
 * @param bg The background to clear with
 */
void terminal_clearScreen(color_t fg, color_t bg) {
    terminal_fg = fg;
    terminal_bg = bg;
    platform_clearScreen(bg);

    terminal_x = 0;
    terminal_y = 0;
}



/**
 * @brief Put a character in the terminal system
 * @param ch The character to place
 */
void terminal_putCharacter(int ch) {
    if (!terminal_width || !terminal_height) return;

    switch (ch) {
        case '\n':
            // Newline
            terminal_x = 0;
            terminal_y++;
            break;

        case '\0':
            // Null character
            break;

        case '\t':
            // Tab
            for (int i = 0; i < 4; i++) terminal_putCharacter(' ');
            break;

        case '\r':
            // Return carriage
            terminal_x = 0;
            break;

        default:
            // Normal character
            platform_drawCharacter(ch, terminal_x, terminal_y, terminal_fg, terminal_bg);
            terminal_x++;
            break;
    }

    if (terminal_x >= terminal_width) {
        // End of the screen width
        terminal_y++;
        terminal_x = 0;
    }

    // Clear the screen if the height is too big
    if (terminal_y >= terminal_height) {
        terminal_clearScreen(terminal_fg, terminal_bg); // No scrolling yet
    }
}


/**
 * @brief Set X/Y
 */
void terminal_setXY(int x, int y) {
    terminal_x = x;
    terminal_y = y;
}




static void draw_square(int x, int y, color_t color) {
    int center_x = info.width - 30;
    int center_y = 30;

	for (int _y = 0; _y < 7; ++_y) {
		for (int _x = 0; _x < 7; ++_x) {
			platform_putPixel(center_x - 32 + x * 8 + _x, center_y - 32  + y * 8 + _y, color);
		}
	}
}

/**
 * @brief Draw the Polyaniline test tube
 */
void terminal_drawTestTube(color_t liquid_color) {
	uint64_t logo_tube = 0x3C24242424242400UL; 	// This is a clever trick - the binary values are encoded and bitshifted.
    uint64_t logo_fill = 0x18181800000000UL; 	// This is the filled part of our logo (the funny green liquid)

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			if (logo_tube & (1 << x)) {
				draw_square(x, y, (color_t){.rgb = 0xFFD3D7CF});
			}
		}
		logo_tube >>= 8;
	}

    for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			if (logo_fill & (1 << x)) {
				draw_square(x, y, liquid_color);
			}
		}
		logo_fill >>= 8;
	}
}


/**
 * @brief Set foreground color
 * @param fg The foreground color
 */
void terminal_setForeground(color_t fg) {
    terminal_fg = fg;
}

/**
 * @brief Set background color
 * @param bg The background color
 */
void terminal_setBackground(color_t bg) {
    terminal_bg = bg;
}

/**
 * @brief Draw a centered string
 * @param fmt The formatted, center string
 */
void terminal_printCentered(char *fmt, ...) {
    char fmt_buffer[256];
    
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(fmt_buffer, 256, fmt, ap);
    va_end(ap);

    // Calculate offset
    int off = (terminal_width - len) / 2;

    // The reason we can't set X to off is because we want to overwrite the existing characters
    terminal_setXY(0, terminal_y);
    for (int i = 0; i < off; i++) terminal_putCharacter(' ');
    for (int i = 0; i < len; i++) terminal_putCharacter(fmt_buffer[i]);
    for (int i = terminal_x; i < terminal_width-1; i++) terminal_putCharacter(' ');
    terminal_y++;
}