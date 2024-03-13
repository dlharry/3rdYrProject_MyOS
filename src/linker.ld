SECTIONS
{
    . = 0x80000;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    
    . = ALIGN(0x8);
    bss_begin = .;
    .bss : { *(.bss*) }
    bss_end = .;

    . = ALIGN(0x00001000);
    id_pgd = .;
    .data.id_pgd : { . += (6 * (1 << 12)); }
    
}