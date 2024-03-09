// Written by Hanmin Liu
#include "mm.h"
#include "sysregs.h"
.section ".text.boot" // put the following code in a section namely
// linker should put this at the start of the kernal image
.globl _start // execution starts here
_start:
    mrs x1, mpidr_el1 // read the multiprocessor affinity register(mpidr_el1) into reg x1
    // to get the processor id, use 'and' isolate the lower bits(which usually indicate the core id)
    // and check if the core id is 0(the main core, the one we want)
    and x1, x1, #0xFF
    cbz x1, master // compare and branch if zero , if x1 is not 0, indicating the processor is 
    // not the main core, jumps to the infinite loop labeled '1:'
    b proc_hang

proc_hang:  
    wfe // wait for event until maincore and branch back to the same instruction
    b   proc_hang

master:
    // mrs x0, currentel // if not el2, consider as wrong
    // lsr x0, x0, #2
    // cmp x0, #2
    // bne proc_hang

    ldr x0, =SCTLR_VALUE_MMU_DISABLED //set to el1
    msr sctlr_el1, x0

    ldr x0, =HCR_VALUE //HCR_EL2 hypervisor configuration register,
    msr hcr_el2, x0

    ldr x0, =SCR_VALUE // system control el1
    msr scr_el3, x0

    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0

    eret

el1_entry:
    mov sp, #0x80000

    //bl setup_vm
    //bl enable_mmu

    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0 // bss_size
    bl memzero

    mov sp, #LOW_MEMORY
    bl kernel_main
    b proc_hang