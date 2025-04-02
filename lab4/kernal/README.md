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

## Exception Handling & EL0/EL1 Switching

This kernel implements a complete exception handling system for both kernel (EL1) and user (EL0) mode operations.

### Exception Vector Table

The architecture uses a vector table structure with entries for different exception types:
- Synchronous exceptions (e.g., system calls)
- IRQ interrupts
- FIQ fast interrupts
- System errors

Each exception type has specific handlers for different execution levels (EL1t, EL1h, EL0_64, EL0_32).

### Exception Types

- `SynchronousEL1t/h`: Exceptions occurring at EL1 with SP_EL0/SP_EL1
- `IRQEL1t/h`: IRQ interrupts at EL1
- `FIQEL1t/h`: FIQ interrupts at EL1
- `ErrorEL1t/h`: System errors at EL1
- `SynchronousEL0_64/32`: System calls and exceptions from 64-bit/32-bit user mode
- `IRQEL0_64/32`: IRQ interrupts from user mode
- `FIQEL0_64/32`: FIQ interrupts from user mode
- `ErrorEL0_64/32`: System errors from user mode

### EL0/EL1 Switching

The kernel supports execution of user programs at EL0:
1. Programs are loaded from the CPIO archive to memory
2. Exception vector table is configured with `set_exception_vector_table()`
3. Register state is set up for EL0 execution:
   - `SPSR_EL1 = 0x3c0` (EL0, SP_EL0, interrupts enabled)
   - `ELR_EL1` points to the user program entry
   - `SP_EL0` is set to provide user stack space
4. `eret` instruction switches to EL0 and begins user program execution

### System Call Handling

User programs can perform system calls using the `svc` instruction:
1. The instruction triggers a synchronous exception, moving execution to EL1
2. The exception is caught by the vector table handler
3. The `SynchronousEL0_64` case in `exception_entry()` handles the call:
   - Extracts exception information from ESR_EL1
   - For SVC (0x15), processes the requested system call
   - Updates ELR_EL1 to point to the next instruction
4. After handling, execution returns to user mode at the instruction following the SVC

### Context Saving/Restoring

Full register state is preserved during exception handling:
- `save_all`: Saves all general purpose registers to the stack
- `load_all`: Restores all registers before returning from the exception
- Special registers (SPSR_EL1, ELR_EL1, etc.) are managed by the exception handlers

### User Program Example

```assembly
_start:
    mov x0, 0
1:
    add x0, x0, 1
    svc 0           // System call to kernel
    cmp x0, 5
    blt 1b
1:
    b 1b            // Infinite loop when done
```
## Timer and Interrupt Handling

[TIMER.md](TIMER.md)

### Core Timer Configuration
- Core timer frequency is read from `cntfrq_el0`
- Timer control register: `cntp_ctl_el0`
- Timer value register: `cntp_tval_el0`
- Current counter: `cntpct_el0`
- Core0 timer IRQ control: `0x40000040`

### Timer Implementation
- Timer is configured during kernel initialization
- Interrupt interval: 2 seconds (freq * 2)
- Timer handler displays current system uptime
- Core0 timer IRQ control is set to 2 (disable interrupts)

### Timer Registers
- `cntfrq_el0`: Timer frequency
- `cntp_ctl_el0`: Timer control
- `cntp_tval_el0`: Timer value
- `cntpct_el0`: Current counter 
- `0x40000040`: Core0 timer IRQ control

## Memory Layout

Memory Map:
+------------------+ 0x00000000
|                  |
|    Reserved      |
|                  |
+------------------+ 0x00060000
|                  |
|    Bootloader    |    (128KB)
|                  |
+------------------+ 0x00080000
|                  |
|   Kernel Code    |    
|     (.text)      |
|                  |
|   [Exception     |    (Aligned to 0x800 bytes)
|    Vector Table] |    (Set via vbar_el1)
|                  |
+------------------+ 
|                  |
|   Kernel Data    |    
|     (.data)      |
|                  |
+------------------+ 
|                  |
|   Kernel BSS     |    
|     (.bss)       |    
|                  |
+------------------+ 0x00100000
|                  |
|   Kernel Heap    |    (Dynamically allocated memory)
|                  |
+------------------+ 0x00200000
|                  |
|                  |
|   User Code      |    (.text - Loaded from CPIO archive)
|   User Data      |    (.data)
|   User BSS       |    (.bss)
|   User Heap      |    (Dynamically allocated memory)
|                  |
+------------------+ 0x00400000
|                  |
|   Kernel Stack   |    (For EL1 execution)
|                  |    (SP_EL1 points here during kernel mode)
+------------------+
|                  |
| Exception Frames |    (Register state saved during exceptions)
|                  |    (32 registers × 8 bytes per exception)
+------------------+

## Memory Management

### Dynamic Memory Allocation
- Simple heap allocator implementation
- Supports basic malloc operations
- Memory aligned to 8 bytes
- Heap starts after kernel code and data
- No free implementation (simple bump allocator)

## FDT Support

This kernel includes support for Flattened Device Tree (FDT) to facilitate hardware configuration. The Device Tree Blob (DTB) provides the kernel with essential information regarding the system's hardware layout and peripherals.

- The kernel expects an FDT file (in DTB format) to be loaded (typically by the bootloader) or specified in the boot configuration.

## Bootloader Parameter Handoff for DTB and Initramfs Retrieval

During the boot process, the bootloader passes crucial parameters to the kernel through register x0. This mechanism allows the kernel to acquire the address of the Device Tree Blob (DTB) and, subsequently, determine the location of the Initial RAM Filesystem (initramfs).

The process is as follows:
1. The bootloader prepares the DTB and stores its address in register x0 before transferring control to the kernel.
2. Upon kernel startup, the kernel entry point reads the x0 register value and assigns it to a designated variable, `dtb_address`.
3. The kernel parses the DTB at `dtb_address` to extract configuration data, including the initramfs address.
4. With the initramfs address obtained from the DTB, the kernel can locate and mount the initramfs for system initialization.

This seamless handoff ensures that the kernel receives the necessary hardware configuration parameters directly from the bootloader.

### Boot Process Overview
```
Bootloader                        Kernel
    |                              |
    |-- x0 (DTB address) ------>   |
    |     Transfer control         |
    |                              |
    |                     Read x0 to dtb_address
    |                              |
    |                     Parse DTB structure
    |                              |
    |                     Load initramfs
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
kernel=bootloader.img
arm_64bit=1
initramfs initrd.cpio.gz
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

# Windows use it to find uart DEVICE, and share it.
usbipd list
# Then to attach it
usbipd attach --wsl --busid 1-5
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
