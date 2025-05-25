# LayerOne 2025 GLiTCh BadgE Documentation

Welcome to the official documentation for the LayerOne 2025 GLiTCh BadgE! This documentation will help you get the most out of your badge and explore its many features.

## About the Badge

The LayerOne 2025 GLiTCh BadgE is a versatile hardware hacking platform designed for security researchers, hardware enthusiasts, and anyone interested in exploring digital electronics. It combines a powerful RP2040 microcontroller with specialized hardware for glitching, debugging, and interfacing with other devices.

![GLiTCh BadgE 2025](https://nullspacelabs.com/images/glitch_badge_2025.jpg)

## Documentation Guides

### Getting Started

- [Quick Start Guide](quick_start.md) - Get up and running with your badge
- [CLI User Guide](cli_user_guide.md) - Learn how to use the Command Line Interface
- [USB Modes Guide](usb_modes_guide.md) - Understand the different USB modes and how to use them

### Features and Capabilities

- [Hardware Hacking Guide](hardware_hacking_guide.md) - Explore the hardware hacking features
- [Glitching Techniques Guide](glitching_techniques.md) - Detailed guide on voltage, reset, and crowbar glitching
- [ADC Signal Analysis Guide](adc_signal_analysis.md) - How to use the ADC for signal capture and analysis
- [AVRISP Programming Guide](avrisp_programming_guide.md) - Program AVR microcontrollers with the badge
- [Documenting Research](documenting_research.md) - Best practices for documenting hardware security research

### Development


### Case Studies

- [Hardware Security Case Studies](case_studies/README.md) - Real-world examples of hardware security vulnerabilities
  - [nRF52 Glitch Attack](case_studies/nrf52_glitch_attack.md) - Voltage glitching attack on Nordic's BLE SoC

## Command Reference

For a quick reference of all available CLI commands, see the [Command Reference](command_reference.md).

## USB Modes

The badge supports three USB modes:

1. **Normal Mode** - 3 CDC interfaces + Mass Storage
2. **DAP Mode** - CMSIS-DAP debugging interface + CDC
3. **DFU Mode** - Device Firmware Update mode

Switch between modes using:
```
usb mode normal    # Normal mode (default)
usb mode dap       # CMSIS-DAP mode
usb mode dfu       # Firmware update mode
```

For details on how to use these modes, see the [USB Modes Guide](usb_modes_guide.md).

## Hardware Hacking Features

The badge includes several specialised hardware features:

- **Voltage Glitcher** - Precise voltage glitching attacks
- **Crowbar Circuit** - Power line manipulation
- **SWD Interface** - ARM Cortex-M debugging
- **AVRISP/AVRISP2** - AVR microcontroller programming
- **Analog Monitoring** - ADC for signal capture and analysis
- **ICE40 FPGA** - Programmable logic for custom implementations
- **GPIO Control** - Flexible pin control and interfacing
- **Script System** - Automated command execution

For details on how to use these features, see the [Hardware Hacking Guide](hardware_hacking_guide.md).

## Community Resources

- [Official Website](https://nullspacelabs.com)
- [GitHub Repository](https://github.com/nullspacelabs/layerOne2025)
- [Community Forum](https://forum.nullspacelabs.com)
- [Discord Channel](https://discord.gg/nullspacelabs)

## Contributing

We welcome contributions to improve the badge and its documentation! Please see the [Contributing Guide](contributing.md) for details on how to contribute.

## License

The LayerOne 2025 GLiTCh BadgE documentation is licensed under [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/).

The firmware and hardware designs are licensed under [MIT License](https://opensource.org/licenses/MIT).

## Acknowledgments

The LayerOne 2025 GLiTCh BadgE was created by NullSpaceLabs for the LayerOne 2025 conference. Special thanks to all the contributors and the hardware hacking community for their support and inspiration.