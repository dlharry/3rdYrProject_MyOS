/*
D13.2.137 VBAR_EL1, Vector Base Address Register (EL1)
*/

.globl irq_init_vectors
irq_init_vectors:
    adr x0, vectors
    msr vbar_el1, x0
    ret

/*
C5.2.2 DAIF, interrput mask bits
*/
.globl irq_enable
irq_enable:
    msr daifclr, #2 // clear bit 2, enable irqs
    ret

.globl irq_disable
irq_disable:
    msr daifset, #2 // set bit 2, disable irqs
    ret