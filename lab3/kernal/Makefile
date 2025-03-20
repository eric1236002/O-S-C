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
		$(BUILD_DIR)/shell.o \
		$(BUILD_DIR)/cpio.o \
		$(BUILD_DIR)/utils.o \
		$(BUILD_DIR)/allocator.o \
		$(BUILD_DIR)/fdt.o


CFLAGS = -Wall -O0 -g -ffreestanding -nostdlib -nostartfiles -I$(INCLUDE_DIR)
LDFLAGS = -T linker.ld -nostdlib

all: kernel8.img

kernel8.elf: $(OBJS)
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


kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f kernel8.elf kernel8.img

load: kernel8.img
	qemu-system-aarch64 -M raspi3b -m 1G \
    -kernel kernel8.img \
    -initrd ../initramfs.cpio \
	-dtb ../bcm2710-rpi-3-b-plus.dtb \
    -serial null -serial stdio \
    -display none

debug: kernel8.img
	qemu-system-aarch64 -M raspi3b -m 1G \
    -kernel kernel8.img \
    -initrd ../initramfs.cpio \
	-dtb ../bcm2710-rpi-3-b-plus.dtb \
    -serial null -serial stdio \
    -display none \
    -S -s

.PHONY: all clean load debug
