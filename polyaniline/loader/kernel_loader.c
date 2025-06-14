/**
 * @file polyaniline/elf/kernel_loader.c
 * @brief ELF kernel loader
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */

#include <polyaniline/loader/elf.h>
#include <polyaniline/error.h>
#include <polyaniline/config.h>
#include <stdio.h>
#include <string.h>
#pragma GCC diagnostic ignored "-Wunused-variable"


/**
 * @brief Check the EHDR of a file
 * @returns 1 for ELF32, 2 for ELF64, panics on invalid ELF file
 */
int kernel_checkEHDR(uint8_t *ehdr) {

    // Check header
    if (ehdr[EI_MAG0] != ELFMAG0 ||
        ehdr[EI_MAG1] != ELFMAG1 ||
        ehdr[EI_MAG2] != ELFMAG2 ||
        ehdr[EI_MAG3] != ELFMAG3) 
    {
        polyaniline_error("kernel_checkEHDR(): Invalid EI_MAG values in kernel image\n");
    }

    // Check class
    if (ehdr[EI_CLASS] == ELFCLASS32) {
        return 1;
    } else if (ehdr[EI_CLASS] == ELFCLASS64) {
        return 2;
    } else {
        polyaniline_error("kernel_checkEHDR(): Invalid ELF class %d\n", ehdr[EI_CLASS]);
    }

    __builtin_unreachable();
}


/**
 * @brief Load an ELF32-style image
 * @param ehdr The EHDR of the file
 * @returns End of the file in memory
 */
uintptr_t kernel_load32(Elf32_Ehdr *ehdr) {
    // Make sure machine type is supported
    if (ehdr->e_machine != EM_386) {
        polyaniline_error("kernel_load32(): ehdr->e_machine != EM_386");
    }

    uintptr_t end_ptr = 0x0;
    // Load PHDRs
    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf32_Phdr *phdr = (Elf32_Phdr*)((uintptr_t)ehdr + ehdr->e_phoff + (i * ehdr->e_phentsize));

        switch (phdr->p_type) {
            case PT_NULL:
                break;
            
            case PT_LOAD:
                printf("PT_LOAD vaddr %p offset %p filesz %d memsz %d\n", phdr->p_vaddr, phdr->p_offset, phdr->p_filesz, phdr->p_memsz);
                
                // Save variables (memcpy will overwrite)
                uintptr_t filesz = phdr->p_filesz;
                uintptr_t memsz = phdr->p_memsz;
                uintptr_t vaddr = phdr->p_vaddr;

                // Load into memory
                #pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
                memcpy((uint8_t*)(phdr->p_vaddr), (uint8_t*)(ehdr) + phdr->p_offset, phdr->p_filesz);
                

                if (memsz > filesz) {
                    // Zero out the rest of the section
                    memset((void*)(uintptr_t)(vaddr + filesz), 0, memsz - filesz);
                }
                

                if (vaddr + memsz > end_ptr) end_ptr = vaddr + memsz;

                break;

            default:
                polyaniline_error("kernel_load32(): PHDR type unrecognized - 0x%x\n", phdr->p_type);  
        }
    }

    printf("Successfully loaded all PT sections\n");
    return end_ptr;
}

/**
 * @brief Load an ELF64-style image
 * @param ehdr The EHDR of the file
 * @returns Entrypoint
 */
uintptr_t kernel_load64(Elf64_Ehdr *ehdr) {
    printf("EHDR available at %p\n", ehdr);

    // Make sure machine type is supported
    if (ehdr->e_machine != EM_X86_64) {
        polyaniline_error("kernel_load64(): ehdr->e_machine != EM_X86_64");
    }

    uintptr_t end_ptr = 0x0;
    // Load PHDRs
    for (int i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr *phdr = (Elf64_Phdr*)((uintptr_t)ehdr + ehdr->e_phoff + (i * ehdr->e_phentsize));

        switch (phdr->p_type) {
            case PT_NULL:
                break;
            
            case PT_LOAD:
                printf("PT_LOAD vaddr %p paddr %p offset %p filesz %d memsz %d\n", phdr->p_vaddr, phdr->p_paddr, phdr->p_offset, phdr->p_filesz, phdr->p_memsz);

                // Save variables (memcpy will overwrite)
                uintptr_t filesz = phdr->p_filesz;
                uintptr_t memsz = phdr->p_memsz;
                uintptr_t addr = phdr->p_paddr;

                // Load into memory
                // Normally you want to use vaddr but our kernel is higher half so copy it to paddr and let it set up its own mapping tables
                memcpy((uint8_t*)(phdr->p_paddr), (uint8_t*)(ehdr) + phdr->p_offset, phdr->p_filesz);

                // Zero remainder
                if (memsz > filesz) {
                    // Zero out the rest of the section
                    memset((void*)(uintptr_t)(phdr->p_paddr + filesz), 0, memsz - filesz);
                }

                if (addr + memsz > end_ptr) end_ptr = addr + memsz;

                printf("PT_LOAD success\n");

                break;

            default:
                polyaniline_error("kernel_load64(): PHDR type unrecognized - 0x%x\n", phdr->p_type);  
        }
    }

    printf("Successfully loaded all PT sections\n");
    return end_ptr;
}

/**
 * @brief Load the kernel image
 * @param kernel_image Pointer to kernel image
 * @param entrypoint Output entrypoint
 * @returns A pointer to the end of the kernel image
 */
uintptr_t kernel_load(void *kernel_image, uintptr_t *entrypoint) {
    int ehdr_type = kernel_checkEHDR(kernel_image);
    
    if (ehdr_type == 1) {
        Elf32_Ehdr *ehdr = (Elf32_Ehdr*)kernel_image;
        *entrypoint = ehdr->e_entry;
        printf("Loading ELF32 kernel image\n");
        return kernel_load32(ehdr);
    } else if (ehdr_type == 2) {
        Elf64_Ehdr *ehdr = (Elf64_Ehdr*)kernel_image;
        *entrypoint = ehdr->e_entry;
        printf("Loading ELF64 kernel image\n");

        return kernel_load64(ehdr);
    }

    return 0x0; // shouldn't reach
}