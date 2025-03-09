# CPIO File System Implementation for Raspberry Pi 3

This project implements a simple CPIO file system reader for Raspberry Pi 3, allowing users to list and read files from a CPIO archive.

## Features

- List files in CPIO archive (`ls` command)
- Read file contents (`cat` command)
- Basic shell interface
- Support for newc format CPIO archives

## Prerequisites

- aarch64-linux-gnu toolchain
- QEMU for ARM emulation
- WSL or Linux environment
- make utility

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
+----------------+
```

## Building and Running

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

## Error Handling

- Checks for magic number in CPIO header
- Validates file names
- Handles "TRAILER!!!" marker
- Provides feedback for file not found

## Debug

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
000000a0  43 44 30 46 32 35 30 30  30 30 30 30 31 39 30 30  |CD0F250000001900|
000000b0  30 30 30 30 30 30 30 30  30 30 30 30 35 34 30 30  |0000000000005400|
000000c0  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |0000000000000000|
000000d0  30 30 30 30 30 38 30 30  30 30 30 30 30 30 6f 6e  |00000800000000on|
000000e0  65 2e 74 78 74 00 00 00  66 69 72 73 74 20 74 65  |e.txt...first te|
000000f0  78 74 0d 0a 6f 6e 65 20  74 77 6f 20 74 68 72 65  |xt..one two thre|
00000100  65 00 00 00 30 37 30 37  30 31 30 30 31 35 46 43  |e...0707010015FC|
00000110  43 38 30 30 30 30 38 31  46 46 30 30 30 30 30 33  |C8000081FF000003|
00000120  45 38 30 30 30 30 30 33  45 38 30 30 30 30 30 30  |E8000003E8000000|
00000130  30 31 36 37 43 44 30 46  33 45 30 30 30 30 30 30  |0167CD0F3E000000|
00000140  30 45 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |0E00000000000000|
00000150  35 34 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |5400000000000000|
00000160  30 30 30 30 30 30 30 30  30 38 30 30 30 30 30 30  |0000000008000000|
00000170  30 30 74 77 6f 2e 74 78  74 00 00 00 73 65 63 6f  |00two.txt...seco|
00000180  6e 64 73 20 66 69 6c 65  0d 0a 00 00 30 37 30 37  |nds file....0707|
00000190  30 31 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |0100000000000000|
000001a0  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |0000000000000000|
000001b0  30 30 30 30 30 30 30 30  30 31 30 30 30 30 30 30  |0000000001000000|
000001c0  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 30  |0000000000000000|
```
