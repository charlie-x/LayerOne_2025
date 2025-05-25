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

#### `rgb r|g|b|on|off|pwm:r,g,b`
Controls the RGB LED on the badge.

Examples:
```
rgb on          # Turn on all LEDs
rgb off         # Turn off all LEDs
rgb r           # Toggle red LED
rgb g           # Toggle green LED
rgb b           # Toggle blue LED
rgb pwm:1.0,0.5,0.0  # Set PWM brightness (0.0-1.0)
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

#### `gpio set <pin> <action>`
Controls individual GPIO pins.

Examples:
```
gpio set 16 high        # Set GPIO16 to high
gpio set 17 low         # Set GPIO17 to low
gpio set 18 input       # Set GPIO18 as input
gpio set 19 output 1    # Set GPIO19 as output high
gpio set 20 function 5  # Set GPIO20 to function 5
gpio set 21 reset       # Reset GPIO21 to default
```

#### `pins`
Alias for the `gpio` command.

### FPGA Control

#### `ice reset on|off|pulse`
Controls the FPGA reset pin.

Example:
```
ice reset pulse
```

#### `ice clock on|off`
Controls the 12MHz clock to the FPGA.

Example:
```
ice clock on
```

#### `ice done`
Checks the FPGA DONE signal.

#### `ice init`
initialises FPGA interfaces.

#### `ice start [source]`
Starts the FPGA from specified source (spi, nvcm, flash).

Example:
```
ice start spi
```

#### `ice stop`
Stops the FPGA.

#### `ice status`
Shows the status of the FPGA.

#### `ice spi init|deinit|test|status`
Controls the SPI interface to the FPGA.

Example:
```
ice spi init
```

### Glitching and Hardware Attacks

#### `glitch delay <us>`
Sets the glitch delay in microseconds.

Example:
```
glitch delay 100
```

#### `glitch duration <us>`
Sets the glitch duration in microseconds.

Example:
```
glitch duration 10
```

#### `glitch trigger`
Triggers the glitch generator.

#### `glitch status`
Shows the current glitch settings.

#### `g`
Shortcut to trigger the glitch generator.

#### `crowbar delay <us>`
Sets the crowbar delay in microseconds.

Example:
```
crowbar delay 100
```

#### `crowbar duration <us>`
Sets the crowbar duration in microseconds.

Example:
```
crowbar duration 5
```

#### `crowbar trigger`
Triggers the crowbar circuit.

#### `crowbar status`
Shows the current crowbar settings.

#### `c`
Shortcut to trigger the crowbar circuit.

### ADC (Analog-to-Digital Converter)

#### `adc voltage|current|all`
Reads values from the ADC.

Examples:
```
adc voltage     # Read voltage measurement
adc current     # Read current measurement
adc all         # Read all ADC channels
```

#### `adc_stream start|stop|status`
Controls streaming ADC values to the CDC2 interface.

Examples:
```
adc_stream start    # Start streaming ADC values
adc_stream stop     # Stop streaming ADC values
adc_stream status   # Show streaming status
```

### Communication Interfaces

#### `uart send <text>`
Sends text over the debug UART interface.

Example:
```
uart send "Hello, World!"
```

#### `uart read`
Reads data from the debug UART interface.

#### `swd reset`
Resets the SWD interface.

#### `swd read <len>`
Reads bytes from an SWD device.

Example:
```
swd read 4    # Read 4 bytes
```

#### `swd write <bytes>`
Writes hex bytes to an SWD device.

Example:
```
swd write 0x01 0x02 0x03 0x04
```

#### `swd idcode`
Reads the CPU ID code from an SWD device.

#### Standard AVRISP Commands

#### `avrisp init|deinit`
initialises or deinitialises the AVRISP interface (fixed pins).

#### `avrisp detect`
Detects an AVR device connected via ISP.

#### `avrisp fuses read|write`
Reads or writes AVR fuses.

#### `avrisp erase`
Erases the connected AVR device.

#### `avrisp program <file>`
Programs an AVR device with a hex file.

Example:
```
avrisp program firmware.hex
```

#### `avrisp verify <file>`
Verifies an AVR device against a hex file.

#### `avrisp run`
Runs the AVRISP service on CDC port 1.

#### Configurable AVRISP2/AB Commands

#### `avrisp2 init [mode]`
initialises AVRISP2 with specified mode (spi or bitbang).

Example:
```
avrisp2 init spi
```

#### `avrisp2 init pins <assignments>`
initialises with custom pin assignments.

Example:
```
avrisp2 init pins sck=2 miso=3 mosi=4 reset=5 power=6
```

#### `avrisp2 pins`
Shows or sets pin assignments.

#### `avrisp2 detect|erase`
Basic AVR operations with configurable pins.

#### `avrisp2 fuses read|write`
Fuse operations with configurable pins.

#### `avrisp2 lock read|on|off`
Lock bit operations.

#### `ab <command>`
Shortcut for avrisp2 commands.

Example:
```
ab detect
```

### Debugging and Development

#### `dap init|deinit`
initialises or deinitialises the CMSIS-DAP interface.

#### `dap status`
Shows the status of the CMSIS-DAP interface.

#### `dap connect|disconnect`
Connects to or disconnects from a target device.

#### `dap reset`
Resets the connected target device.

#### `dap idcode`
Reads the IDCODE from the connected target.

#### `dap locked`
Checks if the target device is locked.

#### `interface list`
Lists available communication interfaces.

#### `interface uart0|uart1|usb0|usb1|usb2`
Switches the CLI to a different interface.

Example:
```
interface uart0    # Switch CLI to UART0 interface
```

### System Commands

#### `system reboot`
Reboots the badge.

#### `system dfu`
Reboots the badge into DFU mode.

#### `test [component] [count]`
Runs tests for specific components with optional repeat count.

Examples:
```
test            # Run all tests
test gpio 5     # Test GPIO functionality 5 times
test rgb        # Test RGB LED
test glitch     # Test glitch functionality
test crowbar    # Test crowbar circuit
test adc        # Test ADC
test 3v3        # Test 3.3V rail
test all 3      # Run all tests 3 times
```

#### `debug on|off|status`
Controls debug output.

Examples:
```
debug on        # Enable debug output
debug off       # Disable debug output
debug status    # Show debug status
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

### File System Commands

#### `ls`
Lists the contents of the filesystem.

#### `cat <filename>`
Displays the contents of a file.

Example:
```
cat startup.txt
```

#### `exec <filename>`
Executes commands from a file.

Example:
```
exec startup.txt
```

#### `format`
Reinitialises the filesystem (erases all files).

### Script Commands

#### `script run <filename>`
Runs a script file.

Example:
```
script run test.scr
```

#### `script load <filename>`
Loads a script file.

#### `script list`
Lists available script files.

#### `script vars`
Shows script variables.

### Timing Commands

#### `delay_ns <ns>`
Delays for a specified number of nanoseconds.

Example:
```
delay_ns 1000
```

#### `sleep_ms <ms>`
Sleeps for a specified number of milliseconds.

Example:
```
sleep_ms 100
```

### Easter Eggs

#### `wopr start|stop|status`
Controls the WOPR simulation (War Operation Plan Response).

Examples:
```
wopr start      # Start the WOPR simulation
wopr stop       # Stop the simulation
wopr status     # Show simulation status
```

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