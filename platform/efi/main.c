/**
 * @file platform/efi/main.c
 * @brief EFI main for Polyaniline
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

// EFI
#include <efi.h>
#include <efilib.h>

// minilib
#include <stdio.h>

// Polyaniline includes
#include <polyaniline/efi/gop.h>
#include <polyaniline/terminal.h>
#include <polyaniline/config.h>
#include <polyaniline/polyaniline.h>

// Interfaces
#include <polyaniline/interfaces/keyboard.h>

/* Loaded image */
EFI_LOADED_IMAGE *LoadedImage = NULL;

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    // Initialize the library & print hello
    InitializeLib(ImageHandle, SystemTable);
    ST = SystemTable;

    Print(L"Loading Polyaniline (reached EFI)...\n");

    // Acquire the image base
    EFI_STATUS status = uefi_call_wrapper(ST->BootServices->HandleProtocol, 3, ImageHandle, &LoadedImageProtocol, (void**)&LoadedImage);

    if (EFI_ERROR(status)) {
        Print(L"FATAL: Could not get image base.\n");
        uefi_call_wrapper(ST->RuntimeServices->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
        return EFI_ABORTED;
    }

    Print(L"Image base: 0x%lx\n", LoadedImage->ImageBase);


    // Initialize the GOP
    if (gop_initialize()) {
        Print(L"FATAL: Could not initialize GOP video, rebooting.\n");
        uefi_call_wrapper(ST->RuntimeServices->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
        return EFI_ABORTED;
    }

    // Initialize the terminal
    if (terminal_init(gop_collectVideoInformation())) {
        Print(L"FATAL: Could not initialize terminal systems\n");
        uefi_call_wrapper(ST->RuntimeServices->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
        return EFI_ABORTED;
    
    }

    // All done here, jump to Polyaniline
    polyaniline_main();

    // Uhh... success?
    return EFI_SUCCESS;
}


/**
 * @brief Reboot the system in its current state
 * @returns Only if failed
 */
void platform_reboot() {
    uefi_call_wrapper(ST->RuntimeServices->ResetSystem, EfiResetWarm, EFI_SUCCESS, 0, NULL);
    // ??? now what
    for (;;);
}