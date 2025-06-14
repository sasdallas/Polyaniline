/**
 * @file polyaniline/main.c
 * @brief Main generic logic of Polyaniline
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/polyaniline.h>
#include <polyaniline/platform.h>
#include <polyaniline/terminal.h>
#include <polyaniline/config.h>
#include <polyaniline/error.h>
#include <polyaniline/menu.h>
#include <stdio.h>

/**
 * @brief Main function of Polyaniline. Call after all interfaces are setup
 */
void polyaniline_main() {
    // Print versioning info
    printf("Polyaniline %d.%d.%d-%s (codename \"%s\")\n",
            __polyaniline_version_major,
            __polyaniline_version_minor,
            __polyaniline_version_lower,
            __polyaniline_build_type,
            __polyaniline_version_codename);
    
    printf("Compiled by %s on %s %s\n", 
        __polyaniline_compiler,
        __polyaniline_build_date, 
        __polyaniline_build_time);
     
    terminal_drawTestTube(BOOT_LIQUID_NORMAL);

    // Run the menu
    polyaniline_menu();

    // right back atcha
    platform_boot("itworks");

    // Looks like it didn't work
    polyaniline_error("polyaniline_main(): Failed to launch kernel.\n");
}