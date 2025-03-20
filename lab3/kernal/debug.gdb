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

# 加載符號
file kernel8.elf

# 連接到 QEMU
target remote localhost:1234

# 執行結構體定義
fdt_header_type

# 設置常用斷點
break fdt_traverse
commands
    # 打印 DTB header 信息
    printf "檢查 DTB header:\n"
    printf "Magic: 0x%x\n", *(uint32_t *)(fdt + $fdt_header_magic_offset)
    printf "Total size: 0x%x\n", *(uint32_t *)(fdt + $fdt_header_totalsize_offset)
    printf "Struct offset: 0x%x\n", *(uint32_t *)(fdt + $fdt_header_off_dt_struct_offset)
    printf "Strings offset: 0x%x\n", *(uint32_t *)(fdt + $fdt_header_off_dt_strings_offset)
    continue
end

break initramfs_callback
commands
    # 打印 initramfs 信息
    printf "initramfs 回調函數:\n"
    x/s node_name
    x/s property_name
    x/wx property_value
    continue
end

# 監視 initramfs_start 變量
watch (unsigned long)initramfs_start

# 設置調試輸出
set print pretty on
set print array on
set print array-indexes on

# 顯示即將執行的指令
display/i $pc

# 開始執行
continue 