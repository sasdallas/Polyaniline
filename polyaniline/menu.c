/**
 * @file polyaniline/menu.c
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

#include <polyaniline/menu.h>
#include <polyaniline/interfaces/keyboard.h>
#include <polyaniline/video.h>
#include <polyaniline/platform.h>
#include <polyaniline/terminal.h>
#include <polyaniline/config.h>
#include <polyaniline/config_opts.h>
#include <polyaniline/error.h>
#include <stdio.h>
#include <string.h>

/* Precalculated userbox offset */
int ub_offset_x = 0;
int ub_offset_y = 0;

/**
 * @brief Helper function to draw a titlebar
 */
void menu_drawTitleBar(color_t bg, char *str) {
    int i = ub_offset_x;

    terminal_setXY(i, ub_offset_y);
    terminal_setBackground(bg);

    size_t len = strlen(str);
    int off = (terminal_width - len) / 2;

    for (; i < off; i++) terminal_putCharacter(' ');
    while (*str) terminal_putCharacter(*str++);

    i += len;
    for (; i < terminal_width - ub_offset_x; i++) terminal_putCharacter(' ');

    terminal_setBackground(BOOT_DEFAULT_BG);

    terminal_y++;
}

/**
 * @brief Helper function to render an option
 */
void menu_renderOption(option_t *opt, int selected, int page, int pages) {
    if (selected) {
        // Also render the subtitle text
        int prev_y = terminal_y;
        terminal_setXY(0, terminal_height - ub_offset_y + 1);
        terminal_printCentered("Page %d / %d - <ENTER> = select, <ESC> = back, \030/\031 = up/down, \033/\032 = prev/next page", page+1, pages+1); // Helper text (TODO: move to section?)
        terminal_printCentered("%s", (opt->subtitle1 ? opt->subtitle1 : " "));
        terminal_printCentered("%s", (opt->subtitle2 ? opt->subtitle2 : " "));
        terminal_setXY(0, prev_y);

        // Now set the colors to invert
        terminal_setBackground(RGB(207, 215, 211)); // Soft gray, don't know where I got it from
        terminal_setForeground(RGB(0, 0, 0));
    } else {
        // Set the colors to normal
        terminal_setForeground(RGB(207, 215, 211));
        terminal_setBackground(RGB(0, 0, 0));
    }

    terminal_setXY(ub_offset_x, terminal_y);
    size_t l = strlen(opt->name) + (opt->type == OPTION_TYPE_CHECKBOX ? 4 : 0);

    switch (opt->type) {
        case OPTION_TYPE_CHECKBOX:
            // Checkbox. Invert if the user wants (its for cosmetics, weird me thing)
            printf(" [%s] %s", (((opt->enabled && !opt->inv) || (!opt->enabled && opt->inv)) ? "+" : " "), opt->name);
            break;

        case OPTION_TYPE_SELECT:
        default:
            printf("%s", opt->name);
    }
    
    for (int i = ub_offset_x + l; i < terminal_width - ub_offset_x; i++) terminal_putCharacter(' ');

    terminal_y++;

    terminal_setForeground(BOOT_DEFAULT_FG);
    terminal_setBackground(BOOT_DEFAULT_BG);
}

/**
 * @brief Draw the Polyaniline copyright
 */
void polyaniline_copyright() {
    int prev_x = terminal_x;
    int prev_y = terminal_y;
    terminal_setXY(0, terminal_height - 2);
    terminal_setForeground(RGB(105, 105, 105));
    terminal_printCentered("Polyaniline %d.%d.%d-%s (codename \"%s\")",
            __polyaniline_version_major,
            __polyaniline_version_minor,
            __polyaniline_version_lower,
            __polyaniline_build_type,
            __polyaniline_version_codename);

    terminal_printCentered("(C) reduceOS 2025 - https://github.com/sasdallas/reduceOS");
    terminal_setXY(prev_x, prev_y);
    terminal_setForeground(BOOT_DEFAULT_FG);
}

/**
 * @brief OS configuration menu
 * 
 * Options are exposed in a static variable located in @c config_opts.h
 * This will construct and use a command line to start the OS
 */
void polyaniline_configureOS() {
    terminal_clearScreen(terminal_fg, terminal_bg);
    terminal_drawTestTube(BOOT_LIQUID_NORMAL);
    polyaniline_copyright();

    menu_drawTitleBar(TITLEBAR_DEFAULT_COLOR, "reduceOS configuration manager");
    terminal_y++;

    OPTIONS_START(); 
    OPTION_SELECT("Start reduceOS", "Load reduceOS with the specified options", NULL);
    OPTION_SELECT("Edit command line", "Opens an editor for you to edit the command line on", NULL);
    OPTION_SELECT("Edit drivers", "Edits any drivers that might need to be modified", "This will mount the initial ramdisk to get the driver list");
    OPTION_SELECT("", NULL, NULL); // Blank
    
    memcpy((void*)options + ((optcount+(pages*MAX_PAGES))*sizeof(option_t)), reduce_os_options, sizeof(reduce_os_options));
    optcount += sizeof(reduce_os_options) / sizeof(option_t);

    OPTIONS_LOOP();


    switch (selected) {
        case 0:
            // Start reduceOS by breaking
            break;

        case 1:
            // Edit command line
            polyaniline_error_nonfatal("Not implemented");
            return;

        case 2:
            // Edit drivers
            polyaniline_error_nonfatal("Not implemented");
            return;

        case 3:
            // Blank
            polyaniline_configureOS();
            return;

        case -1:
        default:
            return;
    }

    // Collect and build command line
    char kcmdline[512] = { 0 };
    int length = 0;

    terminal_clearScreen(BOOT_DEFAULT_FG, BOOT_DEFAULT_BG);

    for (int i = 0; i < pages; i++) {
        for (int o = 0; o < MAX_PAGES; o++) {
            if (options[i][o].type == OPTION_TYPE_CHECKBOX && (options[i][o].enabled)) {
                length += strlen((const char*)options[i][o].data);
                if (length >= 512) polyaniline_error_nonfatal("Too many options selected. This is a bug! Report this!");
                strcat(kcmdline, (const char*)options[i][o].data);
            }
        }
    }

    for (int i = 0; i < optcount; i++) {
        if (options[selected_page][i].type == OPTION_TYPE_CHECKBOX && (options[selected_page][i].enabled)) {
            length += strlen((const char*)options[selected_page][i].data);
            if (length >= 512) polyaniline_error_nonfatal("Too many options selected. This is a bug! Report this!");
            strcat(kcmdline, (const char*)options[selected_page][i].data);
        }
    }

    platform_boot(kcmdline);
}



/**
 * @brief Boot choice menu
 */
void polyaniline_bootChoice() {
    terminal_clearScreen(terminal_fg, terminal_bg);
    terminal_drawTestTube(BOOT_LIQUID_NORMAL);
    polyaniline_copyright();

    menu_drawTitleBar(TITLEBAR_DEFAULT_COLOR, "Select an option to use");
    terminal_y++;

    OPTIONS_START();

    OPTION_SELECT("Start reduceOS", "Load reduceOS with the default options", NULL);
    OPTION_SELECT("Configure reduceOS", "Configure and then load reduceOS using the built-in", "Polyaniline editor");
    OPTION_SELECT("Load custom ELF file", "Load a custom ELF file (Multiboot1 only)", NULL);
    OPTION_SELECT("Restart system", "Restart the system", NULL);

    OPTIONS_LOOP();

    switch (selected) {
        case 0:
            // Start reduceOS
            platform_boot((char*)__polyaniline_default_kernel_cmdline);
            polyaniline_error("platform_boot(): Failed to start reduceOS\n");
            break;

        case 1:
            // Configure reduceOS - launch OS configuration menu, where the user will return if they want to go back
            polyaniline_configureOS();
            polyaniline_bootChoice();
            break;
        
        case 2:
            polyaniline_error_nonfatal("polyaniline_bootChoice(): Not implemented\n");
            break;
        
        case -1:
            polyaniline_bootChoice();

        default:
            // Reboot system
            platform_reboot();

            // ???
            polyaniline_error("platform_reboot(): Reboot failed");
            break;
    }
}

/**
 * @brief Start the Polyaniline boot menu
 */
void polyaniline_menu() {
    // Calculate ub_offset_x
    ub_offset_x = (terminal_width - USERBOX_WIDTH) / 2;
    ub_offset_y = (terminal_height / 2) - (USERBOX_HEIGHT/2);

    terminal_clearScreen(terminal_fg, terminal_bg);
    terminal_drawTestTube(BOOT_LIQUID_NORMAL);

    polyaniline_copyright();
    polyaniline_bootChoice();

    terminal_clearScreen(terminal_fg, terminal_bg);
    terminal_drawTestTube(BOOT_LIQUID_NORMAL);
}