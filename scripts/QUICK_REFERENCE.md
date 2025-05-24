# LayerOne 2025 Badge Scripting Language - Quick Reference

## Basic Syntax

- One command per line
- Lines starting with `#` are comments
- Variables are created automatically when assigned
- No declaration needed for variables
- Case-sensitive

## Commands

### System Commands

| Command | Description | Example |
|---------|-------------|---------|
| `sleep <time>[ms|us|ns]` | Pause execution | `sleep 100ms` |
| `print <text>` | Output text | `print "Hello, world!"` |
| `exec <command>` | Execute system command | `exec status` |

### GPIO Commands

| Command | Description | Example |
|---------|-------------|---------|
| `pin <number> out` | Set pin as output | `pin 25 out` |
| `pin <number> in` | Set pin as input | `pin 2 in` |
| `pin <number> high` | Set pin high | `pin 25 high` |
| `pin <number> low` | Set pin low | `pin 25 low` |
| `pin <number> pullup` | Enable pullup resistor | `pin 2 pullup` |
| `pin <number> pulldown` | Enable pulldown resistor | `pin 2 pulldown` |
| `pin <number> nopull` | Disable pull resistors | `pin 2 nopull` |
| `pin <number> -> <var>` | Read pin state to variable | `pin 2 -> button_state` |

### ADC Commands

| Command | Description | Example |
|---------|-------------|---------|
| `adc init <channel>` | Initialize ADC channel | `adc init 0` |
| `adc temp_init` | Initialize temperature sensor | `adc temp_init` |
| `adc <channel> -> <var>` | Read ADC value to variable | `adc 0 -> pot_value` |
| `adc temp -> <var>` | Read temperature to variable | `adc temp -> temp_raw` |

### Control Flow

| Command | Description | Example |
|---------|-------------|---------|
| `if <condition>` | Start conditional block | `if pot_value > 100` |
| `else` | Alternative execution path | `else` |
| `endif` | End conditional block | `endif` |
| `loop <count>` | Loop a block of code | `loop 10` |
| `endloop` | End loop block | `endloop` |
| `goto <label>` | Jump to label | `goto start` |
| `<label>:` | Define a label | `start:` |

### Variables and Expressions

| Command | Description | Example |
|---------|-------------|---------|
| `<expr> -> <var>` | Assign value to variable | `pot_value * 2 -> doubled_value` |
| `$result` | Special variable with last result | `$result -> last_command_result` |

## Operators

### Arithmetic Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `+` | Addition | `a + b -> c` |
| `-` | Subtraction | `a - b -> c` |
| `*` | Multiplication | `a * b -> c` |
| `/` | Division | `a / b -> c` |

### Comparison Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `==` | Equal to | `if a == b` |
| `!=` | Not equal to | `if a != b` |
| `<` | Less than | `if a < b` |
| `>` | Greater than | `if a > b` |
| `<=` | Less than or equal to | `if a <= b` |
| `>=` | Greater than or equal to | `if a >= b` |

## Examples

### Blink LED

```
# Blink LED on pin 25
pin 25 out

loop 10
    pin 25 high
    print "LED on"
    sleep 500ms
    pin 25 low
    print "LED off"
    sleep 500ms
endloop
```

### Read Button and Control LED

```
# Set up pins
pin 2 in
pin 2 pullup
pin 25 out

loop 100
    pin 2 -> button
    
    if button == 0
        pin 25 high
        print "Button pressed, LED on"
    else
        pin 25 low
        print "Button released, LED off"
    endif
    
    sleep 10ms
endloop
```

### Read ADC and Control LED Brightness

```
# ADC controlled LED
adc init 0
pin 25 out

loop 100
    adc 0 -> pot_value
    
    if pot_value > 512
        pin 25 high
        print "LED on"
    else
        pin 25 low
        print "LED off"
    endif
    
    sleep 100ms
endloop
```

### Temperature Monitor

```
# Temperature monitor
adc temp_init

loop 60
    adc temp -> temp_raw
    
    # Convert raw value to temperature
    temp_raw * 3300 / 4096 -> millivolts
    millivolts - 706 -> adjusted
    adjusted / 1.721 -> celsius
    
    print "Temperature: " celsius " C"
    
    if celsius > 30
        print "Warning: High temperature!"
    endif
    
    sleep 1000ms
endloop
```

## Limitations

- Maximum script size: 256 lines
- Maximum line length: 128 characters
- Maximum variables: 32
- Maximum labels: 32
- Maximum call stack depth: 16
- Maximum variable name length: 16 characters
- Maximum string length: 64 characters
- Only integer and string data types
- Limited error reporting
- No functions or subroutines
- No arrays or complex data structures