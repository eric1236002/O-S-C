SECTIONS
{
    . = 0x80000;     /* Kernel load address for AArch64 */
    
    .text : ALIGN(4K) {
        KEEP(*(.text.boot))  /* 保證啟動代碼在最前面 */
        *(.text)
        *(.text.*)
    } :text
    
    .rodata : ALIGN(4K) {
        *(.rodata)
        *(.rodata.*)
    } :rodata
    
    .data : ALIGN(4K) {
        *(.data)
        *(.data.*)
    } :data
    
    .bss : ALIGN(4K) {
        . = ALIGN(16);
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } :data
    
    /DISCARD/ : {
        *(.comment)
        *(.gnu*)
        *(.note*)
        *(.eh_frame*)
    }
}

PHDRS
{
    text PT_LOAD FLAGS(5);     /* 讀+執行 */
    rodata PT_LOAD FLAGS(4);   /* 只讀 */
    data PT_LOAD FLAGS(6);     /* 讀+寫 */
}