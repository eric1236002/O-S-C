CC = aarch64-linux-gnu-gcc
SC = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld 
OBJCOPY = aarch64-linux-gnu-objcopy
CFLAGS = -ffreestanding -O2 -nostdlib -mcpu=cortex-a53 -I./mini_UART
LDFLAGS = -T linker.ld

all: kernel.img

kernel.elf: shell.o boot.o uart.o
	$(LD) $(LDFLAGS) -o kernel.elf shell.o boot.o uart.o

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c -o shell.o

uart.o: mini_UART/uart.c mini_UART/uart.h
	$(CC) $(CFLAGS) -c mini_UART/uart.c -o uart.o

boot.o: boot.S
	$(SC) -c boot.S -o boot.o

kernel.img: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.img

clean:
	rm -f *.o *.elf *.img

load: kernel.img	
	qemu-system-aarch64 -M raspi3b -kernel kernel.img -serial stdio