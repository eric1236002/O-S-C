
CROSS_COMPILE = aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy


ASFLAGS = -g
LDFLAGS = -T linker.ld


KERNEL = kernel8.img
ELF = kernel8.elf
OBJS = a.o


all: $(KERNEL)


$(KERNEL): $(ELF)
	$(OBJCOPY) -O binary $(ELF) $(KERNEL)


$(ELF): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $(ELF)


%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@


clean:
	rm -f $(OBJS) $(ELF) $(KERNEL)

load: kernel8.img	
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none

.PHONY: all clean load