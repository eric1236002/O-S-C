.section ".text.boot"
.globl _start

_start:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, master
    b waiting

relocate:
    // 檢查是否已經重定位
    adr x10, _start        // 獲取當前地址
    mov x11, #0x60000 
    cmp x10, x11           // 比較當前地址是否為0x60000
    b.eq skip_relocation   // 如果已經在0x60000，跳過重定位

    // 使用硬編碼地址
    mov x0, #0x80000      // 源地址
    mov x1, #0x60000      // 目標地址
    mov x2, #0x2000       // 固定大小8KB (足夠整個bootloader)

    // 調試暫停點
    nop
    nop
    nop

relocation_loop:
    ldr x3, [x0], #8
    str x3, [x1], #8
    subs x2, x2, #8
    b.gt relocation_loop

    // 清除快取
    dsb sy
    ic iallu
    dsb sy
    isb

    // 跳轉到新地址 - 直接跳到master處而不是開頭
    mov x30, #0x60000
    br x30

skip_relocation:
    // 已經重定位，直接繼續執行
    b main

master:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero

    // 設置堆疊並調用main
    mov sp, #0x400000
    bl relocate          // 先執行重定位
    // 不會回到這裡，relocated代碼會跳到0x60050

memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    b.gt memzero    
    ret

waiting:
    wfe
    b waiting

