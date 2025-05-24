# LayerOne 2025 GLiTCh BadgE - ADC Signal Analysis Guide

## Introduction

The LayerOne 2025 GLiTCh BadgE includes powerful Analog-to-Digital Converter (ADC) capabilities that allow you to capture, analyze, and visualize analog signals. This guide explains how to use the badge's ADC features for signal analysis, particularly in the context of hardware security research and glitching attacks.

## ADC Hardware Specifications

The badge features multiple ADC channels with the following specifications:

- **Resolution**: 12-bit (0-4095 range)
- **Input Voltage Range**: 0-3.3V
- **Maximum Sample Rate**: 1 MSPS (1 million samples per second)
- **Number of Channels**: 4 independent channels
- **Buffer Size**: 32KB per channel

## Basic ADC Usage

### Reading ADC Values

To read the current value from an ADC channel:

```
adc read [channel]
```

Examples:
```
adc read        # Read all ADC channels
adc read 0      # Read ADC channel 0
adc read 1      # Read ADC channel 1
```

This will display the current voltage level:
```
ADC Channel 0: 2.543V (3112/4095)
```

### Continuous Monitoring

For real-time monitoring of a signal:

```
adc monitor [channel] [interval]
```

Examples:
```
adc monitor 0 100     # Monitor channel 0, update every 100ms
adc monitor 1 500     # Monitor channel 1, update every 500ms
```

Press Ctrl+C to stop monitoring.

## ADC Streaming

For capturing high-speed signals, use the ADC streaming feature:

### Starting ADC Stream

```
adc_stream start [options]
```

Options:
- `channel=X`: Channel number (0-3)
- `rate=X`: Sample rate in Hz (up to 1000000)
- `buffer=X`: Buffer size in samples
- `continuous=true/false`: Continuous streaming mode

Examples:
```
adc_stream start channel=0 rate=1000000     # Stream channel 0 at 1MSPS
adc_stream start channel=1 rate=500000 buffer=16384     # Stream with custom buffer
adc_stream start channel=0,1 rate=250000     # Stream multiple channels
```

### Stopping ADC Stream

```
adc_stream stop
```

### Saving Captured Data

```
adc_stream save [filename]
```

Example:
```
adc_stream save capture.csv     # Save to CSV format
adc_stream save capture.bin     # Save to binary format
```

## Triggered Capture

For capturing specific events, use the triggered capture mode:

### Configuring Trigger

```
adc capture [options]
```

Options:
- `channel=X`: Channel number (0-3)
- `trigger=rising/falling/level`: Trigger type
- `threshold=X`: Trigger threshold voltage
- `samples=X`: Number of samples to capture
- `pretrigger=X`: Number of samples to include before trigger
- `rate=X`: Sample rate in Hz

Examples:
```
adc capture channel=0 trigger=rising threshold=2.5 samples=1000 pretrigger=100
```

This will capture 1000 samples when the signal on channel 0 crosses 2.5V with a rising edge, including 100 samples before the trigger.

### Arming the Trigger

```
adc capture arm
```

The badge will wait for the trigger condition to be met, then capture the specified number of samples.

### Checking Capture Status

```
adc capture status
```

### Saving Captured Data

```
adc capture save [filename]
```

## Signal Analysis

The badge provides built-in analysis tools for captured signals:

### Basic Statistics

```
adc analyze stats [channel]
```

This displays:
- Minimum voltage
- Maximum voltage
- Average voltage
- RMS voltage
- Standard deviation

### Frequency Analysis

```
adc analyze fft [channel]
```

This performs a Fast Fourier Transform and displays:
- Dominant frequencies
- Frequency spectrum
- Power spectral density

### Pulse Analysis

```
adc analyze pulse [channel] [threshold]
```

This analyzes digital pulses in the signal:
- Pulse count
- Pulse widths
- Duty cycle
- Rise/fall times

## Practical Applications

### Power Analysis

Monitoring power consumption can reveal information about device operation:

1. Connect the badge ADC to the target's power line through a small shunt resistor:
   ```
   ┌─────────┐     ┌───────┐     ┌─────────┐
   │         │     │ Shunt │     │         │
   │  Power  ├─────┤ 10Ω   ├─────┤  Target │
   │ Supply  │     └───┬───┘     │         │
   └─────────┘         │         └─────────┘
                       │
                       │
                  ┌────┴────┐
                  │  Badge  │
                  │   ADC   │
                  └─────────┘
   ```

2. Capture power trace during operation:
   ```
   adc capture channel=0 trigger=rising threshold=0.1 samples=10000 pretrigger=1000
   adc capture arm
   ```

3. Analyze the power trace to identify operations:
   ```
   adc analyze stats
   adc analyze fft
   ```

### Glitch Detection

Detect and analyze glitches in a system:

1. Connect the badge ADC to the target's voltage rail:
   ```
   ┌─────────┐           ┌─────────┐
   │         │           │         │
   │  Target ├───VCC─────┤  Badge  │
   │         │           │   ADC   │
   └─────────┘           └─────────┘
   ```

2. Configure high-speed capture:
   ```
   adc_stream start channel=0 rate=1000000
   ```

3. Induce a glitch (using another device or the badge's glitcher)

4. Stop streaming and analyze:
   ```
   adc_stream stop
   adc_stream save glitch.csv
   adc analyze pulse 0 2.5     # Analyze with 2.5V threshold
   ```

### Clock Analysis

Analyze clock signals for timing attacks:

1. Connect the badge ADC to the target's clock line:
   ```
   ┌─────────┐           ┌─────────┐
   │         │           │         │
   │  Target ├───CLK─────┤  Badge  │
   │         │           │   ADC   │
   └─────────┘           └─────────┘
   ```

2. Capture the clock signal:
   ```
   adc_stream start channel=0 rate=1000000
   ```

3. Analyze for jitter or irregularities:
   ```
   adc analyze pulse 0 1.65     # Analyze with mid-point threshold
   ```

## Advanced ADC Techniques

### Synchronized Capture

Synchronize ADC capture with other badge operations:

```
sync setup source=glitch target=adc_capture delay=0
sync arm
glitch trigger     # This will automatically trigger ADC capture
```

### Multi-Channel Correlation

Capture and correlate multiple signals:

```
adc_stream start channel=0,1,2 rate=500000
```

Then analyze the correlation:

```
adc analyze correlation 0 1     # Correlate channels 0 and 1
```

### Long-Term Monitoring

For monitoring signals over extended periods:

```
adc_stream start channel=0 rate=1000 continuous=true file=log.csv
```

This will continuously stream ADC data to a file until stopped.

## Visualizing ADC Data

The badge provides several ways to visualize captured data:

### Text-Based Visualization

```
adc visualize text [channel]
```

Example output:
```
2.5V ┌─────────────────────────────────────────┐
     │                ▄▄▄▄▄                    │
     │               █     █                   │
     │              █       █                  │
     │             █         █                 │
0.0V └─────────────────────────────────────────┘
      0ms                                    10ms
```

### CSV Export for External Tools

```
adc_stream save data.csv
```

You can then import this CSV into tools like:
- Excel/Google Sheets
- MATLAB/Octave
- Python (with matplotlib, numpy)
- Sigrok/PulseView

### Real-Time Plotting

For supported terminals:

```
adc plot [channel] [duration]
```

Example:
```
adc plot 0 5000     # Plot channel 0 for 5 seconds
```

## Practical Examples

### Example 1: Capturing a Voltage Glitch

1. Connect ADC0 to the target's VCC
2. Set up the glitcher:
   ```
   glitch setup width=10 delay=100
   ```
3. Configure ADC capture:
   ```
   adc capture channel=0 trigger=falling threshold=3.0 samples=1000 pretrigger=200 rate=1000000
   ```
4. Arm both systems:
   ```
   adc capture arm
   glitch arm
   ```
5. Trigger the glitch:
   ```
   glitch trigger
   ```
6. Save and analyze the capture:
   ```
   adc capture save glitch_capture.csv
   adc analyze stats
   ```

### Example 2: Power Analysis Attack

1. Connect ADC0 to the target's power line through a shunt resistor
2. Start continuous monitoring:
   ```
   adc_stream start channel=0 rate=500000
   ```
3. Trigger the cryptographic operation on the target
4. Stop streaming and save:
   ```
   adc_stream stop
   adc_stream save crypto_power_trace.csv
   ```
5. Analyze for patterns:
   ```
   adc analyze fft
   ```

### Example 3: Clock Manipulation Detection

1. Connect ADC0 to the target's clock line
2. Connect ADC1 to a trigger signal that indicates when operations start
3. Configure triggered capture:
   ```
   adc capture channel=0,1 trigger=rising trigger_channel=1 threshold=2.5 samples=10000 pretrigger=1000 rate=1000000
   ```
4. Arm the capture:
   ```
   adc capture arm
   ```
5. Start the operation on the target
6. Analyze the clock signal:
   ```
   adc analyze pulse 0 1.65
   ```

## Troubleshooting

### Signal Too Weak

If the signal is too weak:
1. Use an amplifier circuit
2. Adjust the reference voltage:
   ```
   adc setup vref=internal     # Use internal reference
   adc setup vref=external     # Use external reference
   ```

### Noisy Signal

If the signal is noisy:
1. Improve grounding
2. Enable hardware filtering:
   ```
   adc setup filter=enable
   ```
3. Use software filtering:
   ```
   adc analyze filter lowpass cutoff=10000
   ```

### Missed Triggers

If triggers are being missed:
1. Adjust the threshold
2. Use hysteresis:
   ```
   adc capture channel=0 trigger=rising threshold=2.5 hysteresis=0.2
   ```
3. Try a different trigger type

## Conclusion

The badge's ADC capabilities make it a powerful tool for signal analysis in hardware security research. By combining these features with the badge's glitching and debugging capabilities, you can perform comprehensive analysis of hardware systems and identify potential vulnerabilities.

Remember to always document your findings thoroughly, including:
- Capture settings
- Signal characteristics
- Analysis results
- Correlations with other events

This documentation will be invaluable for reproducing results and sharing your research with the community.