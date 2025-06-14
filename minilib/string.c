/**
 * @file minilib/string.c
 * @brief string functions (and memory)
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <string.h>

#ifndef __EFI__ // GNUEFI provides functions for these

void* memcpy(void* __restrict destination_ptr, const void* __restrict source_ptr, size_t size) {
    unsigned char* destination = (unsigned char*)destination_ptr;
    const unsigned char* source = (const unsigned char*)source_ptr;

    for (size_t i = 0; i < size; i++)
        destination[i] = source[i];

    return destination_ptr;
}

void* memset(void* destination_ptr, int value, size_t size) {
    size_t i = 0;
    for (; i < size; i++) {
        ((char*)destination_ptr)[i] = value;
    }
    
    return destination_ptr;
}

#endif

size_t strlen(const char *s) {
    char *p = (char*)s;

    size_t out = 0;
    while (*p++) out++;
    return out;
}

char* strcpy(char* destination_str, const char* source_str) {
    char *destination_ptr = destination_str;
    
    while ( (*destination_str = *source_str) ) {
        destination_str++;
        source_str++;
    }

    return destination_ptr;
}

char * strcat(char *dest, const char *src) {
	char * end = dest;
	while (*end != '\0') {
		++end;
	}
	while (*src) {
		*end = *src;
		end++;
		src++;
	}
	*end = '\0';
	return dest;
}