CC = aarch64-linux-gnu-gcc
CS = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy
CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles


LDFLAGS = -T linker.ld

all: kernel.img

kernel.elf: reboot.o boot.o uart.o
	$(LD) $(LDFLAGS) -o kernel.elf reboot.o shell.o uart.o

reboot.o: reboot/reboot.c reboot/reboot.h
	$(CC) $(CFLAGS) -c reboot/reboot.c -o reboot.o

boot.o: boot.S
	$(SC) -c boot.S -o boot.o

uart.o: mini_UART/uart.c mini_UART/uart.h
	$(CC) $(CFLAGS) -c mini_UART/uart.c -o uart.o

kernel.img: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.img

clean:
	rm -f *.o *.img
