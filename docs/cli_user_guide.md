# LayerOne 2025 GLiTCh BadgE - CLI User Guide

## Introduction

The LayerOne 2025 GLiTCh BadgE is a versatile hardware hacking platform that provides a Command Line Interface (CLI) for controlling various features and functions. This guide explains how to connect to the CLI and use the available commands.

## Connecting to the CLI

### Prerequisites

- Python 3.6 or higher
- PySerial library (`pip install pyserial`)

### Connection Methods

#### 1. Using the CLI Terminal Script

The easiest way to connect to the badge is using the provided `cli_terminal.py` script:

```bash
python3 cli_terminal.py
```

This script will automatically detect your badge and establish a connection. If the script can't find your badge, you can specify the port manually:

```bash
python3 cli_terminal.py /dev/your_port
```

Where `/dev/your_port` is:
- On macOS: typically `/dev/cu.usbmodem11301`
- On Linux: typically `/dev/ttyACM0`
- On Windows: typically `COM3`

#### 2. Using a Serial Terminal

You can also use any serial terminal program (like PuTTY, screen, minicom, etc.) with these settings:
- Baud rate: 115200
- Data bits: 8
- Parity: None
- Stop bits: 1
- Flow control: None

Example with `screen` on macOS/Linux:
```bash
screen /dev/cu.usbmodem11301 115200
```

## USB Modes

The badge supports three USB modes that change how it appears to your computer:

1. **Normal Mode**: Provides 3 CDC (serial) interfaces and a Mass Storage device
2. **DAP Mode**: Provides CMSIS-DAP debugging interface and a CDC interface
3. **DFU Mode**: Device Firmware Update mode for updating the badge firmware

You can switch between these modes using the `usb mode` command (see below).

## Command Reference

### General Commands

#### `help [command]`
Displays help information for all commands or detailed help for a specific command.

Examples:
```
help            # Shows all available commands
help usb        # Shows detailed help for the usb command
help gpio       # Shows detailed help for the gpio command
```

#### `status [subsystem]`
Shows the status of all systems or a specific subsystem.

Examples:
```
status          # Shows status of all subsystems
status usb      # Shows USB status
status gpio     # Shows GPIO status
```

### Hardware Control

#### `rgb r|g|b|on|off`
Controls the RGB LED on the badge.

Examples:
```
rgb on          # Turn on all LEDs
rgb off         # Turn off all LEDs
rgb r           # Toggle red LED
rgb g           # Toggle green LED
rgb b           # Toggle blue LED
```

#### `3v3 on|off`
Controls the 3.3V power rail.

Examples:
```
3v3 on          # Turn on the 3.3V rail
3v3 off         # Turn off the 3.3V rail
```

#### `gpio`
Shows the status of all GPIO pins.

#### `gpio set <pin> <value>`
Sets a GPIO pin to a specific value.

Examples:
```
gpio set 16 1   # Set GPIO16 to high
gpio set 17 0   # Set GPIO17 to low
```

#### `pins`
Alias for the `gpio` command.

### FPGA Control

#### `ice load <file>`
Loads a bitstream into the ICE40 FPGA.

Example:
```
ice load blinky.bin
```

#### `ice start`
Starts the FPGA.

#### `ice stop`
Stops the FPGA.

#### `ice status`
Shows the status of the FPGA.

### Glitching and Hardware Attacks

#### `glitch setup <params>`
Sets up the glitch generator with specific parameters.

Example:
```
glitch setup width=10 delay=100
```

#### `glitch arm`
Arms the glitch generator.

#### `glitch trigger`
Triggers the glitch generator.

#### `g`
Shortcut to trigger the glitch generator.

#### `crowbar setup <params>`
Sets up the crowbar circuit with specific parameters.

#### `crowbar arm`
Arms the crowbar circuit.

#### `crowbar trigger`
Triggers the crowbar circuit.

#### `c`
Shortcut to trigger the crowbar circuit.

### ADC (Analog-to-Digital Converter)

#### `adc read [channel]`
Reads the value from an ADC channel.

Examples:
```
adc read        # Read all ADC channels
adc read 0      # Read ADC channel 0
```

#### `adc_stream start|stop`
Starts or stops streaming ADC values to the CDC2 interface.

Examples:
```
adc_stream start    # Start streaming ADC values
adc_stream stop     # Stop streaming ADC values
```

### Communication Interfaces

#### `uart setup <params>`
Sets up the UART interface with specific parameters.

Example:
```
uart setup baud=115200 tx=0 rx=1
```

#### `uart send <data>`
Sends data over the UART interface.

Example:
```
uart send "Hello, World!"
```

#### `swd scan`
Scans for SWD devices.

#### `swd read <addr> [len]`
Reads memory from an SWD device.

Example:
```
swd read 0x20000000 16    # Read 16 bytes from address 0x20000000
```

#### `swd write <addr> <data>`
Writes data to an SWD device.

Example:
```
swd write 0x20000000 0x12345678
```

#### `avrisp identify`
Identifies an AVR device connected via ISP.

#### `avrisp program <file>`
Programs an AVR device with a hex file.

Example:
```
avrisp program firmware.hex
```

### Debugging and Development

#### `dap info`
Shows information about the CMSIS-DAP interface.

#### `dap connect`
Connects to a target device using CMSIS-DAP.

#### `interface list`
Lists available interfaces.

#### `interface switch <interface>`
Switches the CLI to a different interface.

Example:
```
interface switch uart    # Switch CLI to UART interface
```

### System Commands

#### `system reboot`
Reboots the badge.

#### `system dfu`
Reboots the badge into DFU mode.

#### `test [component]`
Runs tests for specific components.

Examples:
```
test            # Run all tests
test gpio       # Test GPIO functionality
```

#### `usb mode normal|dap|dfu`
Switches the USB mode.

Examples:
```
usb mode normal     # Switch to normal mode (3 CDC + MSC)
usb mode dap        # Switch to DAP mode (CMSIS-DAP + CDC)
usb mode dfu        # Switch to DFU mode
```

#### `usb status`
Shows the current USB status.

### Easter Eggs

#### `wopr`
Launches the WOPR simulation. (War Operation Plan Response)

## USB Mode Details

### Normal Mode
- 3 CDC interfaces:
  - CDC0: CLI interface
  - CDC1: Debug interface
  - CDC2: ADC Stream interface
- MSC (Mass Storage) device

### DAP Mode
- CMSIS-DAP interface for debugging
- CDC0: CLI interface

### DFU Mode
- Device Firmware Update mode
- Used for updating the badge firmware

## Tips and Tricks

1. **Command History**: Press the up and down arrow keys to navigate through command history.

2. **Tab Completion**: Some terminal emulators support tab completion for commands.

3. **Automatic Reconnection**: The `cli_terminal.py` script will automatically reconnect if the connection is lost.

4. **Multiple Interfaces**: In normal mode, you can connect to all three CDC interfaces simultaneously.

5. **Debugging with CMSIS-DAP**: In DAP mode, you can use tools like pyOCD or OpenOCD to debug the badge or external targets.

## Troubleshooting

### Badge Not Detected

If the badge is not detected:

1. Check the USB connection
2. Try a different USB port or cable
3. Restart the badge by unplugging and plugging it back in
4. Make sure you have the correct drivers installed (especially on Windows)

### Wrong USB Mode

If you need to change the USB mode but can't access the CLI:

1. Try connecting to a different CDC interface
2. Use the hardware reset button while holding a specific key to force a mode
3. If all else fails, reflash the firmware

### Command Not Working

If a command is not working as expected:

1. Check the command syntax using `help <command>`
2. Make sure the required hardware is connected and powered
3. Check the status of the relevant subsystem using `status <subsystem>`
4. Try rebooting the badge using `system reboot`