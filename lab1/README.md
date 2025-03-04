# Lab 1 - Bare-metal Programming on Raspberry Pi

A bare-metal programming experiment for Raspberry Pi 3B+ implementing basic kernel features and peripheral drivers.

## Features
- Minimal ARM64 kernel booting via custom bootloader
- UART serial communication (115200 baud rate)
- Mailbox interface for hardware communication
- Basic shell interface over serial


## Building
```bash
make all
```

## Qemu test
```bash
make load
```

## Usage
1. Connect serial adapter to GPIO pins:
   - TX -> GPIO 14 (Pin 8)
   - RX -> GPIO 15 (Pin 10)
2. Insert SD card and power on
3. Connect via serial terminal (e.g. PuTTY/minicom)
4. Available shell commands:
   - `help`: Show command list
   - `hello`: Hello! world
   - `mbox`: Mailbox interface test

## Project Structure
```
├── boot.S        # Primary boot sequence
├── linker.ld     # Memory layout
├── Makefile      # Build system
├── shell.c/h     # Command line interface
├── uart.c/h      # Mini UART driver
└── mailbox/      # Mailbox protocol implementation
```

[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/AaJgSZKl)
