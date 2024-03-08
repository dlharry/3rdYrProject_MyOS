.globl delay
.globl put32
.globl get32
.globl get_el

delay:
    subs x0, x0, #1
    bne delay
    ret

put32:
    str w1, [x0]
    ret

get32:
    ldr w0, [x0]
    ret

get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret