/**
 * @file include/stdio.h
 * @brief stdio header for minilib
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef _STDIO_H
#define _STDIO_H

/**** INCLUDES ****/
#include <polyaniline/video.h>
#include <stddef.h>

#ifndef va_start
#include_next <stdarg.h>
#endif

/**** TYPES ****/

typedef int (*xvas_callback)(void*, char);

/**** FUNCTIONS ****/
int printf(const char * __restrict, ...);
int putchar(int);
int puts(const char *);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
int snprintf(char * str, size_t size, const char * format, ...);
int sprintf(char * str, const char * format, ...);
size_t xvasprintf(xvas_callback callback, void * userData, const char * fmt, va_list args);


#endif