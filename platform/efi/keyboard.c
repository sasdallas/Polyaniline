/**
 * @file platform/efi/keyboard.c
 * @brief Keyboard interface for EFI
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <efi.h>
#include <efilib.h>
#include <polyaniline/interfaces/keyboard.h>

/**
 * @brief Read a keyboard key with a specific timeout
 * @param timeout Timeout in seconds or 0 seconds if not needed
 */
int platform_readKeyboard(int timeout) {
    UINTN index;

    if (timeout) {
        // We need to create an event in conjunction with ST->ConIn->WaitForKey
        // See https://uefi.org/sites/default/files/resources/UEFI_Spec_2_10_Aug29.pdf (sections 7.1.1, 7.1.5, and 7.1.7)
        EFI_EVENT events[] = {ST->ConIn->WaitForKey, NULL};
        uefi_call_wrapper(ST->BootServices->CreateEvent, 5, EVT_TIMER, 0, NULL, NULL, &events[1]);
        uefi_call_wrapper(ST->BootServices->SetTimer, 3, events[1], TimerRelative, timeout * 10000000UL);
        uefi_call_wrapper(ST->BootServices->WaitForEvent, 3, 2, events, &index);
    } else {
        // Wait for event to complete
        uefi_call_wrapper(ST->BootServices->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
    }
    
    // Now read the keystroke
    EFI_INPUT_KEY key;
    EFI_STATUS status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);

    if (EFI_ERROR(status)) {
        // Likely indicates the timer expired or something weird happened
        return 0;
    }

    // Get the scancodes (Table B.1)
    switch (key.ScanCode) {
        case 0x00:
            // This could mean that UnicodeChar was set
            if (key.UnicodeChar == L'\r') return KEYBOARD_ENTER;
            return key.UnicodeChar;

        // Translate arrow keys
        case 0x01:
            return KEYBOARD_UP;
        
        case 0x02:
            return KEYBOARD_DOWN;
    
        case 0x03:
            return KEYBOARD_RIGHT;
        
        case 0x04:
            return KEYBOARD_LEFT;

        case 0x17:
            return KEYBOARD_ESC;
            
        // Don't bother with any other keys, we don't need them.
        default:
            return 0;
    }
}