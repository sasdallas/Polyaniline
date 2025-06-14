/**
 * @file include/polyaniline/config_opts.h
 * @brief Configuration options
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#ifndef POLYANILINE_CONFIG_OPTS_H
#define POLYANILINE_CONFIG_OPTS_H

/**** INCLUDES ****/
#include <polyaniline/menu.h>
#include <stdint.h>
#include <stddef.h>

/**** DEFINITIONS ****/
#define ETHEREAL_CONFIG(n,s1,s2,cmdline,d,i) (option_t){ .name = n, .subtitle1 = s1, .subtitle2 = s2, .type = OPTION_TYPE_CHECKBOX, .enabled = d, .data = (void*)((char*)(cmdline " ")), .inv = i }

/**** VARIABLES ****/

option_t ethereal_options[] = {
    ETHEREAL_CONFIG("Enable Ethereal Polyaniline support", "Configures whether the kernel is told it is being loaded by Polyaniline", "This mainly controls RSDP passing, which is Multiboot incompliant.", "--use-polyaniline", 1, 0),
    ETHEREAL_CONFIG("Debug Console", "Enable/disable redirecting debug logs to console", "Will flood the logs", "--debug=console", 0, 0),
    ETHEREAL_CONFIG("Enable SMP", "Enable/disable SMP support", "If disabled, the SMP subsystem will only run to collect necessary data", "--disable-smp", 0, 1),
    ETHEREAL_CONFIG("ACPI Support", "Enable/disable ACPI support in Hexahedron (minacpi + ACPICA)", "Recommended to disable for systems that fail to initialize video (disables SMP)", "--no-acpi", 0, 1),
    ETHEREAL_CONFIG("ACPICA Support", "Enable/disable ACPICA support in Hexahedron", "minacpi will be used as a backup if ACPICA is disabled", "--no-acpica", 0, 1),
    ETHEREAL_CONFIG("Disable initial video support", "Controls whether Hexahedron initializes the video system.", "WARNING: Any drivers that attempt to initialize the video system may crash the system. Take caution.", "--no-video", 0, 0),
    ETHEREAL_CONFIG("Disable PSF font usage", "Disables usage of the ramdisk PC screen font", NULL, "--no-psf-font", 0, 0),
    ETHEREAL_CONFIG("Disable all drivers", "Disables usage of all drivers", "(NOT RECOMMENDED)", "--no-load-drivers", 0, 0),
    ETHEREAL_CONFIG("PS/2: Test controllers", "Enable/disable support for testing PS/2 controllers", "Only works if ps2.sys is loaded", "--ps2-disable-tests", 0, 0)
};

#endif