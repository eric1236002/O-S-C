## Timer Implementation Overview

The kernel implements a core timer system that leverages the ARM Cortex-A53's built-in timer functionality. This implementation is documented across several files:

### Core Timer Configuration (from timer.c)

```c
void core_timer_enable() {
    unsigned long long freq;
    asm volatile("msr cntp_ctl_el0, %0\n":: "r"(1)); // Enable timer
    asm volatile("mrs %0, cntfrq_el0\n" : "=r"(freq)); // Get timer frequency
    asm volatile("msr cntp_tval_el0, %0\n":: "r"(freq*2)); // Set 2-second interval

    *CORE0_TIMER_IRQ_CTRL = 2; // Disable interrupts
}
```

This function:
1. Enables the core timer by setting bit 0 in the `cntp_ctl_el0` register
2. Reads the system timer frequency from `cntfrq_el0`
3. Sets a 2-second interval by writing `freq*2` to `cntp_tval_el0`
4. Configures the Core0 timer IRQ control register at address `0x40000040`

### Timer Interrupt Handling (from timer.c)

```c
void core_timer_handler() {
    unsigned int freq, current_time;
    asm volatile("mrs x0, cntfrq_el0\n" : "=r"(freq));
    asm volatile("msr cntp_tval_el0, %0\n":: "r"(freq*2));  
    asm volatile("mrs x0, cntpct_el0\n" : "=r"(current_time));
    uart_send_string("After boot. Current time: ");
    uart_send_int(divide(current_time/freq));
    uart_send_string("\n");
}
```

This handler:
1. Reads the timer frequency
2. Resets the timer interval for the next interrupt
3. Reads the current counter value 
4. Calculates and displays the system uptime in seconds

### Exception Handling Integration (from exception_c.c)

The timer is integrated with the exception handling system. In the exception handler function:

```c
case IRQEL0_64:
    core_timer_enable();
    core_timer_handler();
    break;
```

When an IRQ exception occurs from EL0 (user mode), the system:
1. Re-enables the core timer
2. Calls the timer handler function

## ARM Timer Registers Used

| Register | Purpose |
|----------|---------|
| `cntfrq_el0` | System counter frequency |
| `cntp_ctl_el0` | Timer control register |
| `cntp_tval_el0` | Timer counter value |
| `cntpct_el0` | Current physical count |

## Exception Vector Table (from exception_s.S)

The timer interrupts are processed through the exception vector table, which is properly aligned to 0x800 bytes as required by the ARM architecture. The vectors are set up to handle different types of exceptions from various exception levels.

## Documentation

The implementation is documented in `TIMER.md`, which includes:
- Core timer overview
- Register descriptions
- Configuration process
- Interrupt handling flow
- Example code
- Debugging techniques

This timer implementation provides a foundation for system timing functions, allowing the kernel to measure time intervals and perform scheduled operations.
