CC = aarch64-linux-gnu-gcc
AS = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

BUILD_DIR = build
INCLUDE_DIR = include
SRC_DIR = src

OBJS = $(BUILD_DIR)/boot.o \
       $(BUILD_DIR)/reboot.o \
       $(BUILD_DIR)/uart.o \
       $(BUILD_DIR)/loader.o \
       $(BUILD_DIR)/shell.o \
       $(BUILD_DIR)/fdt.o \
       $(BUILD_DIR)/utils.o \
       $(BUILD_DIR)/cpio.o


CFLAGS = -Wall -O0 -ffreestanding -nostdlib -nostartfiles -I$(INCLUDE_DIR) -g
LDFLAGS = -T linker.ld -nostdlib

all: bootloader.img

bootloader.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(@D)
	$(AS) -c $< -o $@


$(BUILD_DIR)/boot.o: boot.S
	@mkdir -p $(@D)
	$(AS) -c $< -o $@


bootloader.img: bootloader.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f bootloader.elf bootloader.img

load: bootloader.img
	qemu-system-aarch64 -M raspi3b \
    -kernel bootloader.img \
    -dtb ../bcm2710-rpi-3-b-plus.dtb \
    -initrd ../initramfs.cpio \
    -serial null -serial pty \
    -display none \

debug: bootloader.img
	qemu-system-aarch64 -M raspi3b \
    -kernel bootloader.img \
    -dtb ../bcm2710-rpi-3-b-plus.dtb \
    -initrd ../initramfs.cpio \
    -serial null -serial pty \
    -display none \
    -S -s
# 在WSL終端1:
# cd /mnt/d/Cloud/O-S-C/lab2/bootloader && make clean && make

# 在WSL終端1:
# cd /mnt/d/Cloud/O-S-C/lab2/bootloader && make debug

# 在WSL終端2:
# cd /mnt/d/Cloud/O-S-C/lab2/bootloader && gdb-multiarch -x debug.gdb

.PHONY: all clean load debug
