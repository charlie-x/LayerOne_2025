# LayerOne 2025 GLiTCh BadgE - Quick Start Guide

## Introduction

Welcome to your LayerOne 2025 GLiTCh BadgE! This quick start guide will help you get up and running with your badge in just a few minutes.

## What's in the Box

- LayerOne 2025 GLiTCh BadgE
- USB-C cable
- Quick reference card

## Hardware Overview

![Badge Overview](https://nullspacelabs.com/images/badge_overview.jpg)

1. **USB-C Port** - Connect to your computer
2. **RGB LED** - Status indicator
3. **Reset Button** - Reset the badge
4. **User Button** - Programmable button
5. **GPIO Headers** - Connect to external devices
6. **FPGA** - ICE40 FPGA for custom logic
7. **Target Headers** - Connect to target devices for hacking
8. **Power Switch** - Turn the badge on/off

## Getting Started

### Step 1: Connect the Badge

1. Connect the USB-C cable to your badge
2. Connect the other end to your computer
3. The RGB LED should light up, indicating the badge is powered

### Step 2: Install Required Software

#### Python and Dependencies

1. Install Python 3.6 or higher:
   - Windows: Download from [python.org](https://python.org)
   - macOS: `brew install python3`
   - Linux: `sudo apt install python3 python3-pip`

2. Install required Python packages:
   ```
   pip install pyserial pyocd
   ```

### Step 3: Connect to the CLI

1. Download the badge software from [GitHub](https://github.com/nullspacelabs/layerOne2025)
2. Open a terminal and navigate to the downloaded folder
3. Run the CLI terminal script:
   ```
   python3 cli_terminal.py
   ```
4. You should see the badge prompt:
   ```
   badge>
   ```

### Step 4: Test Basic Commands

Try these commands to verify your badge is working:

```
help            # Show available commands
status          # Show badge status
rgb on          # Turn on the RGB LED
rgb off         # Turn off the RGB LED
gpio            # Show GPIO pin status
adc all         # Read all ADC channels
```

## USB Modes

The badge supports three USB modes:

1. **Normal Mode** (default) - 3 CDC interfaces + Mass Storage
2. **DAP Mode** - CMSIS-DAP debugging interface + CDC
3. **DFU Mode** - Device Firmware Update mode

To switch between modes, use the `usb mode` command:

```
usb mode normal     # Switch to normal mode
usb mode dap        # Switch to DAP mode
usb mode dfu        # Switch to DFU mode
```

After switching modes, the badge will reset and re-enumerate as a different USB device.

## Basic Features

### Controlling the RGB LED

```
rgb on          # Turn on all LEDs
rgb off         # Turn off all LEDs
rgb r           # Toggle red LED
rgb g           # Toggle green LED
rgb b           # Toggle blue LED
```

### Reading ADC Values

```
adc all         # Read all ADC channels
adc voltage     # Read voltage measurement
adc current     # Read current measurement
```

### Controlling GPIO Pins

```
gpio            # Show GPIO status
gpio set 16 high    # Set GPIO16 high
gpio set 17 low     # Set GPIO17 low
gpio set 18 input   # Set GPIO18 as input
```

### System Commands

```
status          # Show system status
system reboot   # Reboot the badge
test all        # Run all hardware tests
debug on        # Enable debug output
```

## Next Steps

Now that you've got the basics working, here are some next steps to explore:

1. **Read the Documentation** - Check out the [full documentation](README.md) for detailed guides
2. **Try Hardware Hacking** - Explore the [Hardware Hacking Guide](hardware_hacking_guide.md)
3. **Experiment with USB Modes** - Learn more in the [USB Modes Guide](usb_modes_guide.md)
4. **Join the Community** - Connect with other badge owners on [Discord](https://discord.gg/nullspacelabs)

## Troubleshooting

### Badge Not Detected

If your computer doesn't detect the badge:

1. Try a different USB cable
2. Try a different USB port
3. Make sure the power switch is on
4. Press the reset button

### Can't Connect to CLI

If you can't connect to the CLI:

1. Make sure the badge is properly connected
2. Check if another program is using the serial port
3. Try specifying the port manually:
   ```
   python3 cli_terminal.py /dev/your_port
   ```
   Where `/dev/your_port` is:
   - On macOS: typically `/dev/cu.usbmodem11301`
   - On Linux: typically `/dev/ttyACM0`
   - On Windows: typically `COM3`

### Badge Not Responding

If the badge is connected but not responding to commands:

1. Press the reset button
2. Disconnect and reconnect the USB cable
3. Try switching USB modes:
   ```
   usb mode normal
   ```

## Getting Help

If you're still having trouble:

1. Check the [full documentation](README.md)
2. Visit the [community forum](https://forum.nullspacelabs.com)
3. Join the [Discord channel](https://discord.gg/nullspacelabs)
4. Email support at [support@nullspacelabs.com](mailto:support@nullspacelabs.com)