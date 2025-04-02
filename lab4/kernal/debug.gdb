# 設置架構
set architecture aarch64
set gnutarget elf64-littleaarch64

# 手動定義 fdt_header 結構體
define fdt_header_type
  set $fdt_header_magic_offset = 0
  set $fdt_header_totalsize_offset = 4
  set $fdt_header_off_dt_struct_offset = 8
  set $fdt_header_off_dt_strings_offset = 12
end

layout src

# layout asm

# 加載符號
file kernel8.elf

# 連接到 QEMU
target remote localhost:1234

# 執行結構體定義
fdt_header_type

break cpio_load_program
#break *0x814c0
#break *0x81e4c
#break *0x200000
#break *0x83940
#break *0x8393c
#break *0x20000c
break *0x83ce8
#break process_timers
break add_timer
# 設置調試輸出
set print pretty on
set print array on
set print array-indexes on

# 顯示即將執行的指令
display/i $pc
display/i $current_time
# 顯示 filesize 變數
display current_time
display timer_queue[timer_count].expire_time
display  mrs %0, cntpct_el0
display  time
display after
display freq
display timer_count
display timer_queue[timer_count].expire_time
display timer_queue[timer_count].active


display timer_queue[i].expire_time
display timer_queue[i].active
display i
display time_to_expire
display earliest_time
display still_have
display timer_count

# 開始執行
continue 