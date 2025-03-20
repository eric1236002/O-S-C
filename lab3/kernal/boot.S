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
    bl     from_el2_to_el1
    bl     set_exception_vector_table
    adr    x5, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x5
    bl     memzero

    // Save dtb_addr to global variable
    ldr x1, =dtb_addr
    str x0, [x1]

    mov    sp, #0x400000  // 4MB
    bl     main

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

waiting:
    wfe
    b _start

.section ".data"
.global dtb_addr
dtb_addr: .dc.a 0x0

//0x3c5=0b001111000101


//m[3:0] = 0b1010 means EL1h
//m[6] = 1 means FIQ mask bit,Exception masked.
//m[7] = 1 means IRQ mask bit,Exception masked.
//m[8] = 1 means SError (System Error) mask bit,Exception masked.
//m[9] = 1 means Process state D mask mask bit,Exception masked.





