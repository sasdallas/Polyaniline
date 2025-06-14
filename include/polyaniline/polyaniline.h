/**
 * @file include/polyaniline/polyaniline.h
 * @brief Polyaniline main logic
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_POLYANILINE_H
#define POLYANILINE_POLYANILINE_H

/**** FUNCTIONS ****/

/**
 * @brief Main function of Polyaniline. Call after all interfaces are setup
 */
void polyaniline_main();

/**
 * @brief Draw the Polyaniline copyright
 */
void polyaniline_copyright();

#endif