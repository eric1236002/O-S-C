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
    bl     from_el2_to_el1
    bl     set_exception_vector_table
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

from_el2_to_el1:
    mov    x1, (1 << 31)        // set x1 to 0x80000000(hex)
    msr    hcr_el2, x1          // store x1 to hcr_el2, EL1 uses aarch64.msr(Move to System Register)
                                // Forces all stage 2 translations for data accesses and translation table walks to Normal
                                // memory to be Non-cacheable for the EL1&0 translation regime
    mov    x1, 0x3c5            // EL1h (SPSel = 1) with interrupt disabled, this action is to switch to EL1
    msr    spsr_el2, x1         // store x1 to spsr_el2. this instruction set the program status of EL2 when entering EL1.
                                // including interrupt disabled (I and F are set to 1), and selecting EL1h mode (SPSel = 1).
                                // This ensures that EL2 entering EL1 is in a safe and controllable state.
    msr    elr_el2, lr          // store lr to elr_el2. elr_el2 is the exception link register of EL2,
                                // which is the address of the instruction that was used to jump to EL2.
    eret                        // return to EL1

set_exception_vector_table:
    adr x1, exception_vector_table
    msr vbar_el1, x1
    ret

.section ".data"
.global dtb_addr
dtb_addr:
    .quad 0
