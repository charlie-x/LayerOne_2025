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
   glitch setup width=10 delay=100 repeat=1
   ```
   - `width`: Glitch pulse width in clock cycles
   - `delay`: Delay before glitch in clock cycles
   - `repeat`: Number of glitch pulses to generate

3. Arm the glitcher:
   ```
   glitch arm
   ```

4. Trigger the glitch:
   ```
   glitch trigger
   ```
   
   Or use the shortcut:
   ```
   g
   ```

### Advanced Glitching

For more precise control:

```
glitch setup width=5 delay=100 repeat=3 interval=20 trigger=gpio pin=16 edge=rising
```

This configures:
- 3 glitch pulses, each 5 cycles wide
- 20 cycles between each pulse
- Automatically trigger when GPIO16 has a rising edge

### Glitch Scanning

To automatically scan for vulnerable timing windows:

```
glitch scan start=50 end=200 step=5 width=10 target=uart pattern="success"
```

This will:
- Try glitches with delays from 50 to 200 in steps of 5
- Use a glitch width of 10
- Monitor the UART for the pattern "success"
- Report successful glitch parameters

## Crowbar Circuit

The crowbar circuit allows you to briefly short power lines, creating a different type of power disruption.

### Basic Crowbar Usage

1. Connect your target to the crowbar output
2. Configure the crowbar:
   ```
   crowbar setup duration=5 delay=100
   ```
   - `duration`: How long to activate the crowbar in microseconds
   - `delay`: Delay before activation in microseconds

3. Arm the crowbar:
   ```
   crowbar arm
   ```

4. Trigger the crowbar:
   ```
   crowbar trigger
   ```
   
   Or use the shortcut:
   ```
   c
   ```

### Crowbar with External Trigger

```
crowbar setup duration=5 trigger=gpio pin=17 edge=falling
```

This will activate the crowbar when GPIO17 has a falling edge.

## SWD Debugging

The badge can function as an SWD (Serial Wire Debug) adapter for ARM Cortex-M based devices.

### Connecting to a Target

1. Connect the badge to your target:
   - SWDIO → Target SWDIO
   - SWDCLK → Target SWDCLK
   - GND → Target GND

2. Scan for SWD devices:
   ```
   swd scan
   ```

3. Read target ID:
   ```
   swd id
   ```

### Memory Operations

Read memory:
```
swd read 0x20000000 16    # Read 16 bytes from address 0x20000000
```

Write memory:
```
swd write 0x20000000 0x12345678    # Write to memory
```

Dump memory to file:
```
swd dump 0x08000000 0x1000 flash.bin    # Dump 4KB of flash to file
```

### Register Access

Read CPU registers:
```
swd reg read
```

Write to a register:
```
swd reg write r0 0x12345678
```

### Using with CMSIS-DAP

For more advanced debugging, switch to DAP mode:
```
usb mode dap
```

Then use tools like pyOCD, OpenOCD, or ARM Keil to debug your target.

## AVR Programming (AVRISP)

The badge can program AVR microcontrollers using the standard ISP protocol.

### Connecting to an AVR

1. Connect the badge to your AVR:
   - MOSI → AVR MOSI
   - MISO → AVR MISO
   - SCK → AVR SCK
   - RESET → AVR RESET
   - GND → AVR GND
   - VCC → AVR VCC (optional)

2. Initialize the AVRISP interface:
   ```
   avrisp init
   ```

3. Identify the connected AVR:
   ```
   avrisp identify
   ```

### Programming an AVR

Read the flash:
```
avrisp read flash output.hex
```

Program the flash:
```
avrisp program firmware.hex
```

Read fuses:
```
avrisp read fuses
```

Write fuses:
```
avrisp write lfuse 0xFF
avrisp write hfuse 0xD9
avrisp write efuse 0xFF
```

## Analog Monitoring

The badge includes analog monitoring capabilities for capturing and analyzing signals.

### Basic ADC Reading

Read all ADC channels:
```
adc read
```

Read a specific channel:
```
adc read 0    # Read ADC channel 0
```

### Continuous Monitoring

Start ADC streaming:
```
adc_stream start
```

This will stream ADC readings to the CDC2 interface (in Normal mode) or to a file.

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