# LayerOne 2025 GLiTCh BadgE - USB Modes Guide

## Introduction

The LayerOne 2025 GLiTCh BadgE supports multiple USB modes that change how it appears to your computer and what functionality is available. This guide explains the different USB modes, how to switch between them, and how to use each mode effectively.

## USB Mode Overview

The badge supports three primary USB modes:

1. **Normal Mode**: Provides multiple interfaces for general use
2. **DAP Mode**: Provides debugging capabilities
3. **DFU Mode**: Allows firmware updates

## Switching Between Modes

You can switch between USB modes using the CLI command:

```
usb mode [normal|dap|dfu]
```

For example:
- `usb mode normal` - Switch to Normal mode
- `usb mode dap` - Switch to DAP mode
- `usb mode dfu` - Switch to DFU mode

After switching modes, the badge will reset and re-enumerate as a different USB device. Your computer may show a notification about a new device being connected.

## Normal Mode

### Description

Normal mode is the default mode and provides the most comprehensive set of interfaces for general use.

### Interfaces

In Normal mode, the badge presents itself as:

1. **CDC0 (Serial Port)**: Command Line Interface (CLI)
   - Use this to send commands to the badge
   - Connect using `cli_terminal.py` or any serial terminal at 115200 baud

2. **CDC1 (Serial Port)**: Debug Interface
   - Provides debug output from the badge
   - Connect using any serial terminal at 115200 baud

3. **CDC2 (Serial Port)**: ADC Stream Interface
   - Receives streaming ADC data when enabled
   - Enable with the command `adc_stream start`
   - Connect using any serial terminal at 115200 baud

4. **MSC (Mass Storage)**: Flash Storage
   - Appears as a USB drive on your computer
   - Can be used to transfer files to/from the badge
   - Contains documentation and example files

### Use Cases

- General badge interaction and control
- Accessing the file system
- Monitoring debug output
- Capturing analog data streams

## DAP Mode (CMSIS-DAP)

### Description

DAP mode turns the badge into a CMSIS-DAP debug probe, which can be used to debug the badge itself or external targets.

### Interfaces

In DAP mode, the badge presents itself as:

1. **CMSIS-DAP Interface**: Debug Probe
   - Appears as a CMSIS-DAP compatible debug probe
   - Can be used with tools like pyOCD, OpenOCD, or ARM Keil

2. **CDC0 (Serial Port)**: Command Line Interface (CLI)
   - Still available for sending commands to the badge
   - Connect using `cli_terminal.py` or any serial terminal at 115200 baud

### Use Cases

- Debugging the badge's RP2040 microcontroller
- Debugging external ARM Cortex-M based targets
- Programming flash memory on the badge or external targets
- Hardware hacking and security research

### Using with pyOCD

To use the badge as a CMSIS-DAP probe with pyOCD:

1. Install pyOCD:
   ```
   pip install pyocd
   ```

2. List connected probes:
   ```
   pyocd list
   ```

3. Connect to a target:
   ```
   pyocd commander -t rp2040 -u [serial_number]
   ```
   Where `[serial_number]` is the serial number of your badge (e.g., `00000000000000032`)

4. Example commands in pyOCD:
   ```
   info        # Show target information
   reg         # Show registers
   mem read 0x10000000 16    # Read 16 bytes from address 0x10000000
   mem write 0x20000000 0x12345678    # Write to memory
   ```

## DFU Mode (Device Firmware Update)

### Description

DFU mode allows you to update the firmware on the badge using standard DFU tools.

### Interfaces

In DFU mode, the badge presents itself as:

1. **DFU Interface**: Firmware Update Interface
   - Appears as a standard USB DFU device
   - Can be used with tools like dfu-util

2. **CDC0 (Serial Port)**: Command Line Interface (CLI)
   - Still available for sending commands to the badge
   - Connect using `cli_terminal.py` or any serial terminal at 115200 baud

### Use Cases

- Updating the badge firmware
- Recovering from firmware issues
- Installing custom firmware

### Using with dfu-util

To update firmware using dfu-util:

1. Install dfu-util:
   - On macOS: `brew install dfu-util`
   - On Linux: `sudo apt install dfu-util`
   - On Windows: Download from [dfu-util.sourceforge.net](http://dfu-util.sourceforge.net/)

2. List connected DFU devices:
   ```
   dfu-util -l
   ```

3. Flash new firmware:
   ```
   dfu-util -d 1209:2025 -a 0 -D firmware.bin
   ```
   Where `firmware.bin` is your firmware file

## Troubleshooting

### Device Not Recognized After Mode Switch

If your computer doesn't recognize the badge after switching modes:

1. Wait at least 60 seconds for the device to re-enumerate
2. Unplug and replug the badge
3. Try a different USB port or cable
4. Check if the device appears in Device Manager (Windows), System Information (macOS), or `lsusb` (Linux)

### Can't Connect to Serial Port

If you can't connect to the serial port:

1. Make sure you're using the correct port name
2. Check if another program is using the port
3. Try a different USB port
4. Restart the badge

### CMSIS-DAP Not Working

If the CMSIS-DAP interface isn't working:

1. Make sure you're in DAP mode (`usb mode dap`)
2. Check if the device appears in pyOCD (`pyocd list`)
3. Try updating pyOCD to the latest version
4. Check your target connections if debugging an external device

## Advanced: USB Descriptors

The badge uses the following USB identifiers:

- **Normal Mode**:
  - VID:PID: 0x1209:0x2025
  - Manufacturer: "NullSpaceLabs"
  - Product: "GLiTCh BadgE 2025"

- **DAP Mode**:
  - VID:PID: 0x0D28:0x2488 (Standard CMSIS-DAP VID:PID)
  - Manufacturer: "NullSpaceLabs"
  - Product: "CMSIS-DAP"

- **DFU Mode**:
  - VID:PID: 0x1209:0x2025
  - Manufacturer: "NullSpaceLabs"
  - Product: "GLiTCh BadgE 2025 DFU"

## Testing USB Modes

A comprehensive test script is provided to verify all USB modes:

```
python3 test_usb_modes.py
```

This script will:
1. Test Normal mode functionality
2. Switch to DAP mode and test CMSIS-DAP functionality
3. Switch to DFU mode and verify DFU interface
4. Switch back to Normal mode

You can skip specific tests with command-line options:
```
python3 test_usb_modes.py --skip-build --skip-dfu