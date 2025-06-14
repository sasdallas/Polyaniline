/**
 * @file include/polyaniline/efi/multiboot.h
 * @brief Multiboot EFI
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef EFI_MULTIBOOT_H
#define EFI_MULTIBOOT_H

/**** INCLUDES ****/
#include <stdint.h>
#include <polyaniline/multiboot.h>

/**** FUNCTIONS ****/


/**
 * @brief Create multiboot information from allocated structure
 * @param multiboot The allocated structure
 * @param cmdline The command line to use
 * @param initrd_start Initial ramdisk start address
 * @param initrd Initial ramdisk end address
 * @param kernel_end The kernel end that the Multiboot system will put new structures on. Pointer so it can be updated
 * @returns 0 on success
 */
int multiboot_create(multiboot_t *multiboot, char *cmdline, uintptr_t initrd_start, uintptr_t initrd, uintptr_t *kernel_end);

/**
 * @brief Relocate the initial ramdisk after Multiboot information was compiled
 * @param initrd Initial ramdisk location in memory
 * @param initrd_size The size of the initial ramdisk location in memory
 * @param kernel_end The end of the kernel
 */
int multiboot_relocateRamdisk(uintptr_t initrd, uintptr_t initrd_size, uintptr_t kernel_end);

#endif