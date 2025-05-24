# LayerOne 2025 GLiTCh BadgE - Command Reference

This document provides a quick reference for all CLI commands available on the LayerOne 2025 GLiTCh BadgE.

## Command Syntax

Commands follow this general syntax:
```
command [subcommand] [parameters]
```

For detailed help on any command, use:
```
help [command]
```

## General Commands

| Command | Description | Example |
|---------|-------------|---------|
| `help [command]` | Show help for all commands or a specific command | `help usb` |
| `status [subsystem]` | Show status of all systems or a specific subsystem | `status gpio` |
| `exit` | Exit the CLI terminal | `exit` |

## Hardware Control

### RGB LED Control

| Command | Description | Example |
|---------|-------------|---------|
| `rgb on` | Turn on all RGB LEDs | `rgb on` |
| `rgb off` | Turn off all RGB LEDs | `rgb off` |
| `rgb r` | Toggle red LED | `rgb r` |
| `rgb g` | Toggle green LED | `rgb g` |
| `rgb b` | Toggle blue LED | `rgb b` |

### Power Control

| Command | Description | Example |
|---------|-------------|---------|
| `3v3 on` | Turn on the 3.3V rail | `3v3 on` |
| `3v3 off` | Turn off the 3.3V rail | `3v3 off` |

### GPIO Control

| Command | Description | Example |
|---------|-------------|---------|
| `gpio` | Show status of all GPIO pins | `gpio` |
| `gpio set <pin> <value>` | Set a GPIO pin to a specific value | `gpio set 16 1` |
| `pins` | Alias for the `gpio` command | `pins` |

## FPGA Commands

| Command | Description | Example |
|---------|-------------|---------|
| `ice load <file>` | Load a bitstream into the FPGA | `ice load blinky.bin` |
| `ice start` | Start the FPGA | `ice start` |
| `ice stop` | Stop the FPGA | `ice stop` |
| `ice status` | Show FPGA status | `ice status` |
| `ice write <data>` | Write data to the FPGA | `ice write 0x01 0x02` |
| `ice read <length>` | Read data from the FPGA | `ice read 4` |

## Glitching Commands

| Command | Description | Example |
|---------|-------------|---------|
| `glitch setup <params>` | Configure the glitch generator | `glitch setup width=10 delay=100` |
| `glitch arm` | Arm the glitch generator | `glitch arm` |
| `glitch trigger` | Trigger the glitch generator | `glitch trigger` |
| `glitch scan <params>` | Scan for vulnerable glitch parameters | `glitch scan start=50 end=200 step=5` |
| `g` | Shortcut to trigger the glitch generator | `g` |

## Crowbar Commands

| Command | Description | Example |
|---------|-------------|---------|
| `crowbar setup <params>` | Configure the crowbar circuit | `crowbar setup duration=5 delay=100` |
| `crowbar arm` | Arm the crowbar circuit | `crowbar arm` |
| `crowbar trigger` | Trigger the crowbar circuit | `crowbar trigger` |
| `c` | Shortcut to trigger the crowbar circuit | `c` |

## ADC Commands

| Command | Description | Example |
|---------|-------------|---------|
| `adc read [channel]` | Read ADC values | `adc read 0` |
| `adc_stream start` | Start streaming ADC values | `adc_stream start` |
| `adc_stream stop` | Stop streaming ADC values | `adc_stream stop` |
| `adc capture <params>` | Capture ADC data with trigger | `adc capture channel=0 trigger=rising` |

## Communication Interfaces

### UART Commands

| Command | Description | Example |
|---------|-------------|---------|
| `uart setup <params>` | Configure UART interface | `uart setup baud=115200 tx=0 rx=1` |
| `uart send <data>` | Send data over UART | `uart send "Hello, World!"` |
| `uart read [length]` | Read data from UART | `uart read 16` |

### SWD Commands

| Command | Description | Example |
|---------|-------------|---------|
| `swd scan` | Scan for SWD devices | `swd scan` |
| `swd id` | Read target ID | `swd id` |
| `swd read <addr> [len]` | Read memory from target | `swd read 0x20000000 16` |
| `swd write <addr> <data>` | Write data to target | `swd write 0x20000000 0x12345678` |
| `swd reg read [reg]` | Read CPU registers | `swd reg read r0` |
| `swd reg write <reg> <value>` | Write to CPU register | `swd reg write r0 0x12345678` |
| `swd dump <addr> <len> <file>` | Dump memory to file | `swd dump 0x08000000 0x1000 flash.bin` |

### AVRISP Commands

| Command | Description | Example |
|---------|-------------|---------|
| `avrisp init` | Initialize AVRISP interface | `avrisp init` |
| `avrisp identify` | Identify connected AVR | `avrisp identify` |
| `avrisp read <type> [file]` | Read from AVR | `avrisp read flash output.hex` |
| `avrisp program <file>` | Program AVR with hex file | `avrisp program firmware.hex` |
| `avrisp write <fuse> <value>` | Write fuse value | `avrisp write lfuse 0xFF` |

## System Commands

| Command | Description | Example |
|---------|-------------|---------|
| `system reboot` | Reboot the badge | `system reboot` |
| `system dfu` | Reboot into DFU mode | `system dfu` |
| `test [component]` | Run tests | `test gpio` |

## USB Commands

| Command | Description | Example |
|---------|-------------|---------|
| `usb mode normal` | Switch to normal mode (3 CDC + MSC) | `usb mode normal` |
| `usb mode dap` | Switch to DAP mode (CMSIS-DAP + CDC) | `usb mode dap` |
| `usb mode dfu` | Switch to DFU mode | `usb mode dfu` |
| `usb status` | Show USB status | `usb status` |

## DAP Commands

| Command | Description | Example |
|---------|-------------|---------|
| `dap info` | Show DAP interface information | `dap info` |
| `dap connect` | Connect to target via CMSIS-DAP | `dap connect` |
| `dap disconnect` | Disconnect from target | `dap disconnect` |

## Interface Commands

| Command | Description | Example |
|---------|-------------|---------|
| `interface list` | List available interfaces | `interface list` |
| `interface switch <interface>` | Switch CLI to different interface | `interface switch uart` |

## Easter Eggs

| Command | Description | Example |
|---------|-------------|---------|
| `wopr` | Launch WOPR simulation | `wopr` |

## Command Parameters

Many commands accept parameters in the following formats:

1. **Positional parameters**:
   ```
   command param1 param2
   ```

2. **Named parameters**:
   ```
   command name1=value1 name2=value2
   ```

3. **Mixed parameters**:
   ```
   command param1 name2=value2
   ```

## Parameter Types

| Type | Description | Example |
|------|-------------|---------|
| Integer | Whole number | `123`, `0x7B` (hex) |
| Float | Decimal number | `3.14` |
| String | Text (quote if it contains spaces) | `"Hello World"` |
| Boolean | True/false value | `true`, `false`, `1`, `0` |
| Pin | GPIO pin number | `16` |
| Address | Memory address | `0x20000000` |

## Common Parameter Names

| Name | Description | Example |
|------|-------------|---------|
| `width` | Width of pulse in clock cycles | `width=10` |
| `delay` | Delay in clock cycles | `delay=100` |
| `repeat` | Number of repetitions | `repeat=3` |
| `trigger` | Trigger source | `trigger=gpio` |
| `pin` | GPIO pin number | `pin=16` |
| `edge` | Edge type for trigger | `edge=rising` |
| `channel` | ADC channel | `channel=0` |
| `baud` | Baud rate for UART | `baud=115200` |
| `tx` | TX pin for UART | `tx=0` |
| `rx` | RX pin for UART | `rx=1` |