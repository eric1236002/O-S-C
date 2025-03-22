.equ SynchronousEL1t, 1
.equ IRQEL1t, 2
.equ FIQEL1t, 3
.equ ErrorEL1t, 4

.equ SynchronousEL1h, 5
.equ IRQEL1h, 6
.equ FIQEL1h, 7
.equ ErrorEL1h, 8

.equ SynchronousEL0_32, 9
.equ IRQEL0_32, 10
.equ FIQEL0_32, 11
.equ ErrorEL0_32, 12

.equ SynchronousEL0_64, 13
.equ IRQEL0_64, 14
.equ FIQEL0_64, 15
.equ ErrorEL0_64, 16


.macro handle_invalid_entry type
    save_all
    mov	x0, #\type
    bl exception_entry
    load_all
    eret
.endm

.macro	ventry	type
    .align	7
    b	\type
.endm

// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8 
.endm


.align 11 // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
	ventry	sync_invalid_el1t			// Synchronous EL1t
	ventry	irq_invalid_el1t			// IRQ EL1t
	ventry	fiq_invalid_el1t			// FIQ EL1t
	ventry	error_invalid_el1t			// Error EL1t

	ventry	sync_invalid_el1h			// Synchronous EL1h
	ventry	el1_irq					// IRQ EL1h
	ventry	fiq_invalid_el1h			// FIQ EL1h
	ventry	error_invalid_el1h			// Error EL1h

	ventry	sync_invalid_el0_64			// Synchronous 64-bit EL0
	ventry	irq_invalid_el0_64			// IRQ 64-bit EL0
	ventry	fiq_invalid_el0_64			// FIQ 64-bit EL0
	ventry	error_invalid_el0_64			// Error 64-bit EL0

	ventry	sync_invalid_el0_32			// Synchronous 32-bit EL0
	ventry	irq_invalid_el0_32			// IRQ 32-bit EL0
	ventry	fiq_invalid_el0_32			// FIQ 32-bit EL0
	ventry	error_invalid_el0_32			// Error 32-bit EL0


sync_invalid_el1t:
    handle_invalid_entry SynchronousEL1t

irq_invalid_el1t:
    handle_invalid_entry IRQEL1t

fiq_invalid_el1t:
    handle_invalid_entry FIQEL1t

error_invalid_el1t:
    handle_invalid_entry ErrorEL1t


sync_invalid_el1h:
    handle_invalid_entry SynchronousEL1h

el1_irq:
    handle_invalid_entry IRQEL1h

fiq_invalid_el1h:
    handle_invalid_entry FIQEL1h

error_invalid_el1h:
    handle_invalid_entry ErrorEL1h


sync_invalid_el0_64:
    handle_invalid_entry SynchronousEL0_64

irq_invalid_el0_64:
    handle_invalid_entry IRQEL0_64

fiq_invalid_el0_64:
    handle_invalid_entry FIQEL0_64

error_invalid_el0_64:
    handle_invalid_entry ErrorEL0_64


sync_invalid_el0_32:
    handle_invalid_entry SynchronousEL0_32

irq_invalid_el0_32:
    handle_invalid_entry IRQEL0_32

fiq_invalid_el0_32:
    handle_invalid_entry FIQEL0_32

error_invalid_el0_32:
    handle_invalid_entry ErrorEL0_32


.globl set_exception_vector_table
set_exception_vector_table:
    adr x1, exception_vector_table
    msr vbar_el1, x1
    ret

