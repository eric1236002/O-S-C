ENTRY(_start)
SECTIONS
{
    . = 0x20000;
    .text.boot : { *(.text.boot) }
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(0x8);
    .bss : {
        bss_begin = .;
        *(.bss*) 
        bss_end = .;
    }
    _end = .;
}