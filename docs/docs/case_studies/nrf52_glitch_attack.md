# Case Study: nRF52 Glitch Attack

## Introduction

The nRF52 series of System-on-Chips (SoCs) from Nordic Semiconductor are widely used in IoT devices, wearables, and other Bluetooth Low Energy (BLE) applications. In 2019-2020, researchers discovered that these chips were vulnerable to voltage glitching attacks that could bypass their security features, including the readback protection that was designed to prevent extraction of firmware and sensitive data.

This case study examines this vulnerability and how it can be reproduced using the LayerOne 2025 GLiTCh BadgE.

## Background

### The nRF52 Series

The nRF52 series includes several variants (nRF52832, nRF52840, etc.) based on the ARM Cortex-M4F processor. These SoCs feature:

- 32-bit ARM Cortex-M4F CPU (64 MHz)
- Bluetooth 5.0 support
- 256 KB to 1 MB Flash memory
- 32 KB to 256 KB RAM
- Various security features including:
  - Readback protection
  - Debug access port protection
  - Memory protection units

### Security Features

The nRF52 implements several security features, with one of the most important being the APPROTECT (Access Port Protection) mechanism. When enabled, APPROTECT is supposed to prevent:

1. Reading the chip's flash memory through the debug interface
2. Debugging the processor
3. Accessing internal registers and memory

This protection is crucial for devices that store sensitive information like encryption keys, proprietary algorithms, or personal data.

## The Vulnerability

### Discovery

In 2019, researchers from Ledger (a cryptocurrency hardware wallet company) discovered that the nRF52 was vulnerable to voltage glitching attacks. The vulnerability was later independently verified by other researchers and security firms.

### Technical Details

The vulnerability exists in the hardware implementation of the APPROTECT feature. During the boot process, the nRF52 checks if APPROTECT is enabled by reading a specific register. If a precisely timed voltage glitch is applied during this check, the processor can misinterpret the value, effectively bypassing the protection.

Key aspects of the vulnerability:

1. The vulnerability is in hardware, not software
2. It affects all nRF52 series chips manufactured before the discovery
3. It cannot be fixed with a firmware update
4. It requires physical access to the device

### Impact

This vulnerability had significant implications:

1. **Firmware Extraction**: Attackers could extract proprietary firmware
2. **Key Extraction**: Cryptographic keys and secrets could be accessed
3. **IP Theft**: Proprietary algorithms and code could be stolen
4. **Device Cloning**: Devices could be cloned with extracted firmware
5. **Security Bypass**: Security features like secure boot could be circumvented

## Reproducing the Attack with the GLiTCh BadgE

The LayerOne 2025 GLiTCh BadgE is well-suited for reproducing this attack for educational and security research purposes.

### Required Equipment

- LayerOne 2025 GLiTCh BadgE
- nRF52 development board or target device
- Jumper wires
- (Optional) Logic analyzer or oscilloscope

### Physical Setup

1. Connect the badge to the nRF52 target:
   - Badge GND → nRF52 GND
   - Badge Glitch Out → nRF52 VDD
   - Badge SWD Clock → nRF52 SWDCLK
   - Badge SWD Data → nRF52 SWDIO
   - Badge ADC0 → nRF52 VDD (for monitoring)

2. Setup diagram:
   ```
   ┌─────────┐           ┌─────────┐
   │         │  Glitch   │         │
   │  Badge  ├───Output──┤  nRF52  │
   │         │           │         │
   │         │  SWDCLK   │         │
   │         ├───────────┤         │
   │         │           │         │
   │         │  SWDIO    │         │
   │         ├───────────┤         │
   │         │           │         │
   │         │  ADC0     │         │
   │         ├───────────┤         │
   │         │           │         │
   └────┬────┘           └────┬────┘
        │                     │
        └─────────GND─────────┘
   ```

### Attack Procedure

#### Step 1: Initial Reconnaissance

1. Power up the nRF52 without glitching
2. Attempt to connect via SWD:
   ```
   swd scan
   ```
3. Verify that APPROTECT is enabled (connection should fail)

#### Step 2: Configure the Glitcher

1. Set up the voltage glitcher:
   ```
   glitch setup width=5 delay=0 repeat=1
   ```

2. Configure ADC monitoring:
   ```
   adc_stream start channel=0 rate=1000000
   ```

#### Step 3: Glitch During Reset

1. Set up a script to automate the process:
   ```
   # Example script
   for delay in range(0, 200, 5):
       # Configure glitch
       send_command("glitch setup width=5 delay=" + str(delay) + " repeat=1")
       
       # Reset target
       toggle_reset()
       
       # Apply glitch
       send_command("glitch trigger")
       
       # Try to connect via SWD
       if swd_connect():
           print("Success at delay=" + str(delay))
           break
   ```

2. Run the script to find the vulnerable timing window

#### Step 4: Exploit the Vulnerability

Once the correct timing is found:

1. Apply the glitch during reset
2. Connect via SWD:
   ```
   swd connect
   ```
3. Dump the flash memory:
   ```
   swd dump 0x00000000 0x80000 firmware.bin
   ```

### Parameter Tuning

The attack requires precise timing. Typical parameters:

- **Width**: 3-10 clock cycles
- **Delay**: Varies by device, typically 50-150 clock cycles after reset
- **Voltage Drop**: From 3.3V to approximately 1.8-2.2V
- **Glitch Type**: Voltage glitch (not clock glitch)

## Analysis of the Attack

### Why It Works

The vulnerability exists because:

1. The APPROTECT check happens early in the boot process
2. The check is performed only once
3. The hardware lacks redundancy or verification for this critical security check
4. The CPU can misinterpret register values when its voltage is disturbed

### Success Rate

With properly tuned parameters, success rates of 30-70% are typical. Factors affecting success:

1. Precise timing of the glitch
2. Magnitude of the voltage drop
3. Duration of the glitch
4. Quality of the power supply
5. Specific nRF52 variant and revision

## Countermeasures

Nordic Semiconductor addressed this vulnerability in several ways:

1. **Hardware Revision**: Newer revisions of the nRF52 chips include hardware fixes
2. **Identification**: Nordic published information to identify vulnerable vs. fixed chips
3. **Recommendations**: Additional security measures were recommended for sensitive applications

For developers using potentially vulnerable nRF52 chips:

1. **Physical Security**: Make physical access to the device difficult
2. **Encrypted Storage**: Store sensitive data in encrypted form
3. **Runtime Integrity Checks**: Implement software integrity verification
4. **Layered Security**: Don't rely solely on APPROTECT

## Lessons Learned

The nRF52 glitch attack teaches several important lessons:

1. **Hardware Security is Critical**: Software security measures can be bypassed by hardware attacks
2. **Single Points of Failure**: Security should not depend on a single check
3. **Redundancy Matters**: Critical security checks should be redundant and diverse
4. **Physical Access Risks**: Physical access to devices enables powerful attack vectors
5. **Testing Limitations**: Standard security testing may not catch glitch vulnerabilities

## Conclusion

The nRF52 glitch attack represents a classic example of how hardware vulnerabilities can undermine otherwise well-designed security systems. It demonstrates the importance of considering physical attacks in security threat models, especially for devices that may be physically accessible to attackers.

The LayerOne 2025 GLiTCh BadgE provides an excellent platform for studying this and similar vulnerabilities, allowing security researchers and developers to better understand and defend against such attacks.

## References

1. Ledger Donjon, "Unfixable Seed Extraction on Trezor - A practical and reliable attack"
2. Colin O'Flynn, "Glitching the nRF52 (Nordic's BLE Chip)"
3. Nordic Semiconductor, "nRF52 Series PSA: Readback protection bypass"
4. Riscure, "Voltage glitching to bypass hardware security"
5. Ken Munro, "Breaking BLE: Extracting secrets from nRF52 chips"