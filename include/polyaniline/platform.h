/**
 * @file include/polyaniline/platform.h
 * @brief Platform system
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_PLATFORM_H
#define POLYANILINE_PLATFORM_H

/**** FUNCTIONS ****/

/**
 * @brief Boot the kernel using a specified command line
 * @param cmdline The command line to use
 * @returns Only if the boot is not successful.
 */
void platform_boot(char *cmdline);

/**
 * @brief Reboot the system in its current state
 * @returns Only if failed
 */
void platform_reboot();

#endif