/**
 * @file polyaniline/config.c
 * @brief Configuration data for Polyaniline
 * 
 * 
 * @copyright
 * This file is part of the Hexahedron kernel, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/config.h>


// This file is deleted and rebuilt every build to update these.
const char *__polyaniline_build_date = __DATE__;
const char *__polyaniline_build_time = __TIME__;


// Versioning information
const int __polyaniline_version_major = 2;
const int __polyaniline_version_minor = 0;
const int __polyaniline_version_lower = 0;

// Codename
const char *__polyaniline_version_codename = "Sunset";

// Default boot files
char *__polyaniline_kernel_file = "hexahedron-kernel.elf";
char *__polyaniline_initrd_file = "initrd.tar.img";

// Default kernel command line
const char *__polyaniline_default_kernel_cmdline = "--use-polyaniline";

// Kernel load address
const uintptr_t __polyaniline_kernel_address = 0x100000;

/**** AUTO-GENERATED VERSIONING INFO ****/


#if defined(__EFI__)
const char *__polyaniline_build_type = "EFI";
#elif defined(__BIOS__)
const char *__polyaniline_build_type = "BIOS";
#else
const char *__polyaniline_build_type = "UNKNOWN";
#endif

// Compiler information
#if (defined(__GNUC__) || defined(__GNUG__)) && !(defined(__clang__) || defined(__INTEL_COMPILER))
#define POLYANILINE_COMPILER_VERSION "GCC " __VERSION__;
#else
#define POLYANILINE_COMPILER_VERSION "Unknown Compiler"
#endif

const char *__polyaniline_compiler = POLYANILINE_COMPILER_VERSION;