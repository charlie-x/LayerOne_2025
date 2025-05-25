# LayerOne 2025 GLiTCh BadgE - AVRISP Programming Guide

## Introduction

The LayerOne 2025 GLiTCh BadgE includes a powerful AVRISP (AVR In-System Programming) capability that allows you to program AVR microcontrollers directly from the badge. This guide explains how to use this feature for programming, reading, and verifying AVR chips.

## What is AVRISP?

AVRISP (AVR In-System Programming) is a protocol developed by Atmel (now Microchip) for programming AVR microcontrollers while they are installed in a circuit. The badge implements this protocol, allowing it to function as an AVRISP programmer compatible with tools like AVRDUDE.

## Supported AVR Chips

The badge has built-in support for several common AVR microcontrollers:

| Chip | Flash Size | Page Size | Signature Bytes |
|------|------------|-----------|-----------------|
| ATtiny13A | 1KB | 32 bytes | 0x1E, 0x90, 0x07 |
| ATtiny85 | 8KB | 64 bytes | 0x1E, 0x93, 0x0B |
| ATmega328P | 32KB | 128 bytes | 0x1E, 0x95, 0x0F |
| ATmega328PB | 32KB | 128 bytes | 0x1E, 0x95, 0x16 |
| ATmega644PA | 64KB | 256 bytes | 0x1E, 0x96, 0x0A |
| ATmega2560 | 256KB | 256 bytes | 0x1E, 0x98, 0x01 |

## Hardware Setup

### Connecting to an AVR Chip

To program an AVR microcontroller, you need to connect the badge to the target chip using the following pins:

1. **MOSI** (Master Out Slave In): Connect to the target's MOSI pin
2. **MISO** (Master In Slave Out): Connect to the target's MISO pin
3. **SCK** (Serial Clock): Connect to the target's SCK pin
4. **RESET**: Connect to the target's RESET pin
5. **GND** (Ground): Connect to the target's GND
6. **VCC** (Optional): Connect to the target's VCC if you want to power the target from the badge

### Connection Diagram

```
┌─────────┐           ┌─────────┐
│         │  MOSI     │         │
│  Badge  ├───────────┤   AVR   │
│         │           │         │
│         │  MISO     │         │
│         ├───────────┤         │
│         │           │         │
│         │  SCK      │         │
│         ├───────────┤         │
│         │           │         │
│         │  RESET    │         │
│         ├───────────┤         │
│         │           │         │
│         │  GND      │         │
│         ├───────────┤         │
│         │           │         │
└─────────┘           └─────────┘
```

### SPI Clock Speed

The badge supports two SPI clock speeds for programming:
- **Slow Clock**: 100 kHz (for slower AVR chips or longer wires)
- **Fast Clock**: 1 MHz (for faster programming with most chips)

## Using the CLI Interface

The badge's CLI provides several commands for AVR programming operations:

### Basic AVRISP Commands

#### Initializing the AVRISP Interface

```
avrisp init
```

This command initializes the AVRISP interface with default settings.

#### Identifying the Connected Chip

```
avrisp identify
```

This command reads the signature bytes from the connected AVR chip and attempts to identify it based on the signature.

Example output:
```
Reading signature...
Signature: 0x1E 0x95 0x0F
Identified chip: ATmega328P
Flash size: 32KB
Page size: 128 bytes
```

### Programming Commands

#### Reading Flash Memory

```
avrisp read flash [filename]
```

This command reads the entire flash memory from the AVR chip and optionally saves it to a file.

#### Programming Flash Memory

```
avrisp program [filename]
```

This command programs the AVR chip with the contents of the specified HEX file.

Example:
```
avrisp program blink.hex
```

#### Verifying Flash Memory

```
avrisp verify [filename]
```

This command verifies that the contents of the AVR chip's flash memory match the specified HEX file.

### Fuse Operations

AVR chips use fuse bytes to configure various hardware settings. The badge provides commands to read and write these fuses.

#### Reading Fuses

```
avrisp read fuses
```

This command reads all fuse bytes (low, high, extended, and lock bits) from the AVR chip.

Example output:
```
Low fuse: 0xFF
High fuse: 0xD9
Extended fuse: 0xFF
Lock bits: 0xFF
```

#### Writing Fuses

```
avrisp write lfuse [value]
avrisp write hfuse [value]
avrisp write efuse [value]
avrisp write lock [value]
```

These commands write values to the specified fuse bytes.

Example:
```
avrisp write lfuse 0xFF
avrisp write hfuse 0xD9
avrisp write efuse 0xFF
```

### Advanced Commands

#### Erasing the Chip

```
avrisp erase
```

This command performs a chip erase, clearing all flash memory, EEPROM, and (in some cases) fuse bytes.

#### Reading EEPROM

```
avrisp read eeprom [filename]
```

This command reads the EEPROM memory from the AVR chip and optionally saves it to a file.

#### Writing EEPROM

```
avrisp write eeprom [filename]
```

This command writes data to the EEPROM memory of the AVR chip from the specified file.

## Using with AVRDUDE

The badge can function as an AVRISP programmer compatible with AVRDUDE, a popular command-line tool for programming AVR microcontrollers.

### Setting Up AVRDUDE

1. Connect the badge to your computer
2. Switch to AVRISP mode:
   ```
   usb mode avrisp
   ```
3. Note the serial port assigned to the badge (e.g., /dev/ttyACM0, COM3)

### AVRDUDE Commands

#### Reading the Chip Signature

```
avrdude -c avrisp -P [port] -p m328p
```

Replace `[port]` with your serial port and `m328p` with your target chip.

#### Programming a HEX File

```
avrdude -c avrisp -P [port] -p m328p -U flash:w:blink.hex:i
```

#### Reading Fuses

```
avrdude -c avrisp -P [port] -p m328p -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h
```

#### Writing Fuses

```
avrdude -c avrisp -P [port] -p m328p -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m
```

## Programming Examples

### Example 1: Programming an Arduino Bootloader

To program an ATmega328P with the Arduino bootloader:

1. Connect the badge to the ATmega328P
2. Initialize the AVRISP interface:
   ```
   avrisp init
   ```
3. Identify the chip:
   ```
   avrisp identify
   ```
4. Erase the chip:
   ```
   avrisp erase
   ```
5. Program the bootloader:
   ```
   avrisp program arduino_bootloader.hex
   ```
6. Set the fuses for the Arduino:
   ```
   avrisp write lfuse 0xFF
   avrisp write hfuse 0xD6
   avrisp write efuse 0xFD
   ```
7. Verify the programming:
   ```
   avrisp verify arduino_bootloader.hex
   ```

### Example 2: Reading and Backing Up an Existing Chip

To read and back up the contents of an existing AVR chip:

1. Connect the badge to the AVR chip
2. Initialize the AVRISP interface:
   ```
   avrisp init
   ```
3. Identify the chip:
   ```
   avrisp identify
   ```
4. Read and save the flash memory:
   ```
   avrisp read flash backup.hex
   ```
5. Read and save the fuses:
   ```
   avrisp read fuses
   ```
   (Note the values for future reference)
6. Read and save the EEPROM (if needed):
   ```
   avrisp read eeprom backup.eep
   ```

## Troubleshooting

### Common Issues

#### Cannot Identify Chip

If the badge cannot identify the chip:

1. Check your connections, especially RESET, MOSI, MISO, and SCK
2. Ensure the chip is powered (VCC and GND)
3. Try a slower clock speed:
   ```
   avrisp init slow
   ```
4. Check if the chip is in a deep sleep mode or has fuses set that disable programming

#### Programming Fails

If programming fails:

1. Verify the chip is properly connected
2. Ensure the chip is not write-protected (check lock bits)
3. Try erasing the chip first:
   ```
   avrisp erase
   ```
4. Verify the HEX file is valid and appropriate for your chip
5. Check if the chip has enough memory for your program

#### Fuse Programming Warnings

Be careful when programming fuses! Incorrect fuse settings can make the chip unprogrammable through normal means. Common dangerous settings include:

- Disabling the RESET pin
- Setting incorrect clock sources
- Enabling brown-out detection at too high a voltage

If you're unsure about fuse values, consult the datasheet for your specific AVR chip or use an online fuse calculator.

## Advanced Usage

### High-Voltage Programming

Some AVR chips that have been "bricked" by incorrect fuse settings may require high-voltage programming to recover. The badge does not support high-voltage programming directly, but you can use it in conjunction with a high-voltage adapter.

### Clock Considerations

When programming chips with non-standard clock configurations:

1. If the chip uses an external crystal, ensure it's connected
2. If the chip uses an internal RC oscillator, be aware of its frequency
3. For chips running at very low clock speeds, use the slow programming clock

## Conclusion

The AVRISP functionality of the LayerOne 2025 GLiTCh BadgE provides a powerful and flexible way to program AVR microcontrollers. Whether you're programming a new chip, recovering a bricked device, or just experimenting with AVR development, the badge offers all the tools you need for successful AVR programming.