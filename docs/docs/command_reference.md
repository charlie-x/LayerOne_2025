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

## Hardware Control

### RGB LED Control

| Command | Description | Example |
|---------|-------------|---------|
| `rgb on` | Turn on all RGB LEDs | `rgb on` |
| `rgb off` | Turn off all RGB LEDs | `rgb off` |
| `rgb r` | Toggle red LED | `rgb r` |
| `rgb g` | Toggle green LED | `rgb g` |
| `rgb b` | Toggle blue LED | `rgb b` |
| `rgb pwm:r,g,b` | Set PWM brightness (0.0-1.0) | `rgb pwm:1.0,0.5,0.0` |

### Power Control

| Command | Description | Example |
|---------|-------------|---------|
| `3v3 on` | Turn on the 3.3V rail | `3v3 on` |
| `3v3 off` | Turn off the 3.3V rail | `3v3 off` |

### GPIO Control

| Command | Description | Example |
|---------|-------------|---------|
| `gpio` | Show status of all GPIO pins | `gpio` |
| `gpio set <pin> <action>` | Control individual pins | `gpio set 16 high` |
| `pins` | Alias for the `gpio` command | `pins` |

**GPIO Actions:**
- `input` - Set pin as input
- `output [value]` - Set as output with optional value
- `function <func>` - Set pin function
- `high/1` - Set pin high
- `low/0` - Set pin low
- `reset` - Reset pin to default state

## FPGA Commands

| Command | Description | Example |
|---------|-------------|---------|
| `ice reset on\|off\|pulse` | Control FPGA reset pin | `ice reset pulse` |
| `ice clock on\|off` | Control 12MHz clock | `ice clock on` |
| `ice done` | Check DONE signal | `ice done` |
| `ice status` | Show FPGA status | `ice status` |
| `ice init` | initialise FPGA interfaces | `ice init` |
| `ice start [source]` | Start FPGA (spi, nvcm, flash) | `ice start spi` |
| `ice stop` | Stop FPGA | `ice stop` |
| `ice spi init\|deinit\|test\|status` | SPI interface control | `ice spi init` |

## Glitching Commands

| Command | Description | Example |
|---------|-------------|---------|
| `glitch delay <us>` | Set delay in microseconds | `glitch delay 100` |
| `glitch duration <us>` | Set duration in microseconds | `glitch duration 10` |
| `glitch trigger` | Trigger the glitch generator | `glitch trigger` |
| `glitch status` | Show current settings | `glitch status` |
| `g` | Shortcut to trigger the glitch generator | `g` |

## Crowbar Commands

| Command | Description | Example |
|---------|-------------|---------|
| `crowbar delay <us>` | Set delay in microseconds | `crowbar delay 100` |
| `crowbar duration <us>` | Set duration in microseconds | `crowbar duration 5` |
| `crowbar trigger` | Trigger the crowbar circuit | `crowbar trigger` |
| `crowbar status` | Show current settings | `crowbar status` |
| `c` | Shortcut to trigger the crowbar circuit | `c` |

## ADC Commands

| Command | Description | Example |
|---------|-------------|---------|
| `adc voltage\|current\|all` | Read ADC values | `adc voltage` |
| `adc_stream start\|stop\|status` | Control ADC streaming to CDC2 | `adc_stream start` |

## Communication Interfaces

### UART Commands

| Command | Description | Example |
|---------|-------------|---------|
| `uart send <text>` | Send text to debug UART | `uart send "Hello, World!"` |
| `uart read` | Read from debug UART | `uart read` |

### SWD Commands

| Command | Description | Example |
|---------|-------------|---------|
| `swd reset` | Reset SWD interface | `swd reset` |
| `swd read <len>` | Read bytes from SWD | `swd read 4` |
| `swd write <bytes>` | Write hex bytes to SWD | `swd write 0x01 0x02` |
| `swd idcode` | Read CPU ID code | `swd idcode` |

### AVRISP Commands

**Standard AVRISP (fixed pins):**

| Command | Description | Example |
|---------|-------------|---------|
| `avrisp init\|deinit` | initialise/deinitialise interface | `avrisp init` |
| `avrisp detect` | Detect AVR chip | `avrisp detect` |
| `avrisp fuses read\|write` | Fuse operations | `avrisp fuses read` |
| `avrisp erase` | Erase chip | `avrisp erase` |
| `avrisp program <file>` | Program hex file | `avrisp program firmware.hex` |
| `avrisp verify <file>` | Verify hex file | `avrisp verify firmware.hex` |
| `avrisp run` | Run AVRISP service on CDC1 | `avrisp run` |

**Configurable AVRISP2/AB:**

| Command | Description | Example |
|---------|-------------|---------|
| `avrisp2 init [mode]` | initialise with mode (spi\|bitbang) | `avrisp2 init spi` |
| `avrisp2 init pins <assignments>` | Custom pin assignments | `avrisp2 init pins sck=2 miso=3 mosi=4 reset=5` |
| `avrisp2 pins` | Show/set pin assignments | `avrisp2 pins` |
| `avrisp2 detect\|erase` | Basic operations | `avrisp2 detect` |
| `avrisp2 fuses read\|write` | Fuse operations | `avrisp2 fuses read` |
| `avrisp2 lock read\|on\|off` | Lock bit operations | `avrisp2 lock read` |
| `ab <command>` | Shortcut for avrisp2 commands | `ab detect` |

## System Commands

| Command | Description | Example |
|---------|-------------|---------|
| `system reboot` | Reboot the badge | `system reboot` |
| `system dfu` | Reboot into DFU mode | `system dfu` |
| `test [component] [count]` | Run tests with optional repeat count | `test gpio 5` |
| `debug on\|off\|status` | Control debug output | `debug on` |

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
| `dap init\|deinit` | initialise/deinitialise DAP | `dap init` |
| `dap status` | Show DAP status | `dap status` |
| `dap connect\|disconnect` | Target connection control | `dap connect` |
| `dap reset` | Reset target | `dap reset` |
| `dap idcode` | Read target IDCODE | `dap idcode` |
| `dap locked` | Check if target is locked | `dap locked` |

## Interface Commands

| Command | Description | Example |
|---------|-------------|---------|
| `interface list` | List available interfaces | `interface list` |
| `interface uart0\|uart1\|usb0\|usb1\|usb2` | Switch interface | `interface uart0` |

## File System Commands

| Command | Description | Example |
|---------|-------------|---------|
| `ls` | List filesystem contents | `ls` |
| `cat <filename>` | Display file contents | `cat script.txt` |
| `exec <filename>` | Execute command file | `exec startup.txt` |
| `format` | Reinitialise filesystem (erases all) | `format` |

## Script Commands

| Command | Description | Example |
|---------|-------------|---------|
| `script run <filename>` | Run script file | `script run test.scr` |
| `script load <filename>` | Load script | `script load test.scr` |
| `script list` | List available scripts | `script list` |
| `script vars` | Show script variables | `script vars` |

## Timing Commands

| Command | Description | Example |
|---------|-------------|---------|
| `delay_ns <ns>` | Delay for nanoseconds | `delay_ns 1000` |
| `sleep_ms <ms>` | Sleep for milliseconds | `sleep_ms 100` |

## Easter Eggs

| Command | Description | Example |
|---------|-------------|---------|
| `wopr start\|stop\|status` | Control WOPR simulation | `wopr start` |

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