/**
 * @file include/polyaniline/loader/kernel_loader.h
 * @brief Kernel loader
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_LOADER_KERNEL_LOADER_H
#define POLYANILINE_LOADER_KERNEL_LOADER_H

/**** FUNCTIONS ****/

/**
 * @brief Load the kernel image
 * @param kernel_image Pointer to kernel image
 * @param entrypoint Output entrypoint
 * @returns A pointer to the end of the kernel image
 */
uintptr_t kernel_load(void *kernel_image, uintptr_t *entrypoint);

#endif
