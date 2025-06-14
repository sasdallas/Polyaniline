/**
 * @file platform/efi/multiboot.c
 * @brief EFI Multiboot parsing code
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/multiboot.h>
#include <polyaniline/efi/gop.h>
#include <polyaniline/config.h>
#include <polyaniline/error.h>
#include <stdio.h>
#include <string.h>
#include <efi.h>
#include <efilib.h>

/* Allocate a new multiboot structure */
#define MULTIBOOT_ALLOCATE(type)    ({type *structure = (type*)*kernel_end; *kernel_end = *kernel_end + sizeof(type) + 1; structure; })
#define MULTIBOOT_ALLOCATE_SIZE(size)   ({void *structure = (void*)*kernel_end; *kernel_end = *kernel_end + size + 1; structure; })

/* Stored Multiboot information */
multiboot_t *mboot = NULL;

/**
 * @brief Relocate the initial ramdisk after Multiboot information was compiled
 * @param initrd Initial ramdisk location in memory
 * @param initrd_size The size of the initial ramdisk location in memory
 * @param kernel_end The end of the kernel
 */
int multiboot_relocateRamdisk(uintptr_t initrd, uintptr_t initrd_size, uintptr_t kernel_end) {
    // Page align the kernel end if requried
    if (kernel_end & 0xFFF) {
        kernel_end += 0x1000;
        kernel_end &= ~0xFFF;
    }

    // Setup the module
    multiboot1_mod_t *mod = (multiboot1_mod_t*)(uintptr_t)mboot->mods_addr;
    mod->mod_start = (uint32_t)0x300000;
    mod->mod_end = (uint32_t)(0x300000 + initrd_size);

    // Copy
    // !!!: stupid but kernel will whine if we go anywhere other than an address >0x200000... why???
    memcpy((void*)0x300000, (void*)initrd, initrd_size);
    printf("Relocated initial ramdisk from %p to %016llX\n", initrd, (uint32_t)0x300000);

    return 0;
}



/**
 * @brief Create multiboot information from allocated structure
 * @param multiboot The allocated structure
 * @param cmdline The command line to use
 * @param initrd Initial ramdisk address
 * @param kernel_end The kernel end that the Multiboot system will put new structures on. Pointer so it can be updated
 * @returns 0 on success
 */
int multiboot_create(multiboot_t *multiboot, char *cmdline, uintptr_t initrd_start, uintptr_t initrd, uintptr_t *kernel_end) {
    mboot = multiboot;
    memset((void*)multiboot, 0, sizeof(multiboot_t));

    // Get the initial ramdisk end
    uintptr_t initrd_end = *kernel_end;

    // Bootloader name
    // char bootloader_name[128];
    // snprintf(bootloader_name, 128, "Polyaniline %d.%d.%d-%s %s\n",
    //         __polyaniline_version_major,
    //         __polyaniline_version_minor,
    //         __polyaniline_version_lower,
    //         __polyaniline_build_type,
    //         __polyaniline_version_codename);

    char *btldr_name = (char*)MULTIBOOT_ALLOCATE_SIZE(strlen("Polyaniline"));
    strcpy(btldr_name, "Polyaniline");
    multiboot->boot_loader_name = (uint32_t)(uintptr_t)btldr_name;

    char *cmdline_new = (char*)MULTIBOOT_ALLOCATE_SIZE(strlen(cmdline)); 
    strcpy(cmdline_new, cmdline);
    multiboot->cmdline = (uint32_t)(uintptr_t)cmdline_new;

    // Get framebuffer
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_STATUS status;
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) {
        polyaniline_error("multiboot_create(): Failed to locate GOP (BS->LocateProtocol error)\n");
        return 1;
    }
    
    multiboot->framebuffer_addr = gop->Mode->FrameBufferBase;
    multiboot->framebuffer_bpp = gop->Mode->Info->HorizontalResolution;
    multiboot->framebuffer_pitch = gop->Mode->Info->PixelsPerScanLine * 4;
    multiboot->framebuffer_height = gop->Mode->Info->VerticalResolution;
    multiboot->framebuffer_width = gop->Mode->Info->HorizontalResolution;
    multiboot->flags |= 0x1000;

    // Create module
    multiboot1_mod_t *initrd_mod = MULTIBOOT_ALLOCATE(multiboot1_mod_t);
    initrd_mod->cmdline = (uint32_t)(uintptr_t)MULTIBOOT_ALLOCATE_SIZE(strlen("type=initrd"));
    strcpy((char*)(uintptr_t)initrd_mod->cmdline, "type=initrd");
    initrd_mod->mod_start = initrd_start;
    initrd_mod->mod_end = initrd;
    multiboot->mods_addr = (uint32_t)(uintptr_t)initrd_mod;
    multiboot->mods_count = 1;

    // Realign to page boundary for memory map. This isn't required but is liked when done
    *kernel_end += 0x1000;
    *kernel_end &= ~0xFFF;

    UINTN map_size = 0, map_key, descriptor_size;

    // We need to get the map size so we can allocate - no error checking is done because this is supposed to fail
    status = uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, &map_size, NULL, &map_key, &descriptor_size, NULL);

    // Now allocate
    EFI_MEMORY_DESCRIPTOR *memory_desc = MULTIBOOT_ALLOCATE_SIZE(map_size);

    status = uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, &map_size, memory_desc, &map_key, &descriptor_size, NULL);
    if (EFI_ERROR(status)) {
        polyaniline_error("multiboot_create(): Could not get memory map\n");
    }

    uint64_t himem;
    int descriptor_count = map_size / descriptor_size; // Calculate descriptors

    // Create memory map
    multiboot1_mmap_entry_t *mmap = MULTIBOOT_ALLOCATE_SIZE((sizeof(uintptr_t) * 2 + sizeof(uint32_t) * 2) * descriptor_count);
    multiboot->mmap_addr = (uintptr_t)mmap;

    for (int i = 0; i < descriptor_count; i++) {
        // Get the descriptor
        EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR*)((uintptr_t)memory_desc + (descriptor_size*i));
        
        // Configure mmap
        mmap->addr = desc->PhysicalStart;
        mmap->size = sizeof(uintptr_t) * 2 + sizeof(uint32_t);
        mmap->len = desc->NumberOfPages * 4096;

        switch (desc->Type) {
            
				case EfiConventionalMemory:
				case EfiLoaderCode:
				case EfiLoaderData:
				case EfiBootServicesCode:
				case EfiBootServicesData:
				case EfiRuntimeServicesCode:
				case EfiRuntimeServicesData:
					mmap->type = 1;
					break;
				case EfiReservedMemoryType:
				case EfiUnusableMemory:
				case EfiMemoryMappedIO:
				case EfiMemoryMappedIOPortSpace:
				case EfiPalCode:
				case EfiACPIMemoryNVS:
				case EfiACPIReclaimMemory:
				default:
					mmap->type = 2;
					break;
        }

        // himem will fill the header after
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE && mmap->addr >= 0x100000) {
            himem += mmap->len;
        }

        // Update mmap
        mmap = (multiboot1_mmap_entry_t*)((uintptr_t)mmap + mmap->size + sizeof(uint32_t));
    }

    multiboot->mem_lower = 1024;
    multiboot->mem_upper = himem / 1024;
    multiboot->mmap_length = (uintptr_t)mmap - multiboot->mmap_addr;
    multiboot->flags |= 0x0049; // MULTIBOOT_FLAG_MMAP | MULTIBOOT_FLAG_MODULES | MULTIBOOT_FLAG_MEMINFO

    // Copy the initial ramdisk into the kernel location
    *kernel_end += 0x5000;
    *kernel_end &= ~0xFFF;

    printf("kernel_end = %p\n", *kernel_end);

    return 0;
}