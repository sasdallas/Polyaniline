/**
 * @file polyaniline/error.c
 * @brief Error handler for Polyaniline
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/error.h>
#include <polyaniline/terminal.h>
#include <polyaniline/menu.h>
#include <polyaniline/interfaces/keyboard.h>
#include <stdio.h>
#include <stdarg.h>


/**
 * @brief Critically halt Polyaniline due to a fatal error
 * @param format The error format string
 */
void polyaniline_error(char *format, ...) {
    // terminal_clearScreen(BOOT_DEFAULT_FG, BOOT_DEFAULT_BG);
    terminal_drawTestTube(BOOT_LIQUID_ERROR);
    
    menu_drawTitleBar(RGB(255, 0, 0), "Polyaniline Error");
    terminal_setForeground(RGB(255, 0, 0));
    terminal_y++;

    UB_PRINT("An error occurred while booting. Please see details below.\n\n");

    // Copy to error buffer
    char errbuf[512];
    va_list ap;
    va_start(ap, format);
    int len = vsnprintf(errbuf, 512, format, ap);
    va_end(ap);

    // Print the message
    UB_PRINT("*** %s", errbuf);
    if (errbuf[len-2] == '\n') terminal_y--; // !!!: polyaniline_error() will take newlines.

    // Halt
    terminal_y++;

    UB_PRINT("Boot process halted. Please verify your configurations are valid.\n");
    UB_PRINT("Start an issue on GitHub if you require further assistance.\n");

    for (;;); 

    __builtin_unreachable();
}

/**
 * @brief Nonfatally error the system
 * @param format The error format string
 * 
 * @note This will redirect back to @c polyaniline_menu
 */
void polyaniline_error_nonfatal(char *format, ...) {
    terminal_clearScreen(BOOT_DEFAULT_FG, BOOT_DEFAULT_BG);
    terminal_drawTestTube(BOOT_LIQUID_ERROR);
    
    menu_drawTitleBar(RGB(255, 0, 0), "Polyaniline Error");
    terminal_setForeground(RGB(255, 0, 0));
    terminal_y++;

    UB_PRINT("An error occurred while booting. Please see details below.\n\n");

    // Copy to error buffer
    char errbuf[512];
    va_list ap;
    va_start(ap, format);
    int len = vsnprintf(errbuf, 512, format, ap);
    va_end(ap);

    // Print the message
    UB_PRINT("*** %s", errbuf);
    if (errbuf[len-2] == '\n') terminal_y--; // !!!: polyaniline_error() will take newlines.

    // Halt
    terminal_y++;

    UB_PRINT("Press OK to continue and return to the Polyaniline menu...\n");

    // "   OK   "
    terminal_setXY((terminal_width - 8) / 2, terminal_height - ub_offset_y);
    terminal_setBackground(RGB(255, 255, 255));
    terminal_setForeground(RGB(0, 0, 0));
    printf("   OK   ");
    while (platform_readKeyboard(0) != KEYBOARD_ENTER);
    terminal_setBackground(RGB(0, 0, 0));
    terminal_setForeground(RGB(255, 255, 255));

    polyaniline_menu();
}
