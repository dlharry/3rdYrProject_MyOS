// Written by Hanmin Liu
// 
.section ".text.boot" // put the following code in a section namely
// linker should put this at the start of the kernal image
.global _start // execution starts here

_start:
    mrs x1, mpidr_el1 // read the multiprocessor affinity register(mpidr_el1) into reg x1
    // to get the processor id, use 'and' isolate the lower bits(which usually indicate the core id)
    // and check if the core id is 0(the main core, the one we want)
    and x1, x1, #3
    cbz x1, 2f // compare and branch if zero , if x1 is not 0, indicating the processor is 
    // not the main core, jumps to the infinite loop labeled '1:'

1:  wfe // wait for event until maincore and branch back to the same instruction
    b   1b

2:  ldr x1, =_start // when main core, 
    // stack pointer set to address of `_start`, 
    // initializing the stack to start just below the boot code
    mov sp, x1

    ldr x1, =__bss_start // start address
    ldr w2, =__bss_size // size of the section

3:  cbz w2, 4f // if zero quit loop
    str xzr, [x1], #8 // xzr the zero register
    sub w2, w2, #1 // decrement size counter
    cbnz w2, 3b 

4:  bl main // jump to c

b 1b // in case main returns