# LayerOne 2025 GLiTCh BadgE


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
   git clone https://github.com/nullspacelabs/layerOne2025.git
   cd layerOne2025
   ```

2. Connect your badge to your computer using a USB-C cable

3. Run the CLI terminal:
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