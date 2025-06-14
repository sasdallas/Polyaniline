/**
 * @file platform/efi/boot.c
 * @brief EFI boot logic
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the BSD 3-clause license.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/platform.h>
#include <polyaniline/error.h>
#include <polyaniline/multiboot.h>
#include <polyaniline/efi/multiboot.h>
#include <polyaniline/config.h>
#include <polyaniline/loader/kernel_loader.h>
#include <efi.h>
#include <efilib.h>
#include <stdio.h>


/* Loaded image */
extern EFI_LOADED_IMAGE *LoadedImage;

/**
 * @brief Read the kernel file into memory
 * @returns A pointer to the kernel file
 */
uintptr_t platform_loadKernel() {
    // We want to get the simple filesystem protocol for managing files
    EFI_STATUS status;
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs_protocol;
    status = uefi_call_wrapper(ST->BootServices->LocateProtocol, 3, &fs_guid, NULL, (VOID**)&fs_protocol);


    // Now we need to open the root directory of the volume
    EFI_FILE_PROTOCOL *root_directory;
    status = uefi_call_wrapper(fs_protocol->OpenVolume, 2, fs_protocol, &root_directory);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Opening root directory for EFI failed\n");
    }

    // Convert the kernel file path to CHAR16
    CHAR16 kernel_file_path[64] = { 0 };
    int i = 0;
    char *s = (char*)__polyaniline_kernel_file;

    while (*s) {
        kernel_file_path[i] = *s;
        i++;
        s++;
    }


    // Try to get the kernel file
    EFI_FILE_PROTOCOL *kernel_file;
    status = uefi_call_wrapper(root_directory->Open, 5, root_directory, &kernel_file, kernel_file_path, EFI_FILE_MODE_READ, 0);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Kernel file '%s' not found.\n", __polyaniline_kernel_file);
    }

    printf("Located kernel file successfully (\"%s\")\n", __polyaniline_kernel_file);

    // Get the file information 
    EFI_FILE_INFO *kernel_info = NULL;
    EFI_GUID information_id = EFI_FILE_INFO_ID;
    UINTN buffer_size = 128; // !!! wtf?
    status = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &information_id, &buffer_size, (void*)kernel_info);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to get information on kernel file (status %d)\n", status);
    }

    // Calculate the amount of pages for the kernel
    UINTN kernel_pages =  (kernel_info->FileSize / 4096) + 1;

    // Make some memory for the kernel to sit in
    // (Section 7.2.1 - UEFI spec 2.10) Allocation requests of Type AllocateAddress allocate pages at the address pointed by Memory on input
    EFI_PHYSICAL_ADDRESS kernel_address = (EFI_PHYSICAL_ADDRESS)__polyaniline_kernel_address;
    status = uefi_call_wrapper(ST->BootServices->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, kernel_pages, &kernel_address);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to allocate %d pages for kernel (status %d address %p)\n", kernel_pages, status, kernel_address);
    }

    printf("Loading file into memory at %p\n", kernel_address);

    // Make even more memory for the kernel to sit in after loading
    EFI_PHYSICAL_ADDRESS kernel_address_actual = (EFI_PHYSICAL_ADDRESS)__polyaniline_kernel_address;
    status = uefi_call_wrapper(ST->BootServices->AllocatePages, 4, AllocateAddress, EfiLoaderData, kernel_pages, &kernel_address_actual); 

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to allocate %d pages for kernel (status %d address %p)\n", kernel_pages, status, kernel_address_actual);
    }

    printf("Allocated %d pages for kernel successfully\n", kernel_pages);

    // Now read in the file
    UINTN read_size = 134217728; // TEMPORARY
    status = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &read_size, (void*)kernel_address);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to read kernel file\n");
    }

    printf("Kernel loaded successfully (%i KB)\n", read_size / 1024);
    return kernel_address;
}

/**
 * @brief Load the initial ramdisk
 * @param initrd_start Start of initrd
 * @param initrd_end End of initrd
 */
uintptr_t platform_loadInitrd(uintptr_t *initrd_start, uintptr_t *initrd_end) {
    // We want to get the simple filesystem protocol for managing files
    EFI_STATUS status;
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs_protocol;
    status = uefi_call_wrapper(ST->BootServices->LocateProtocol, 3, &fs_guid, NULL, (VOID**)&fs_protocol);


    // Now we need to open the root directory of the volume
    EFI_FILE_PROTOCOL *root_directory;
    status = uefi_call_wrapper(fs_protocol->OpenVolume, 2, fs_protocol, &root_directory);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Opening root directory for EFI failed\n");
    }

    // Convert the initrd file path to CHAR16
    CHAR16 initrd_file_path[64] = { 0 };
    int i = 0;
    char *s = (char*)__polyaniline_initrd_file;

    while (*s) {
        initrd_file_path[i] = *s;
        i++;
        s++;
    }


    // Try to get the kernel file
    EFI_FILE_PROTOCOL *initrd_file;
    status = uefi_call_wrapper(root_directory->Open, 5, root_directory, &initrd_file, initrd_file_path, EFI_FILE_MODE_READ, 0);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Initial ramdisk file '%s' not found.\n", __polyaniline_initrd_file);
    }

    printf("Located initial ramdisk file successfully (\"%s\")\n", __polyaniline_initrd_file);

    // Get the file information 
    EFI_FILE_INFO *initrd_info = NULL;
    EFI_GUID information_id = EFI_FILE_INFO_ID;
    UINTN buffer_size = 128; // !!! wtf?
    status = uefi_call_wrapper(initrd_file->GetInfo, 4, initrd_file, &information_id, &buffer_size, (void*)initrd_info);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to get information on initrd file (status %d)\n", status);
    }

    // Calculate the amount of pages for the initrd
    UINTN initrd_pages =  (initrd_info->FileSize / 4096) + 200;


    // Make memory for the initrd
    EFI_PHYSICAL_ADDRESS initrd_address = (EFI_PHYSICAL_ADDRESS)0x0;
    status = uefi_call_wrapper(ST->BootServices->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, initrd_pages, &initrd_address); 

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to allocate %d pages for initial ramdisk (status %d address %p)\n", initrd_pages, status, initrd_address);
    }


    printf("Loading initial ramdisk into memory at %p - %p\n", initrd_address, initrd_address + initrd_info->FileSize);

    // Now read in the file
    UINTN read_size = 134217728; // TEMPORARY
    status = uefi_call_wrapper(initrd_file->Read, 3, initrd_file, &read_size, (void*)initrd_address);

    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to read initial ramdisk file\n");
    }

    printf("Initial ramdisk loaded successfully (%i KB)\n", read_size / 1024);

    *initrd_start = initrd_address;
    *initrd_end = initrd_address + initrd_info->FileSize;
    *initrd_end = (*initrd_end + 0x1000) & ~0xFFF;
    return initrd_address;
}

typedef struct gdtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdtr_t;

typedef struct gdt_entry {
    uint16_t limit;                 // Maximum address
    uint16_t base_lo;               // Lower 16 bits of the base
    uint8_t base_mid;               // Next 8 bits of the base
    uint8_t access;                 // Access bits (determines segment ring)
    uint8_t flags;                  // Should contain flags, but also contains part of the 20-bit limit address.
                                    // Why would you do this, Intel?
    uint8_t base_hi;                // Final 8 bits of base
} __attribute__((packed)) gdt_entry_t;  

typedef struct gdt {
    gdt_entry_t entry[10];
} __attribute__((packed)) gdt_t;

/**
 * @brief Start kernel image in 32-bit protected mode
 * @param entrypoint The entrypoint
 * @param gdtr The GDT to load
 * @param multiboot Multiboot structure
 */
extern void platform_bootKernelImage(uintptr_t entrypoint, gdtr_t *gdtr, multiboot_t *multiboot);

/**
 * @brief Boot the kernel using a specified command line
 * @param cmdline The command line to use
 * @returns Only if the boot is not successful.
 */
void platform_boot(char *cmdline) {
    // First clear the UEFI watchdog timer to prevent it from resetting us
    uefi_call_wrapper(ST->BootServices->SetWatchdogTimer, 4, 0, 0, 0, NULL);

    // Load kernel
    uintptr_t kernel_address = platform_loadKernel();

    // Done. Now load the ELF file.
    uintptr_t kernel_entry = 0x0;
    uintptr_t kernel_end = kernel_load((void*)kernel_address, &kernel_entry);

    // Allocate a Multiboot structure
    multiboot_t *mboot = (multiboot_t*)kernel_end;
    kernel_end += sizeof(multiboot_t);


    // Load the initial ramdisk
    uintptr_t initrd_start, initrd_end;
    platform_loadInitrd(&initrd_start, &initrd_end);

    // Parse Multiboot structure
    // !!!: This seems to crash a lot..
    if (multiboot_create(mboot, cmdline, initrd_start, initrd_end, &kernel_end)) {
        polyaniline_error("platform_boot(): Could not parse Multiboot information\n");
    }

    // Exit boot services
    EFI_STATUS status;
    UINTN mapSize = 0, mapKey, descriptorSize;
    uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5, &mapSize, NULL, &mapKey, &descriptorSize, NULL);
    status = uefi_call_wrapper(ST->BootServices->ExitBootServices, 2, LoadedImage, mapKey);
    if (EFI_ERROR(status)) {
        polyaniline_error("platform_boot(): Failed to exit boot services\n");
    }

    printf("Exited boot services successfully\n");


    printf("Finished loading everything successfully (%p - %p)\n", kernel_entry, kernel_end);

    // Create temporary GDT
    gdt_t temp_gdt = {
        .entry = {
            {0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00},       // (0x00) Null entry
            {0xFFFF, 0x0000, 0x00, 0x9A, 0xCF, 0x00},       // (0x08) 32-bit kernel-mode code segment
            {0xFFFF, 0x0000, 0x00, 0x92, 0xCF, 0x00},       // (0x10) 32-bit kernel-mode data segment
            {0xFFFF, 0x0000, 0x00, 0xFA, 0xCF, 0x00},       // (0x18) 32-bit user-mode code segment
            {0xFFFF, 0x0000, 0x00, 0xF2, 0xCF, 0x00},       // (0x20) 32-bit user-mode data segment
            {0},       // (0x28)
            {0xFFFF, 0x0000, 0x00, 0x92, 0xAF, 0x00},       // 64-bit kernel-mode data segment
            {0xFFFF, 0x0000, 0x00, 0x9A, 0xAF, 0x00},       // 64-bit kernel-mode code segment
        }
    };

    gdtr_t temp_gdtr = { .limit = sizeof(temp_gdt.entry) - 1, .base = (uintptr_t)&temp_gdt.entry};
    printf("GDTR available at %p - GDT at %p\n", &temp_gdtr, &temp_gdt);

    platform_bootKernelImage(kernel_entry, &temp_gdtr, (multiboot_t*)mboot);
}