/**
 * @file include/polyaniline/error.h
 * @brief Polyaniline error handler
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_ERROR_H
#define POLYANILINE_ERROR_H

/**** FUNCTIONS ****/

/**
 * @brief Critically halt Polyaniline due to a fatal error
 * @param format The error format string
 */
void polyaniline_error(char *format, ...);

/**
 * @brief Nonfatally error the system
 * @param format The error format string
 * 
 * @note This will redirect back to @c polyaniline_menu
 */
void polyaniline_error_nonfatal(char *format, ...);

#endif