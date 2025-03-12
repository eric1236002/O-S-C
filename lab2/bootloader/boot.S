.section ".text.boot"
.globl _start

_start:
    mrs x5, mpidr_el1
    and x5, x5, #0xFF
    cbz x5, master
    b waiting

relocate:
    adr x10, _start         //get the address of _start
    mov x11, #0x60000       //define the address of bootloader
    cmp x10, x11            //check if the address of _start is the same as the address of bootloader
    b.eq skip_relocation    //if the address of _start is the same as the address of bootloader, skip the relocation

    mov x5, #0x80000        //define the source address
    mov x1, #0x60000        //define the target address
    mov x2, #0x2000         //define the size of the relocation

    nop
    nop
    nop

relocation_loop:  
    ldr x3, [x5], #8         //load the data from the source address to the target address
    str x3, [x1], #8         //store the data to the target address
    subs x2, x2, #8          //subtract the size of the relocation
    b.gt relocation_loop     //if the size of the relocation is greater than 0, repeat the relocation


    mov x30, #0x60000        //define the address of bootloader
    br x30                   //jump to the address of bootloader

skip_relocation:
    ldr x1, =dtb_addr        //get the address of dtb_addr
    str x0, [x1]            //store the address of dtb_addr to the address of dtb_addr
    b main

master:
    adr x5, bss_begin
    adr x1, bss_end
    sub x1, x1, x5
    bl memzero

    mov sp, #0x400000
    bl relocate          

memzero:
    str xzr, [x5], #8
    subs x1, x1, #8
    b.gt memzero    
    ret

waiting:
    wfe
    b waiting

.section ".data"
.global dtb_addr
dtb_addr:
    .quad 0
