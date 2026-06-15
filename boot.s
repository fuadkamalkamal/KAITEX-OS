/* Multiboot header */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set VIDEO,    1<<2             /* request video mode */
.set FLAGS,    ALIGN | MEMINFO | VIDEO
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
/* The following 5 fields are for the ELF or AOUT executable format, we can leave them 0 since we use ELF */
.long 0, 0, 0, 0, 0
/* The following fields are for video mode request */
.long 0 /* mode_type: 0 for linear graphics */
.long 1024 /* width: 1024 px */
.long 768 /* height: 768 px */
.long 32 /* depth: 32 bits per pixel (BPP) */

/* 
Stack allocation. 
The bootloader (GRUB or QEMU) leaves us in a 32-bit protected mode.
We need a stack to execute C code.
*/
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

/*
Entry point of the kernel.
*/
.section .text
.global _start
.type _start, @function
_start:
    /* Set up the stack pointer */
    mov $stack_top, %esp
    
    /* Push Multiboot Info dan Magic Number sebagai parameter untuk kernel_main */
    push %ebx
    push %eax
    
    /* Call the C main function */
    call kernel_main
    
    /* If kernel_main returns, halt the CPU */
    cli
1:  hlt
    jmp 1b
.size _start, . - _start
