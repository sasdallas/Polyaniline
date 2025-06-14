/**
 * @file platform/efi/gop.c
 * @brief Graphics output protocol
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/efi/gop.h>
#include <polyaniline/video.h>
#include <efi.h>
#include <efilib.h>

/* Variables */
EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

/* Log "method" */
#define LOG(...) Print(L"[GOP] " __VA_ARGS__)

/**
 * @brief Initialize the Graphics Output Protocol
 */
int gop_initialize() {
    // Let's get the protocol first
    EFI_STATUS status;
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) {
        LOG(L"Failed to locate GOP (BS->LocateProtocol error)\n");
        return 1;
    }

    // Get the native mode of the hardware
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN SizeOfInfo;
    status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &SizeOfInfo, &info);
    if (status == EFI_NOT_STARTED) {
        // Set the video mode
        status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    }
    
    // Did it fail?
    if (EFI_ERROR(status)) {
        LOG(L"Failed to get native video mode from GOP.\n");
        return 1;
    }

    // Clear the screen
    platform_clearScreen(BOOT_DEFAULT_BG);
    
    return 0;
}

/**
 * @brief Collect and return video information
 */
video_info_t gop_collectVideoInformation() {
    video_info_t ret = {
        .bpp = gop->Mode->Info->PixelsPerScanLine,
        .width = gop->Mode->Info->HorizontalResolution,
        .height = gop->Mode->Info->VerticalResolution
    };

    return ret;
}

/**
 * @brief Plot a pixel on X, Y
 * @param x X coordinate
 * @param y Y coordinate
 * @param color Color of the pixel
 */
void platform_putPixel(int x, int y, color_t color) {
    *((uint32_t*)(gop->Mode->FrameBufferBase + 4 * gop->Mode->Info->PixelsPerScanLine * y + 4 * x)) = color.rgb;
}


/**
 * @brief Clear the screen
 * @param color The color to clear with
 */
void platform_clearScreen(color_t color) {
    EFI_PHYSICAL_ADDRESS vram = gop->Mode->FrameBufferBase;
    for (unsigned int y = 0; y < gop->Mode->Info->VerticalResolution; y++) {
        for (unsigned int x = 0; x < gop->Mode->Info->HorizontalResolution; x++) {
            *((uint32_t*)(vram + 4 * x)) = color.rgb;
        }
        
        vram += (4 * gop->Mode->Info->PixelsPerScanLine);
    }
}

/**
 * @brief Draw a character to the screen
 * @param ch The character to draw
 * @param x The X coordinate
 * @param y The Y coordinate
 * @param fg The fg of the character
 * @param bg The bg of the character
 */
void platform_drawCharacter(int ch, int x, int y, color_t fg, color_t bg) {
    uint8_t* fc = terminal_font[ch];

    int realx = x * FONT_CELL_WIDTH;
    int realy = y * FONT_CELL_HEIGHT;

    for (uint8_t h = 0; h < FONT_CELL_HEIGHT; h++) {
        for (uint8_t w = 0; w < FONT_CELL_WIDTH; w++) {
            if (fc[h] & (1 << (FONT_MASK - w))) {
                // Foreground pixel
                platform_putPixel(realx+w, realy+h, fg);
            } else {
                // Background pixel
                platform_putPixel(realx+w, realy+h, bg);
            }
        }
    }
}
