/**
 * @file include/string.h
 * @brief String header
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef _STRING_H
#define _STRING_H

/**** INCLUDES ****/
#include <stdint.h>
#include <stddef.h>

/**** FUNCTIONS ****/

void * memcpy ( void * destination, const void * source, size_t num );
void * memset ( void * ptr, int value, size_t num );
size_t strlen(const char *s);
char * strcpy(char *dest, const char *src);
char * strcat(char *dest, const char *src);

#endif