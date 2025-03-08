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


CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -I$(INCLUDE_DIR)
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
    -serial null -serial pty \
    -display none \

.PHONY: all clean load
