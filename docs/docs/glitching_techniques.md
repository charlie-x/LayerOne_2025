# LayerOne 2025 GLiTCh BadgE - Glitching Techniques Guide

## Introduction

This guide provides detailed information on how to perform various glitching attacks using the LayerOne 2025 GLiTCh BadgE. Glitching is a technique used in hardware security research to manipulate a device's operation by introducing intentional faults, potentially bypassing security measures or causing unexpected behavior.

## Types of Glitching

The badge supports several types of glitching techniques:

1. **Voltage Glitching**: Briefly disrupting the target's power supply voltage
2. **Clock Glitching**: Manipulating the clock signal to cause timing faults
3. **Reset Glitching**: Manipulating the reset line to cause partial resets
4. **Crowbar Glitching**: Using the crowbar circuit to create power shorts
5. **Electromagnetic Glitching**: Using external EM pulses (with additional hardware)

## Hardware Setup

### General Setup

1. Connect your target device to the badge:
   - Target VCC → Badge glitch output
   - Target GND → Badge GND
   - (Optional) Target clock → Badge clock output
   - (Optional) Target reset → Badge reset output

2. Connect monitoring equipment:
   - Badge ADC inputs → Target test points
   - (Optional) External oscilloscope → Badge test points

### Voltage Glitching Setup

For voltage glitching, the badge provides a controlled power output that can be briefly disrupted:

```
┌─────────┐           ┌─────────┐
│         │  Glitch   │         │
│  Badge  ├───Output──┤  Target │
│         │           │         │
└────┬────┘           └────┬────┘
     │                     │
     └─────────GND─────────┘
```

### Crowbar Glitching Setup

For crowbar glitching, the badge can briefly short the target's power supply:

```
┌─────────┐           ┌─────────┐
│         │  VCC      │         │
│  Badge  ├───Output──┤  Target │
│         │           │         │
└────┬────┘           └────┬────┘
     │      Crowbar        │
     └─────Connection──────┘
```

### Reset Glitching Setup

For reset glitching, connect the badge to the target's reset line:

```
┌─────────┐           ┌─────────┐
│         │  Reset    │         │
│  Badge  ├───Output──┤  Target │
│         │           │         │
└────┬────┘           └────┬────┘
     │                     │
     └─────────GND─────────┘
```

## Voltage Glitching Techniques

Voltage glitching involves briefly disrupting the target's power supply to cause instruction skips or memory corruption.

### Basic Voltage Glitch

1. Configure the glitcher:
   ```
   glitch setup width=10 delay=100 repeat=1
   ```

2. Arm the glitcher:
   ```
   glitch arm
   ```

3. Trigger the glitch:
   ```
   glitch trigger
   ```

### Timing-Based Voltage Glitch

To target specific operations, you need to time the glitch precisely:

1. Configure the glitcher with external trigger:
   ```
   glitch delay 10      # Set delay to 10 microseconds
   glitch duration 5    # Set duration to 5 microseconds
   ```

2. Connect monitoring to observe the target's operation

3. Trigger the glitch at the right moment:
   ```
   glitch trigger
   ```
   
   Or use the shortcut:
   ```
   g
   ```

### Glitch Parameter Tuning

Effective glitching requires finding the right parameters. Start with:

1. **Duration**: 5-20 microseconds
2. **Delay**: 0-200 microseconds  

Then perform manual parameter sweeps:

```
glitch delay 50       # Start with 50µs delay
glitch duration 10    # Start with 10µs duration
g                     # Test
glitch delay 60       # Try different delay
g                     # Test again
```

Monitor the target's response to find successful parameters.

## Crowbar Glitching Techniques

Crowbar glitching creates a more aggressive power disruption by briefly shorting the power supply.

### Basic Crowbar Glitch

1. Configure the crowbar:
   ```
   crowbar delay 100     # Set delay to 100 microseconds
   crowbar duration 5    # Set duration to 5 microseconds
   ```

2. Check the settings:
   ```
   crowbar status
   ```

3. Trigger the crowbar:
   ```
   crowbar trigger
   ```
   
   Or use the shortcut:
   ```
   c
   ```

### Timed Crowbar Glitch

For precise timing, you'll need to manually coordinate the timing:

1. Configure the crowbar parameters:
   ```
   crowbar delay 50      # Adjust based on target timing
   crowbar duration 2    # Very short duration
   ```

2. Monitor the target (e.g., via UART) and trigger at the right moment:
   ```
   c                     # Trigger when ready
   ```

### Crowbar vs. Voltage Glitching

- **Crowbar Glitching**: More aggressive, causes deeper voltage drops, works well for devices with large capacitors
- **Voltage Glitching**: More precise, allows finer control, better for sensitive devices

## Reset Glitching Techniques

Reset glitching manipulates the reset line to cause partial resets of the target.

### Basic Reset Glitch

Reset glitching can be achieved using GPIO control:

1. Connect the target's reset line to a GPIO pin on the badge

2. Control the reset line:
   ```
   gpio set 20 output 1  # Set GPIO20 as output high (inactive reset)
   gpio set 20 low       # Pull reset low (active)
   delay_ns 10000        # Brief reset pulse
   gpio set 20 high      # Release reset
   ```

### Reset Glitch During Boot

To target the boot process:

1. Configure timing and coordinate with boot sequence:
   ```
   gpio set 20 low       # Hold in reset
   sleep_ms 100          # Wait
   gpio set 20 high      # Release reset
   sleep_ms 50           # Wait for partial boot
   gpio set 20 low       # Glitch reset
   delay_ns 5000         # Brief glitch
   gpio set 20 high      # Release
   ```

2. Power cycle the target

3. Immediately arm and trigger the reset glitcher:
   ```
   reset_glitch arm
   reset_glitch trigger
   ```

## Using the ADC for Glitch Analysis

The badge's ADC can be used to monitor and analyze glitches in real-time.

### Monitoring Power During Glitches

1. Connect the target's VCC to an ADC input:
   ```
   ┌─────────┐           ┌─────────┐           ┌─────────┐
   │         │  Glitch   │         │           │         │
   │  Badge  ├───Output──┤  Target ├───VCC─────┤  Badge  │
   │         │           │         │           │   ADC   │
   └────┬────┘           └────┬────┘           └─────────┘
        │                     │
        └─────────GND─────────┘
   ```

2. Configure ADC streaming:
   ```
   adc_stream start channel=0 rate=1000000
   ```

3. Perform your glitch

4. Stop ADC streaming and save the data:
   ```
   adc_stream stop
   adc_stream save glitch_capture.csv
   ```

### Trigger-Based Capture

To capture only the glitch event:

1. Configure ADC trigger capture:
   ```
   adc capture channel=0 trigger=falling threshold=2.5 samples=1000 pretrigger=100
   ```

2. Arm the ADC capture:
   ```
   adc capture arm
   ```

3. Perform your glitch

The ADC will automatically capture when the voltage falls below 2.5V, including 100 samples before the trigger.

### Analyzing Glitch Waveforms

Key characteristics to look for in the captured waveforms:

1. **Voltage Drop**: How far the voltage drops during the glitch
2. **Glitch Duration**: The actual duration of the voltage drop
3. **Recovery Time**: How long it takes for the voltage to stabilize
4. **Ringing**: Oscillations after the glitch

Example analysis:
```
Glitch parameters: width=10, delay=100
Measured voltage drop: 3.3V → 1.2V
Measured duration: 120ns
Recovery time: 350ns
Result: Target reset successfully bypassed
```

## Documenting Glitch Attacks

### Standard Documentation Format

When documenting glitch attacks, include:

1. **Target Information**:
   - Device name and model
   - Firmware version
   - Operating conditions

2. **Setup Description**:
   - Connection diagram
   - Equipment used
   - Badge configuration

3. **Glitch Parameters**:
   - Type of glitch (voltage, crowbar, reset)
   - Width, delay, repeat settings
   - Trigger method

4. **Results**:
   - Success rate (e.g., 8/10 attempts successful)
   - Observed behavior
   - Captured waveforms

5. **Analysis**:
   - Why the glitch worked
   - Critical timing points
   - Potential countermeasures

### Example Documentation

```markdown
# Voltage Glitch Attack on XYZ Secure Microcontroller

## Target
- Device: XYZ Secure Microcontroller
- Firmware: v1.2.3
- Operating voltage: 3.3V
- Clock: 16MHz

## Setup
- LayerOne 2025 GLiTCh BadgE connected to target VCC
- Target running authentication routine
- Badge ADC0 monitoring target VCC
- Badge GPIO16 connected to target "processing" LED for trigger

## Glitch Parameters
- Type: Voltage glitch
- Width: 12 clock cycles
- Delay: 85 clock cycles after trigger
- Repeat: 1
- Trigger: Rising edge on GPIO16

## Results
- Success rate: 7/10 attempts
- When successful: Authentication bypassed, access granted
- Captured waveform shows voltage drop from 3.3V to 1.1V for 150ns
- Target continues operation after glitch without reset

## Analysis
- Glitch likely causes the comparison instruction to be skipped
- Critical timing is when the "processing" LED turns on
- The 85-cycle delay targets the exact instruction performing the password check
- Countermeasure: Implement redundant checks or glitch detection
```

## Advanced Techniques

### Multi-Stage Glitching

Some targets require multiple glitches to bypass security:

1. Configure the first glitch:
   ```
   glitch setup width=5 delay=100 name=glitch1
   ```

2. Configure the second glitch:
   ```
   glitch setup width=10 delay=500 name=glitch2
   ```

3. Create a sequence:
   ```
   glitch sequence add glitch1 glitch2 interval=1000
   ```

4. Arm and trigger the sequence:
   ```
   glitch sequence arm
   glitch sequence trigger
   ```

### Combining Glitch Types

For complex targets, combine different glitch types:

```
attack setup type=combined voltage_width=5 voltage_delay=100 reset_width=2 reset_delay=105
attack arm
attack trigger
```

This performs a voltage glitch followed immediately by a reset glitch.

### Fault Injection Campaigns

For systematic research:

1. Define parameter ranges:
   ```
   campaign setup param=width range=5:20:5 param=delay range=0:200:10 iterations=5
   ```

2. Define success criteria:
   ```
   campaign success uart="Access granted" or gpio=16:high
   ```

3. Run the campaign:
   ```
   campaign start
   ```

4. Analyze results:
   ```
   campaign report
   ```

## Troubleshooting

### Glitch Not Working

1. **No Effect on Target**:
   - Increase glitch width
   - Decrease power supply capacitance on target
   - Try crowbar instead of voltage glitch

2. **Target Always Resets**:
   - Decrease glitch width
   - Increase delay
   - Add capacitance to target

3. **Inconsistent Results**:
   - Improve trigger timing
   - Stabilize target operating conditions
   - Use ADC to verify glitch timing

## Conclusion

Glitching is both an art and a science. Success requires:

1. Understanding the target's operation
2. Precise timing and parameter tuning
3. Careful analysis of results
4. Systematic documentation

The LayerOne 2025 GLiTCh BadgE provides all the tools needed for effective glitching research. By combining different techniques and carefully analyzing the results, you can uncover vulnerabilities and better understand hardware security.