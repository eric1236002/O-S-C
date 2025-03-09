# CPIO File System Implementation for Raspberry Pi 3

This project implements a simple CPIO file system reader for Raspberry Pi 3, allowing users to list and read files from a CPIO archive.

## Features

- List files in CPIO archive (`ls` command)
- Read file contents (`cat` command)
- Basic shell interface
- Support for newc format CPIO archives

## Prerequisites

- aarch64-linux-gnu toolchain
- QEMU for ARM emulation (for testing)
- WSL or Linux environment
- make utility
- SD card with at least 32MB
- Raspberry Pi 3
- USB to TTL Serial Cable

## Directory Structure

```
kernal/
├── include/           # Header files
│   ├── cpio.h        # CPIO structure and function declarations
│   ├── uart.h        # UART communication
│   ├── utils.h       # Utility functions
│   └── shell.h       # Shell interface
├── src/              # Source files
│   ├── cpio.c        # CPIO implementation
│   ├── uart.c        # UART implementation
│   ├── utils.c       # Utility functions
│   └── shell.c       # Shell implementation
├── boot.S            # Boot code
└── linker.ld         # Linker script
```

## Memory Layout

```
Memory Map:
+----------------+ 0x00000000
|                |
|                |
+----------------+ 0x20000000
|    INITRD      |  <-- CPIO archive loaded here
|                |
+----------------+ 0x80000
|    KERNEL      |  <-- Kernel code and data
|                |
+----------------+ heap_start
|    HEAP        |  <-- Dynamic memory allocation
|                |
+----------------+ heap_end
```

## Memory Management

### Dynamic Memory Allocation
- Simple heap allocator implementation
- Supports basic malloc operations
- Memory aligned to 8 bytes
- Heap starts after kernel code and data
- No free implementation (simple bump allocator)


```

## Building and Testing

### 1. QEMU Testing
1. Create CPIO archive:
```bash
cd rootfs
find . -print0 | cpio --null -ov --format=newc > ../initramfs.cpio
cd ..
```

2. Build the kernel:
```bash
cd kernal
make clean
make
```

3. Run in QEMU:
```bash
make load
```

### 2. Raspberry Pi Testing

1. Prepare Files:
   First, create the CPIO archive and build the kernel:
```bash
# Create CPIO archive from rootfs
cd rootfs
find . -print0 | cpio --null -ov --format=newc > ../initramfs.cpio
cd ..

# Build kernel
cd kernal
make clean && make
```

2. Prepare SD Card:
   - Create `config.txt` in the root directory with following content:
```
kernel_address=0x60000
kernel=bootloader.img
arm_64bit=1
initramfs initrd.cpio.gz followkernel
initramfs initramfs.cpio 0x20000000
```

3. Copy Required Files to SD Card:
   You only need to copy these two files to the SD card root:
```bash
# Copy kernel and CPIO archive to SD card
cp kernal/kernel8.img /path/to/sd/
cp initramfs.cpio /path/to/sd/
```

3. Setup Serial Connection:
```bash
# Linux/WSL (replace ttyUSB0 with your device)
sudo minicom -D /dev/ttyUSB0 -b 115200

# Windows (replace COM3 with your device)
# Use PuTTY or other terminal emulator
# Speed: 115200
# Data bits: 8
# Stop bits: 1
# Parity: None
# Flow control: None
```

4. Power on Raspberry Pi and test commands:
```
# Basic commands
help    - Show available commands
ls      - List files
cat     - Read file content
reboot  - Reboot system
```

## Shell Commands

- `help`: Display available commands
- `ls`: List all files in the CPIO archive
- `cat`: Display file contents (prompts for filename)
- `reboot`: Reboot the system

## Technical Details

### CPIO Format
- Uses "newc" format (magic number: "070701")
- Header size: 110 bytes
- File data and file name are 4-byte aligned

### Implementation Details
- UART communication at 115200 baud
- Memory-mapped CPIO archive at 0x20000000
- Supports file names up to 100 characters
- 4-byte alignment for both header and data
- Simple bump allocator for dynamic memory
- 8-byte memory alignment for allocations

## Error Handling

- Checks for magic number in CPIO header
- Validates file names
- Handles "TRAILER!!!" marker
- Provides feedback for file not found

## Debug

### Memory Debugging
To check heap usage:
```c
// Print current heap top
uart_send_string("Heap top: ");
uart_send_hex((unsigned long)heap_top);
uart_send_string("\n\r");
```

```bash
hexdump -C ../initramfs.cpio | head -n 30
```
Can use hexdump to check the CPIO archive, so we can see the file name and file data. Magic number always be the first 6 bytes.
```
00000000  30 37 30 37 30 31 30 30  31 35 46 44 37 43 30 30  |0707010015FD7C00|
00000010  30 30 34 31 46 46 30 30  30 30 30 33 45 38 30 30  |0041FF000003E800|
00000020  30 30 30 33 45 38 30 30  30 30 30 30 30 31 36 37  |0003E80000000167|
00000030  43 44 30 46 32 44 30 30  30 30 30 30 30 30 30 30  |CD0F2D0000000000|
00000040  30 30 30 30 30 30 30 30  30 30 30 30 35 34 30 30  |0000000000005400|
00000050  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |0000000000000000|
00000060  30 30 30 30 30 32 30 30  30 30 30 30 30 30 2e 00  |00000200000000..|
00000070  30 37 30 37 30 31 30 30  31 35 46 44 38 31 30 30  |0707010015FD8100|
00000080  30 30 38 31 46 46 30 30  30 30 30 33 45 38 30 30  |0081FF000003E800|
00000090  30 30 30 33 45 38 30 30  30 30 30 30 30 31 36 37  |0003E80000000167|
000000a0  43 44 35 35 37 36 30 30  30 30 30 30 31 39 30 30  |CD55760000001900|
000000b0  30 30 30 30 30 30 30 30  30 30 30 30 35 34 30 30  |0000000000005400|
000000c0  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |0000000000000000|
000000d0  30 30 30 30 30 38 30 30  30 30 30 30 30 30 6f 6e  |00000800000000on|
000000e0  65 2e 74 78 74 00 00 00  66 69 72 73 74 20 74 65  |e.txt...first te|
000000f0  78 74 0d 0a 6f 6e 65 20  74 77 6f 20 74 68 72 65  |xt..one two thre|
00000100  65 00 00 00 30 37 30 37  30 31 30 30 31 35 46 43  |e...0707010015FC|
00000110  43 38 30 30 30 30 38 31  46 46 30 30 30 30 30 33  |C8000081FF000003|
00000120  45 38 30 30 30 30 30 33  45 38 30 30 30 30 30 30  |E8000003E8000000|
00000130  30 31 36 37 43 44 35 35  37 38 30 30 30 30 30 31  |0167CD5578000001|
00000140  45 32 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |E200000000000000|
00000150  35 34 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |5400000000000000|
00000160  30 30 30 30 30 30 30 30  30 38 30 30 30 30 30 30  |0000000008000000|
00000170  30 30 74 77 6f 2e 74 78  74 00 00 00 20 20 20 20  |00two.txt...    |
00000180  2c 6f 38 38 38 38 38 38  6f 2e 20 20 20 20 20 20  |,o888888o.      |
00000190  20 20 64 38 38 38 38 38  38 6f 2e 20 20 20 20 20  |  d888888o.     |
000001a0  20 20 2c 6f 38 38 38 38  38 38 6f 2e 0d 0a 20 2e  |  ,o888888o... .|
000001b0  20 38 38 38 38 20 20 20  20 20 60 38 38 2e 20 20  | 8888     `88.  |
000001c0  20 20 2e 60 38 38 38 38  3a 27 20 60 38 38 2e 20  |  .`8888:' `88. |
000001d0  20 20 20 38 38 38 38 20  20 20 20 20 60 38 38 2e  |   8888     `88.|
```
