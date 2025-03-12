.section ".text.boot"
.globl _start
_start:
    mrs x5, mpidr_el1
    and x5, x5,#0xFF
    cbz x5, master
    bne waiting

memzero:
    str xzr, [x5], #8
    subs x1, x1, #8
    b.gt memzero
    ret

master:
    adr    x5, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x5
    bl     memzero

    // Save dtb_addr to global variable
    ldr x1, =dtb_addr
    str x0, [x1]

    mov    sp, #0x400000  // 4MB
    bl     main

waiting:
    wfe
    b _start

.section ".data"
.global dtb_addr
dtb_addr: .dc.a 0x0

