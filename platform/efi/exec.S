/**
 * @file platform/efi/boot.S
 * @brief Boot code for starting a kernel image
 * 
 * 
 * @copyright
 * This file is part of the Polyaniline bootloader, which is part of the Ethereal Operating System.
 * It is released under the terms of the GPLv3 license, unlike other parts of Ethereal.
 * Please see the LICENSE file in the main repository for more details.
 * 
 * Copyright (C) 2024 Samuel Stuart
 */


.global platform_bootKernelImage
platform_bootKernelImage:
    // Push address of kernel
    push %rdi

    // Push address of structure
    push %rdx

    // Load GDT
    lgdt (%rsi)

    // Disable IRQs
    cli 

    // Go to protected mode
    push $0x8
    lea .inProtectedMode(%rip), %rax
    push %rax
    lretq

.code32

.inProtectedMode:
    movw $0x10, %ax
    movw %ax, %ds

    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    // Disable LME
    mov $0xC0000080, %ecx
    rdmsr
    and $0xFFFFFEFF, %eax
    wrmsr
    
    // Disable paging   
    mov     %cr0, %eax                     
    and     $0x7FFFFFFF, %eax                  
    mov     %eax, %cr0

    // Load dummy variables
    mov $0x2BADB002, %eax
    mov $0xDEADDEAD, %ebp
    popl %ebx
    add $4, %esp

    // Jump to kernel
    jmpl *0(%esp)
    
    cli
    hlt