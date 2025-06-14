/**
 * @file include/polyaniline/config.h
 * @brief Configuration data
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_CONFIG_H
#define POLYANILINE_CONFIG_H

/**** INCLUDES ****/
#include <stdint.h>

/**** VARIABLES ****/

// Versioning
extern const int __polyaniline_version_major;
extern const int __polyaniline_version_minor;
extern const int __polyaniline_version_lower;

// Codename
extern const char *__polyaniline_version_codename;

// Date and time
extern const char *__polyaniline_build_date;
extern const char *__polyaniline_build_time;

// Build type (EFI or BIOS)
extern const char *__polyaniline_build_type;

// Compiler
extern const char *__polyaniline_compiler;


// IMPORTANT: The kernel filename by default and the initrd file name by default
extern char *__polyaniline_kernel_file, *__polyaniline_initrd_file;

extern const char *__polyaniline_default_kernel_cmdline;

extern const uintptr_t __polyaniline_kernel_address;

#endif