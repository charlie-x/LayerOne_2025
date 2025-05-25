# LayerOne 2025 GLiTCh BadgE - Documenting Hardware Security Research

## Introduction

Proper documentation is a crucial aspect of hardware security research. This guide provides a structured approach to documenting your findings when using the LayerOne 2025 GLiTCh BadgE for hardware security research, glitching attacks, and vulnerability analysis.

## Why Documentation Matters

Good documentation serves several important purposes:

1. **Reproducibility**: Allows others (or your future self) to reproduce your findings
2. **Knowledge Transfer**: Helps share techniques and discoveries with the community
3. **Evidence**: Provides proof of vulnerabilities for responsible disclosure
4. **Learning**: Helps you understand what worked, what didn't, and why
5. **Recognition**: Properly documented research is more likely to be recognized and cited

## Documentation Structure

A comprehensive hardware security research document typically includes these sections:

### 1. Executive Summary

A brief overview (1-2 paragraphs) that summarizes:
- The target device
- The vulnerability discovered
- The impact of the vulnerability
- The attack method used

Example:
```markdown
This report documents a successful voltage glitching attack against the XYZ Secure Microcontroller. 
By applying precisely timed voltage glitches during the authentication process, we were able to 
bypass the password check with a success rate of approximately 70%. This vulnerability could allow 
an attacker with physical access to gain unauthorized access to protected functionality and data.
```

### 2. Target Description

Detailed information about the target device:

- **Device Identification**:
  - Manufacturer and model
  - Hardware version
  - Firmware version
  - Serial number (if relevant)

- **Technical Specifications**:
  - Processor/microcontroller type
  - Operating voltage and frequency
  - Memory configuration
  - Security features

- **Intended Security Level**:
  - What security claims does the manufacturer make?
  - What assets is the device protecting?
  - What threat model is it designed to withstand?

- **Photos and Diagrams**:
  - Clear photos of the device
  - Block diagrams
  - Identification of key components

### 3. Setup and Methodology

Detailed description of your test setup:

- **Equipment Used**:
  - LayerOne 2025 GLiTCh BadgE configuration
  - Additional test equipment (oscilloscopes, power supplies, etc.)
  - Custom hardware or adapters

- **Physical Setup**:
  - Connection diagrams
  - Photos of the actual setup
  - Modifications made to the target (if any)

- **Software Tools**:
  - Badge firmware version
  - Scripts or tools developed for the attack
  - Analysis software used

- **Methodology**:
  - Step-by-step description of the approach
  - Initial reconnaissance
  - Parameter selection process
  - Testing procedure

Example:
```markdown
## Setup

The target device was connected to the GLiTCh BadgE as shown in Figure 1. The badge's glitch 
output was connected to the target's VCC pin through a 10Ω current-limiting resistor. The badge's 
ADC0 was connected to the target's VCC to monitor the glitch waveform, while ADC1 was connected 
to the target's UART TX line to monitor responses.

The badge was configured in normal mode with the following connections:
- Badge Glitch Out → Target VCC
- Badge GND → Target GND
- Badge GPIO16 → Target "Processing" LED (for trigger)
- Badge UART RX → Target UART TX
- Badge UART TX → Target UART RX
- Badge ADC0 → Target VCC
- Badge ADC1 → Target UART TX

## Methodology

1. Initial reconnaissance was performed by monitoring normal operation of the authentication process
2. The timing of the password verification was determined by observing the "Processing" LED
3. A parameter sweep was conducted to identify effective glitch parameters
4. Targeted glitches were applied at the identified timing window
5. Successful bypasses were verified by capturing the target's UART output
```

### 4. Attack Description

Detailed explanation of the attack:

- **Attack Type**:
  - Voltage glitching
  - Clock glitching
  - Reset glitching
  - Crowbar attack
  - Combined approach

- **Attack Parameters**:
  - Glitch width
  - Glitch delay
  - Trigger method
  - Repetition rate
  - Voltage levels

- **Attack Process**:
  - Initialization steps
  - Trigger mechanism
  - Timing considerations
  - Success criteria

- **Parameter Tuning**:
  - How parameters were selected
  - What ranges were tested
  - How optimal values were determined

Example:
```markdown
## Voltage Glitching Attack

The attack targeted the password verification routine in the authentication process. Based on 
initial analysis, the verification occurs approximately 120ms after the "Enter Password:" prompt 
is displayed, and the "Processing" LED activates during this time.

### Glitch Parameters
- Type: Voltage glitch
- Width: 12 clock cycles (~750ns at 16MHz)
- Delay: 85 clock cycles (~5.3μs) after trigger
- Trigger: Rising edge on GPIO16 (connected to "Processing" LED)
- Voltage drop: From 3.3V to approximately 1.1V

### Attack Process
1. The badge was configured to monitor the target's UART interface
2. The "login" command was sent to the target
3. When prompted for a password, an incorrect password "test123" was sent
4. The glitcher was configured to trigger on the rising edge of the "Processing" LED
5. The glitcher was armed before sending the password
6. When the LED activated, the glitch was automatically triggered
7. The target's response was captured and analyzed for success indicators
```

### 5. Results

Clear presentation of your findings:

- **Success Rate**:
  - Number of successful attempts vs. total attempts
  - Statistical analysis if applicable

- **Observed Behavior**:
  - Target's response to the attack
  - Error messages or unexpected outputs
  - Timing measurements

- **Waveforms and Captures**:
  - ADC captures of the glitch
  - Logic analyzer traces
  - UART/communication logs

- **Parameter Sensitivity**:
  - How changes in parameters affected success rate
  - Critical parameters vs. less important ones
  - Operating windows for successful attacks

Example:
```markdown
## Results

### Success Rate
The attack was attempted 50 times with the following results:
- Successful bypasses: 35 (70%)
- Failed attempts: 12 (24%)
- Target crashes: 3 (6%)

### Observed Behavior
When successful, the target responded with "Access Granted" despite the incorrect password. 
The normal authentication process takes approximately 200ms, but successful glitch attacks 
resulted in responses within 150ms, suggesting that part of the verification routine was skipped.

### Waveform Analysis
Figure 3 shows the voltage waveform captured during a successful glitch. The voltage dropped 
from 3.3V to 1.1V for approximately 750ns, followed by a recovery period of about 350ns with 
some ringing before stabilizing.

### Parameter Sensitivity
The attack showed high sensitivity to the glitch width. Values between 10-15 clock cycles 
were effective, while values outside this range either had no effect or caused the target 
to reset. The delay parameter was less critical, with successful glitches occurring in the 
range of 80-100 clock cycles after the trigger.
```

### 6. Analysis

Interpretation of your results:

- **Root Cause Analysis**:
  - Why the attack worked
  - What security mechanism was bypassed
  - Underlying hardware/software vulnerability

- **Fault Model**:
  - Type of fault induced (instruction skip, data corruption, etc.)
  - Affected component (CPU, memory, peripheral)
  - Timing of the fault relative to operations

- **Technical Explanation**:
  - Circuit behavior during the glitch
  - Software execution path analysis
  - Assembly-level explanation if available

- **Comparison with Similar Attacks**:
  - How this attack relates to known techniques
  - Novel aspects of your approach

Example:
```markdown
## Analysis

### Root Cause
The successful glitch appears to cause an instruction skip during the password comparison 
routine. Based on the timing and behavior, we believe the glitch affects the conditional 
branch instruction that would normally exit the authentication routine if the password 
is incorrect.

### Fault Model
The most likely fault model is an instruction skip affecting the CPU core. The timing 
corresponds to the execution of a branch instruction at address 0x08001A4C (identified 
through firmware analysis). When this instruction is skipped, execution continues as if 
the comparison had succeeded.

### Circuit Behavior
The voltage glitch creates a momentary brownout condition that affects the CPU's ability 
to fetch and execute instructions correctly. The relatively quick recovery (350ns) prevents 
the brownout detection circuit from triggering a full reset, allowing execution to continue 
with the skipped instruction.

### Comparison with Known Techniques
This attack is similar to documented voltage glitching attacks against other ARM Cortex-M 
based microcontrollers. However, the target's brownout detection appears to be less sensitive 
than typical implementations, making it more vulnerable to this type of attack.
```

### 7. Countermeasures

Discussion of potential mitigations:

- **Hardware Countermeasures**:
  - Improved power supply filtering
  - Voltage monitors
  - Clock monitors
  - Physical protection

- **Software Countermeasures**:
  - Redundant checks
  - Control flow integrity
  - Timing variation
  - Checksums and integrity verification

- **Effectiveness Assessment**:
  - How effective each countermeasure would be
  - Implementation complexity
  - Performance impact

Example:
```markdown
## Countermeasures

### Recommended Hardware Countermeasures
1. **Improved Power Supply Filtering**: Adding additional decoupling capacitors (100nF and 10μF) 
   close to the VCC pin would make the device more resistant to voltage glitches.
2. **Voltage Monitor**: Implementing an external voltage supervisor with a fast response time 
   (<100ns) would detect glitches and trigger a reset.

### Recommended Software Countermeasures
1. **Redundant Verification**: Implementing multiple password checks at different points in 
   the code would require multiple successful glitches to bypass.
2. **Control Flow Integrity**: Adding checksums or signatures to critical code paths would 
   detect instruction skips.
3. **Temporal Redundancy**: Repeating the verification process multiple times with delays 
   would make timing-specific glitches less effective.

### Effectiveness Assessment
The most effective approach would be a combination of hardware and software countermeasures. 
The hardware improvements would increase the difficulty of performing a successful glitch, 
while the software countermeasures would provide defense in depth if the hardware protection 
is bypassed.
```

### 8. Conclusion

Summary of your findings:

- **Key Takeaways**:
  - Most important findings
  - Significance of the vulnerability
  - Broader implications

- **Future Work**:
  - Additional tests to consider
  - Variations of the attack to explore
  - Related research directions

Example:
```markdown
## Conclusion

This research demonstrates that the XYZ Secure Microcontroller is vulnerable to voltage 
glitching attacks that can bypass its authentication mechanism. The attack has a relatively 
high success rate (70%) and requires only moderate expertise and equipment to perform.

The vulnerability undermines the security claims of the device and could allow unauthorized 
access to protected functionality and sensitive data. The most concerning aspect is that the 
attack leaves no permanent evidence, making it difficult to detect that the device has been 
compromised.

Future work should explore whether similar vulnerabilities exist in other security-critical 
operations of the device, such as encryption key handling and secure boot processes. Additionally, 
testing the proposed countermeasures would provide valuable validation of their effectiveness.
```

### 9. Appendices

Supporting information:

- **Raw Data**:
  - Complete test results
  - Parameter sweep data
  - Success rate tables

- **Code and Scripts**:
  - Badge configuration scripts
  - Analysis scripts
  - Custom tools developed

- **Additional Waveforms**:
  - Comprehensive set of captures
  - Comparison of successful vs. failed attempts
  - Annotated waveforms

- **References**:
  - Related research papers
  - Datasheets and technical documentation
  - Tools and methodologies used

## Documentation Tools

The LayerOne 2025 GLiTCh BadgE provides several features to help with documentation:

### Capture and Export

- **ADC Data Export**:
  ```
  adc_stream save capture.csv
  ```

- **Terminal Log Export**:
  ```
  log save session.txt
  ```

- **Screenshot Capture**:
  ```
  screenshot save screen.png
  ```

### Automated Documentation

- **Test Campaign Reports**:
  ```
  campaign report --format=markdown > campaign_results.md
  ```

- **Parameter Sweep Visualization**:
  ```
  glitch visualize sweep --format=heatmap > sweep_results.png
  ```

- **Session Recording**:
  ```
  session record start
  # Perform your tests
  session record stop
  session export report.md
  ```

## Best Practices

### 1. Document in Real-Time

Don't wait until after your research to document. Take notes during the process:

- Record initial observations
- Document parameter changes and results immediately
- Capture waveforms as you go
- Note unexpected behaviors when they occur

### 2. Use Standard Formats

Standardize your documentation format:

- Use consistent units (ns, μs, MHz, etc.)
- Define abbreviations and technical terms
- Use clear naming conventions for files and parameters
- Include timestamps for captures and tests

### 3. Include Metadata

Always include contextual information:

- Date and time of tests
- Environmental conditions (temperature, power supply quality)
- Badge firmware version
- Target device details
- Test equipment configuration

### 4. Visual Documentation

A picture is worth a thousand words:

- Include photos of your setup
- Use diagrams to explain connections
- Annotate waveforms with key events
- Use tables for parameter comparisons
- Create graphs for success rates and correlations

### 5. Version Control

Track changes to your documentation:

- Use version control (Git) for scripts and notes
- Date all versions of your documentation
- Note major changes between versions
- Archive raw data separately from analysis

## Example Documentation Template

```markdown
# Hardware Security Research Report

## Target Information
- Device: [Manufacturer and Model]
- Version: [Hardware/Firmware Version]
- Security Features: [List of security features]

## Test Setup
- Equipment: LayerOne 2025 GLiTCh BadgE (Firmware v1.2.3)
- Connections: [Diagram or description]
- Additional Equipment: [List any other equipment]
- Software Tools: [List software tools used]

## Methodology
1. [Step 1]
2. [Step 2]
3. [Step 3]

## Attack Parameters
- Type: [Voltage/Clock/Reset/Crowbar] Glitch
- Width: [Value] [Units]
- Delay: [Value] [Units]
- Trigger: [Description]
- [Other relevant parameters]

## Results
- Success Rate: [X/Y] attempts ([Z]%)
- Observed Behavior: [Description]
- Waveforms: [Include or reference waveforms]
- Parameter Sensitivity: [Description]

## Analysis
- Root Cause: [Description]
- Fault Model: [Description]
- Technical Explanation: [Description]

## Countermeasures
- Hardware: [List recommendations]
- Software: [List recommendations]
- Effectiveness: [Assessment]

## Conclusion
[Summary of findings and implications]

## Appendices
- Raw Data: [Links or references]
- Code: [Links or references]
- Additional Waveforms: [Links or references]
- References: [List of references]
```

## Responsible Disclosure

When documenting vulnerabilities, consider responsible disclosure:

1. **Notify the Manufacturer**:
   - Provide clear documentation
   - Include reproduction steps
   - Suggest countermeasures

2. **Establish a Timeline**:
   - Give reasonable time for fixes
   - Coordinate disclosure dates
   - Follow up on mitigation progress

3. **Limit Sensitive Details**:
   - Consider what details to include in public reports
   - Balance transparency with security
   - Focus on methodology rather than exploit details

## Conclusion

Thorough documentation is as important as the technical work itself in hardware security research. By following these guidelines, you can create comprehensive, useful documentation that enhances the value of your research and contributes to the security community's knowledge base.

The LayerOne 2025 GLiTCh BadgE's built-in documentation features make it easier to capture and organize your findings, allowing you to focus on the research while still maintaining excellent documentation practices.