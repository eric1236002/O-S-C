set architecture aarch64
file bootloader.elf
target remote localhost:1234

# 設置關鍵斷點
break *0x80000        
break *0x80014        
break *0x60050       
break *0x60078       

# 設置內存檢查點
break *0x60028       

commands 5
  print "循環檢查 - x0=$x0 x1=$x1 x2=$x2"
  continue
end

# 設置寄存器監視
display/i $pc       
display/x $x10        
display/x $x11       
display/x $x30        

# 啟動執行並輸出日誌
printf "開始調試...\n"
printf "1. _start@0x80000 應先執行\n" 
printf "2. 重定位到0x60000 應成功\n"
printf "3. 然後跳轉到0x60050 (master)\n"

continue 