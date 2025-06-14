/**
 * @file include/polyaniline/efi/gop.h
 * @brief Graphics Output Protocol Header file
 * 
 * 
 * @copyright
 * This file is part of the Hexahedron kernel, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_EFI_GOP_H
#define POLYANILINE_EFI_GOP_H


/**** INCLUDES ****/
#include <polyaniline/video.h>

/**** FUNCTIONS ****/

/**
 * @brief Initialize the Graphics Output Protocol routine
 */
int gop_initialize();

/**
 * @brief Collect and return video information
 */
video_info_t gop_collectVideoInformation();

#endif