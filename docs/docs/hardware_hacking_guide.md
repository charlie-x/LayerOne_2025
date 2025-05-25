# LayerOne 2025 GLiTCh BadgE - Hardware Hacking Guide

## Introduction

The LayerOne 2025 GLiTCh BadgE is designed as a versatile hardware hacking platform with features specifically tailored for security research, glitching attacks, and hardware exploration. This guide explains how to use these features effectively.

## Hardware Hacking Features

The badge includes several specialized hardware features:

1. **Voltage Glitcher**: For performing precise voltage glitching attacks
2. **Crowbar Circuit**: For power line manipulation
3. **SWD Interface**: For debugging ARM-based targets
4. **AVRISP**: For programming AVR microcontrollers
5. **Analog Monitoring**: For capturing and analyzing signals
6. **FPGA**: Programmable logic for custom hardware implementations

## Voltage Glitching

Voltage glitching is a technique where you briefly disrupt the power supply to a target device, potentially causing it to skip instructions or bypass security checks.

### Basic Glitching

1. Connect your target device to the badge's glitching output
2. Configure the glitch parameters:
   ```
   glitch delay 100    # Set delay in microseconds
   glitch duration 10  # Set duration in microseconds
   ```

3. Check the current settings:
   ```
   glitch status
   ```

4. Trigger the glitch:
   ```
   glitch trigger
   ```
   
   Or use the shortcut:
   ```
   g
   ```

### Advanced Glitching Workflow

For systematic glitch testing:

1. Set initial parameters:
   ```
   glitch delay 50     # Start with 50µs delay
   glitch duration 5   # Use 5µs duration
   ```

2. Test the glitch:
   ```
   g                   # Trigger glitch
   ```

3. Adjust parameters and repeat:
   ```
   glitch delay 60     # Try different delay
   g                   # Test again
   ```

4. Monitor target response via UART or other interfaces to determine successful parameters.

## Crowbar Circuit

The crowbar circuit allows you to briefly short power lines, creating a different type of power disruption.

### Basic Crowbar Usage

1. Connect your target to the crowbar output
2. Configure the crowbar:
   ```
   crowbar delay 100      # Set delay in microseconds
   crowbar duration 5     # Set duration in microseconds
   ```

3. Check the current settings:
   ```
   crowbar status
   ```

4. Trigger the crowbar:
   ```
   crowbar trigger
   ```
   
   Or use the shortcut:
   ```
   c
   ```

## SWD Debugging

The badge can function as an SWD (Serial Wire Debug) adapter for ARM Cortex-M based devices.

### Connecting to a Target

1. Connect the badge to your target:
   - SWDIO → Target SWDIO
   - SWDCLK → Target SWDCLK
   - GND → Target GND

2. Reset the SWD interface:
   ```
   swd reset
   ```

3. Read target ID code:
   ```
   swd idcode
   ```

### Basic SWD Operations

Read data from SWD:
```
swd read 4    # Read 4 bytes
```

Write data to SWD:
```
swd write 0x01 0x02 0x03 0x04    # Write hex bytes
```

### Using with CMSIS-DAP

For more advanced debugging, switch to DAP mode:
```
usb mode dap
```

Then use tools like pyOCD, OpenOCD, or ARM Keil to debug your target.

## AVR Programming (AVRISP)

The badge can program AVR microcontrollers using the standard ISP protocol.

### Standard AVRISP (Fixed Pins)

1. Connect the badge to your AVR using the fixed pin assignments:
   - MOSI → AVR MOSI
   - MISO → AVR MISO
   - SCK → AVR SCK
   - RESET → AVR RESET
   - GND → AVR GND
   - VCC → AVR VCC (optional)

2. initialise the AVRISP interface:
   ```
   avrisp init
   ```

3. Detect the connected AVR:
   ```
   avrisp detect
   ```

### Programming with Standard AVRISP

Program the flash:
```
avrisp program firmware.hex
```

Verify the flash:
```
avrisp verify firmware.hex
```

Read/write fuses:
```
avrisp fuses read
avrisp fuses write
```

Erase the chip:
```
avrisp erase
```

Run AVRISP service on CDC1:
```
avrisp run
```

### Configurable AVRISP2/AB

For custom pin assignments:

1. initialise with custom pins:
   ```
   avrisp2 init pins sck=2 miso=3 mosi=4 reset=5 power=6
   ```

2. Or initialise with mode:
   ```
   avrisp2 init spi
   avrisp2 init bitbang
   ```

3. Use AB commands (shortcut):
   ```
   ab detect
   ab erase
   ab fuses read
   ```

## Analog Monitoring

The badge includes analog monitoring capabilities for capturing and analyzing signals.

### Basic ADC Reading

Read voltage measurement:
```
adc voltage
```

Read current measurement:
```
adc current
```

Read all ADC channels:
```
adc all
```

### Continuous Monitoring

Start ADC streaming:
```
adc_stream start
```

This streams ADC readings to the CDC2 interface in Normal USB mode.

Check streaming status:
```
adc_stream status
```

Stop ADC streaming:
```
adc_stream stop
```

### Trigger-Based Capture

Capture ADC data when a trigger condition is met:
```
adc capture channel=0 trigger=rising threshold=2.5 samples=1000 pretrigger=100
```

This captures 1000 samples when channel 0 crosses 2.5V with a rising edge, including 100 samples before the trigger.

## FPGA Programming

The badge includes an ICE40 FPGA that can be programmed for custom hardware implementations.

### Loading a Bitstream

Load a bitstream from a file:
```
ice load custom_design.bin
```

### FPGA Control

Start the FPGA:
```
ice start
```

Stop the FPGA:
```
ice stop
```

Check FPGA status:
```
ice status
```

### FPGA Communication

Send data to the FPGA:
```
ice write 0x01 0x02 0x03 0x04
```

Read data from the FPGA:
```
ice read 4    # Read 4 bytes
```

## Combining Techniques

The real power of the badge comes from combining different techniques.

### Example: Glitch-Based Password Bypass

1. Connect to target UART:
   ```
   uart setup baud=115200 tx=0 rx=1
   ```

2. Set up the glitcher:
   ```
   glitch setup width=10 delay=100 trigger=uart pattern="Password:"
   ```

3. Arm the glitcher:
   ```
   glitch arm
   ```

4. Send login command to target:
   ```
   uart send "login admin"
   ```

The glitcher will automatically trigger when the target prompts for a password.

### Example: SWD Memory Manipulation During Operation

1. Connect to target via SWD
2. Monitor a memory location:
   ```
   swd watch 0x20000100
   ```

3. Set up a trigger to modify memory when a specific value is detected:
   ```
   swd trigger addr=0x20000100 value=0x01 action="write 0x20000104 0x12345678"
   ```

This will automatically write to address 0x20000104 when the value at 0x20000100 becomes 0x01.

## Safety Precautions

When using the hardware hacking features:

1. **Always check voltage levels** before connecting to targets
2. **Start with conservative glitch parameters** to avoid damaging targets
3. **Use current limiting** when possible
4. **Be careful with the crowbar circuit** as it can draw significant current
5. **Disconnect sensitive equipment** when experimenting with new techniques

## Troubleshooting

### Glitcher Not Working

1. Check connections to the target
2. Verify the glitch parameters are appropriate for your target
3. Ensure the target is powered correctly
4. Try different timing parameters

### SWD Connection Issues

1. Check the SWD connections
2. Verify the target is powered
3. Try a slower SWD clock speed:
   ```
   swd setup speed=100
   ```
4. Make sure the target is not in a locked state

### AVRISP Problems

1. Check connections
2. Verify the clock speed is appropriate:
   ```
   avrisp setup speed=100
   ```
3. Make sure the target AVR is powered
4. Try resetting the target before operations