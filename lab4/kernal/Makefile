CC = aarch64-linux-gnu-gcc
AS = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

BUILD_DIR = build
INCLUDE_DIR = include
SRC_DIR = src
USER_DIR = user

OBJS = $(BUILD_DIR)/boot.o \
		$(BUILD_DIR)/reboot.o \
		$(BUILD_DIR)/uart.o \
		$(BUILD_DIR)/shell.o \
		$(BUILD_DIR)/cpio.o \
		$(BUILD_DIR)/utils.o \
		$(BUILD_DIR)/allocator.o \
		$(BUILD_DIR)/fdt.o \
		$(BUILD_DIR)/exception_c.o \
		$(BUILD_DIR)/exception_s.o \
		$(BUILD_DIR)/timer.o \
		$(BUILD_DIR)/buddy.o


CFLAGS = -Wall -O0 -g -ffreestanding -nostdlib -nostartfiles -I$(INCLUDE_DIR)
LDFLAGS = -T linker.ld -nostdlib

all: kernel8.img create_rootfs

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

$(USER_DIR)/user.o: $(USER_DIR)/user.s
	$(AS) -c $< -o $@

user.elf: $(USER_DIR)/user.o
	$(LD) -T $(USER_DIR)/user.ld -o $@ $<

user.img: user.elf
	$(OBJCOPY) -O binary $< $@


create_rootfs: user.img
	@mkdir -p ../rootfs
	cp user.img ../rootfs/
	cp kernel8.img .././
	cd ../rootfs && find . -print0 | cpio --null -ov --format=newc > ../initramfs.cpio
	rm -f user.elf user.img
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
