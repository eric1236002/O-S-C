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
       $(BUILD_DIR)/bootloader.o \
       $(BUILD_DIR)/shell.o \


CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -I$(INCLUDE_DIR)
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
	qemu-system-aarch64 -M raspi3b \
    -kernel kernel8.img \
    -serial null -serial pty \
    -display none \

.PHONY: all clean load
