.section ".text.boot"
.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0,#0xFF
    cbz x0, master
    bne waiting

memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero
    ret

master:
    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero

    mov    sp, #0x400000  // 4MB
    bl     main

waiting:
    wfe
    b _start

