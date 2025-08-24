# LayerOne 2025 GLiTCh BadgE

later on there are ai generated docs...

altium link.. there is an eror on one of the leds on the charlieplex'd leds...
also the 3 pin JST SWD is reversed
.
https://365.altium.com/files/01BF9671-6FB8-4A9E-9FD1-49A52BA8D165

first flash drive appears as RPI-RP2 copy the uf2 file to it, after that use the system command to reboot to the dfu or hold the RP-BOOT button during power on or a reset cycle

the pico-ice can be used to test the  fpga.

https://a360.co/4dxIxcs
terminal is CR, so do cr/lf conversion.

for 3v3 control the pads need to be soldered on the pcb,



switch_pwr  
short 1-2
![image](https://github.com/user-attachments/assets/cf1b73e3-9c54-4e85-a0a2-8153f8ca2d10)

glitch_ctrl
solder all three
short 1-2-3
![image](https://github.com/user-attachments/assets/7def6045-e0bb-45a3-b9a5-88939d821888)

3cv3_ctrl
short 1-2-3
![image](https://github.com/user-attachments/assets/88a24b83-067c-499f-8c80-73a1713b94d6)


fet
short 1-2-3
![image](https://github.com/user-attachments/assets/4419e85d-f257-4988-bc79-ecb437aeeb05)


https://a360.co/4dxIxcs


thea re two pots vr1, vr2 , vr1 changes run votlage of vtg, vr2 controls the low voltage

diag scripts will find


Serial Ports for CMSIS-DAP
==========================

Found serial ports:
crw-rw-rw-  1 root  wheel  0x9000003 May 25 08:40 /dev/cu.usbmodem1202

first serial port is the CDC
crw-rw-rw-  1 root  wheel  0x9000005 May 25 08:40 /dev/cu.usbmodem14301

is for hook up to the fpga etc
crw-rw-rw-  1 root  wheel  0x9000009 May 25 08:40 /dev/cu.usbmodem14303

adc streaming , two adc's interlevaed see the adc python script

crw-rw-rw-  1 root  wheel  0x9000007 May 25 08:40 /dev/cu.usbmodem14305


the adc streaming is seperate and always runs

// run it
python adc_stream_reader.py /dev/cu.usbmodem14305

// pack ADC values into buffer
 - #define HEADER_BYTE 0xAA
 - #define PACKET_SIZE 5
 - 
 - void pack_adc_values(uint16_t _adc0, uint16_t _adc1, uint8_t seq, uint8_t* buffer) {
 -    buffer[0] = HEADER_BYTE;                      // header byte for synchronization
 -    buffer[1] = seq;                              // sequence number
 -     buffer[2] = _adc0 & 0xFF;                      // low 8 bits of adc0
 -     buffer[3] = ((_adc0 >> 8) & 0x0F) |            // high 4 bits of adc0
 -                 ((_adc1 & 0x0F) << 4);             // low 4 bits of adc1
 -     buffer[4] = (_adc1 >> 4) & 0xFF;               // high 8 bits of adc1
 - }


<img width="1203" alt="image" src="https://github.com/user-attachments/assets/87d81ba6-0b7e-4d30-98ad-a1d6741d11d0" />




tats for LayerOne2025 codebase:

  Files by type:
  - C (.c): 57 files
  - Headers (.h): 48 files
  - Python (.py): 25 files
  - Markdown (.md): 17 files
  - Shell scripts (.sh): 8 files

  Code metrics:
  - Total lines of code: ~30,383 lines
  - Total commits: 82
  - Contributors: 2 (charliex: 82)

  Top directories:
  - Root directory (372 files)
  - cli/ (42 files)
  - docs/ (12 files)



## Overview

The LayerOne 2025 GLiTCh BadgE is a versatile hardware hacking platform designed for security researchers, hardware enthusiasts, and anyone interested in exploring digital electronics. It combines a powerful RP2040 microcontroller with specialized hardware for glitching, debugging, and interfacing with other devices.

## Features

- **RP2040 Microcontroller** - Dual-core ARM Cortex-M0+ processor
- **ICE40 FPGA** - Programmable logic for custom hardware implementations
- **Voltage Glitcher** - For performing precise voltage glitching attacks
- **Crowbar Circuit** - For power line manipulation
- **SWD Interface** - For debugging ARM-based targets
- **AVRISP** - For programming AVR microcontrollers
- **Analog Monitoring** - For capturing and analyzing signals
- **Multiple USB Modes** - Normal, DAP, and DFU modes for different use cases

## Documentation

Comprehensive documentation is available in the [docs](docs/) directory:

- [Quick Start Guide](docs/quick_start.md) - Get up and running with your badge
- [CLI User Guide](docs/cli_user_guide.md) - Learn how to use the Command Line Interface
- [USB Modes Guide](docs/usb_modes_guide.md) - Understand the different USB modes
- [Hardware Hacking Guide](docs/hardware_hacking_guide.md) - Explore the hardware hacking features
- [Glitching Techniques Guide](docs/glitching_techniques.md) - Detailed guide on voltage, reset, and crowbar glitching
- [ADC Signal Analysis Guide](docs/adc_signal_analysis.md) - How to use the ADC for signal capture and analysis
- [AVRISP Programming Guide](docs/avrisp_programming_guide.md) - Program AVR microcontrollers with the badge
- [Documenting Research](docs/documenting_research.md) - Best practices for documenting hardware security research
- [Command Reference](docs/command_reference.md) - Quick reference for all CLI commands
- [Case Studies](docs/case_studies/README.md) - Real-world examples of hardware security vulnerabilities
  - [nRF52 Glitch Attack](docs/case_studies/nrf52_glitch_attack.md) - Voltage glitching attack on Nordic's BLE SoC

## Getting Started

### Prerequisites

- Python 3.6 or higher
- PySerial library (`pip install pyserial`)
- PyOCD for debugging (`pip install pyocd`)

### Installation

1. Clone this repository:
   ```
   git clone git@github.com:charlie-x/LayerOne_2025.git
   cd LayerOne_2025
   ```

2. Connect your badge to your computer using a USB-C cable
   Either use the RP-BOOT switch to put the badge into RPI-RP2 mode while its in reset, or switch on. or if its already flashed you can use system dfu command on the badge itself, copy the uf2/layerOne2025.uf2 file to the RPI-RP2 folder, use -X if you use cp in Mac OS
  
4. Run the CLI terminal:
   ```
   python3 cli_terminal.py
   ```

### Basic Usage

Once connected to the CLI, you can use commands like:

```
help            # Show available commands
status          # Show badge status
rgb on          # Turn on the RGB LED
usb mode dap    # Switch to DAP mode
```

For a complete list of commands, see the [Command Reference](docs/command_reference.md).

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

## Logic Analyser mode

use command 

adc_stream la


<img width="1570" height="1306" alt="image" src="https://github.com/user-attachments/assets/8252e059-2852-4418-89c5-e1b98bc19e27" />

## Development

### Building the Firmware

To build the firmware:

```
./build.sh
```

### Flashing the Firmware

To build and flash the firmware:

```
./build_and_flash.sh
```

### Running Tests

To run the USB mode tests:

```
python3 test_usb_modes.py
```

## Contributing

We welcome contributions to improve the badge and its documentation! Please see the [Contributing Guide](docs/contributing.md) for details on how to contribute.

## License

The LayerOne 2025 GLiTCh BadgE firmware and hardware designs are licensed under [MIT License](https://opensource.org/licenses/MIT).

## Acknowledgments

The LayerOne 2025 GLiTCh BadgE was created by NullSpaceLabs for the LayerOne 2025 conference. Special thanks to all the contributors and the hardware hacking community for their support and inspiration.

## Contact

- Website: [nullspacelabs.com](https://nullspacelabs.com)
- Email: [info@nullspacelabs.com](mailto:info@nullspacelabs.com)
- Twitter: [@nullspacelabs](https://twitter.com/nullspacelabs)
