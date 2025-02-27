.section ".text.boot"
.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0,#0xFF
    cbz x0, master
    bne waiting
    b _start

master:
    mov     x0, _bss_start   // 設置x0為BSS段的起始地址
    mov     x1, _bss_end     // 設置x1為BSS段的結束地址
    loop:
        cmp     x0, x1        // 比較當前地址與結束地址
        bge     end_loop      // 如果已經到達結束地址，跳出循環
        str     wzr, [x0], #4 // 將0寫入地址並遞增
        b       loop
    end_loop:

    ldr x0, =stack_top
    mov sp, x0   

    // Enable Mini UART
    ldr     x0, =0x7E215004   // Load the physical address of AUXENB
    ldr     w1, [x0]         // Load the current value of AUXENB
    orr     w1, w1, #1       // Set bit 0 (enable mini UART)
    str     w1, [x0]         // Store the modified value back to AUXENB
    

    ldr     x0, =0x7E215060  // 載入 AUX_MU_CNTL_REG 的地址
    mov     w1, #0                // 將 w1 設置為 0（禁用發射器和接收器）
    str     w1, [x0]              // 將 w1 寫入到 AUX_MU_CNTL_REG

    //Set AUX_MU_IER_REG to 0. Disable interrupt because currently you don’t need interrupt.
    ldr     x0, =0x7E215044
    mov     w1, #0                // 將 w1 設置為 0（禁用中斷）
    str     w1, [x0]

    //Set AUX_MU_LCR_REG to 3. Set the data size to 8 bit.
    ldr     x0, =0x7E21500C
    mov     w1, #3                // 將 w1 設置為 3（設置數據大小為 8 位）
    str     w1, [x0]

    //Set AUX_MU_MCR_REG to 0. Don’t need auto flow control.
    ldr     x0, =0x7E215050
    mov     w1, #0                // 將 w1 設置為 0（不需自動流量控制）
    str     w1, [x0]

    //Set AUX_MU_BAUD to 270. Set baud rate to 115200
    ldr     x0, =0x7E215068
    mov     w1, #270
    str     w1, [x0]

    //Set AUX_MU_IIR_REG to 6
    ldr     x0, =0x7E215048
    mov     w1, #6
    str     w1, [x0]

    //Set AUX_MU_CNTL_REG to 3. Enable the transmitter and receiver.
    ldr     x0, =0x7E215060
    mov     w1, #3
    str     w1, [x0]

    ldr     x0, =0x7E200004     // Load the address of GPFSEL1
    ldr     w1, [x0]           // Load the current value of GPFSEL1
    bic     w1, w1, #(0b111 << 12) // Clear the bits for GPIO 14 (bits 12-14)
    bic     w1, w1, #(0b111 << 15) // Clear the bits for GPIO 15 (bits 15-17)
    mov     w2, #0b101
    lsl     w2, w2, #12
    orr     w1, w1, w2        // Set GPIO 14 to ALT5 (101)
    mov     w2, #0b101
    lsl     w2, w2, #15
    orr     w1, w1, w2        // Set GPIO 15 to ALT5 (101)
    str     w1, [x0]           // Store the modified value back to GPFSEL1
    
    //disable GPIO pull-up/down by setting GPPUD and GPPUDCLKn registers.
    ldr     x0, =0x7E200094
    mov     w1, #0
    str     w1, [x0]
    
    // set 14 and 15 pin's pull up/down state
    mov w1,#0x0C000
    ldr x2, =0x7E200098
    str w1,[x2]


waiting:
    wfe
    b _start
    
    .section ".bss"
    .global _bss_start
    .global _bss_end
_bss_start:
    .space 0
_bss_end:
    
.section ".stack"
    .space 4096
    .global stack_top
stack_top:
