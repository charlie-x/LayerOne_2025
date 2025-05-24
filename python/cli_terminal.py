#!/usr/bin/env python3
"""
cli terminal for layerone 2025 badge

this script provides a simple terminal interface to send commands to the
layerone 2025 badge's cli interface and display the responses.

usage:
    python cli_terminal.py [port]

    if port is not specified, the script will try to auto-detect the port.
"""

import sys
import os
import re
import time
import platform
import subprocess
import serial
import serial.tools.list_ports
import readline  # for command history
import threading
import select

# constants
BAUD_RATE = 115200
PROMPT = "badge> "
RECONNECT_DELAY = 2  # seconds to wait between reconnection attempts

def find_macos_cli_port_using_system_commands():
    """use macos system commands to find the cli interface port."""
    import subprocess
    import re
    import os
    
    # check if the badge is connected using system_profiler
    try:
        result = subprocess.run(['system_profiler', 'SPUSBDataType'], capture_output=True, text=True)
        if "NullSpaceLabs" not in result.stdout and "glitch badge 2025" not in result.stdout.lower():
            print("Badge not found using system_profiler. Looking for 'NullSpaceLabs' or 'glitch badge 2025'.")
            return None
    except Exception as e:
        print(f"error running system_profiler: {e}")
        return None
    
    # try to find the cli interface using ioreg
    try:
        # get interface names
        result = subprocess.run(['ioreg', '-c', 'IOUSBHostInterface', '-r'], capture_output=True, text=True)
        
        # look for cli interface
        if "IOUSBHostInterface" in result.stdout and "cu.usbmodem" in result.stdout:
            print("found cli interface in ioreg output.")
            return None  # let the serial port detection handle it
    except Exception as e:
        print(f"error running ioreg: {e}")
    
    # try to find the cli interface using ls
    try:
        # get list of devices in /dev
        result = subprocess.run(['ls', '/dev'], capture_output=True, text=True)
        
        # look for usbmodem devices
        modem_pattern = re.compile(r'cu\.usbmodem\d+')
        modem_devices = modem_pattern.findall(result.stdout)
        
        if modem_devices:
            # sort by numeric suffix
            modem_devices.sort(key=lambda x: int(''.join(filter(str.isdigit, x))))
            port = f"/dev/{modem_devices[0]}"
            print(f"found cli interface using ls: {port}")
            return port
    except Exception as e:
        print(f"error running ls: {e}")
    
    return None

def find_badge_port():
    """find the badge's cli interface port."""
    # try to find the badge using system commands first
    if platform.system() == "Darwin":  # macos
        port = find_macos_cli_port_using_system_commands()
        if port:
            print(f"Found badge port using macOS system commands: {port}")
            return port
    
    print("system command detection didn't find the port, falling back to pyserial detection...")
    
    # list all serial ports
    ports = list(serial.tools.list_ports.comports())
    
    # print available ports with more details
    print(f"found {len(ports)} serial ports:")
    for i, port in enumerate(ports):
        manufacturer = port.manufacturer if port.manufacturer else "Unknown"
        description = port.description if port.description else "Unknown"
        print(f"{i+1}. {port.device} - {description}")
        print(f"   Manufacturer: {manufacturer}")
        print(f"   Hardware ID: {port.hwid}")
    
    # look for badge ports
    badge_ports = []
    for port in ports:
        # check if it's a badge port
        if (("0d28:2488" in port.hwid.lower()) or  # vid:pid for badge
            ("nullspacelabs" in port.manufacturer.lower() if port.manufacturer else False) or
            ("glitch badge 2025" in port.description.lower() if port.description else False) or
            ("layerone" in port.description.lower() if port.description else False) or
            ("glitchy" in port.description.lower() if port.description else False)):
            badge_ports.append(port)
            print(f"Found badge port with matching criteria: {port.device}")
    
    if badge_ports:
        print(f"\nfound {len(badge_ports)} glitch badge 2025 or pico device(s)")
        
        # First, look specifically for the CLI interface port
        # Based on our investigation, the CLI interface is on the port ending with "11301"
        for port in badge_ports:
            if "11301" in port.device:
                print(f"Found CLI interface port (11301 pattern): {port.device}")
                return port.device
        
        # Platform-specific detection logic
        if platform.system() == "Darwin":  # macOS
            # Group ports by their base name
            port_groups = {}
            for port in badge_ports:
                # extract base name (e.g., /dev/cu.usbmodem11301 -> /dev/cu.usbmodem)
                base = re.sub(r'\d+$', '', port.device)
                if base not in port_groups:
                    port_groups[base] = []
                port_groups[base].append(port)
            
            # Look for other patterns
            for port in badge_ports:
                if "usbmodem" in port.device and (port.device.endswith("1") or
                                                port.device.endswith("1234561")):
                    print(f"Found CLI interface (by pattern): {port.device}")
                    return port.device
            
            # fallback to looking for the first interface in each group
            for base, group in port_groups.items():
                if len(group) >= 1:
                    # sort by the numeric suffix
                    group.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device.replace(base, '')))))
                    # the first interface (index 0) should be cdc0
                    print(f"found cli interface (first interface): {group[0].device}")
                    return group[0].device
                    
        elif platform.system() == "Windows":  # windows
            # look for com ports with specific patterns (com3 is often the cli interface)
            for port in badge_ports:
                if port.device.endswith("3"):  # com3, com13, etc.
                    print(f"found cli interface (by pattern): {port.device}")
                    return port.device
            
            # on windows, try to group com ports by their location id
            port_groups = {}
            for port in badge_ports:
                # extract location id from hwid if possible
                loc_match = re.search(r'LOCATION=(\S+)', port.hwid)
                location = loc_match.group(1) if loc_match else "unknown"
                if location not in port_groups:
                    port_groups[location] = []
                port_groups[location].append(port)
            
            # for each group, try to find the first interface
            for location, group in port_groups.items():
                if len(group) >= 1:
                    # sort by com port number
                    group.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device))))
                    # the first interface (index 0) should be cdc0
                    print(f"found cli interface (first interface): {group[0].device}")
                    return group[0].device
        
        elif platform.system() == "Linux":  # linux
            # look for acm ports with specific patterns (ttyacm0 is often the cli interface)
            acm_ports = [p for p in badge_ports if "ttyACM" in p.device]
            for port in acm_ports:
                if port.device.endswith("0"):  # ttyacm0, etc.
                    print(f"found cli interface (by pattern): {port.device}")
                    return port.device
            
            # fallback to looking for the first interface in each group
            port_groups = {}
            for port in badge_ports:
                # extract base name
                base = re.sub(r'\d+$', '', port.device)
                if base not in port_groups:
                    port_groups[base] = []
                port_groups[base].append(port)
                
            for base, group in port_groups.items():
                if len(group) >= 1:
                    # sort by the numeric suffix
                    group.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device.replace(base, '')))))
                    # the first interface (index 0) should be cdc0
                    print(f"found cli interface (first interface): {group[0].device}")
                    return group[0].device
        
        # If we have multiple badge ports but couldn't identify the CLI interface
        # using platform-specific logic, use the first one
        if len(badge_ports) > 0:
            print(f"Using first badge port as fallback: {badge_ports[0].device}")
            return badge_ports[0].device
    
    print("\ncouldn't find any suitable ports, using default for this os")
    
    # fallback to default port for this os
    if platform.system() == "Darwin":  # macos
        # check if any usbmodem devices exist
        devices = os.listdir('/dev')
        modem_devices = [d for d in devices if d.startswith('cu.usbmodem')]
        if modem_devices:
            # Sort to prioritize 11301 if it exists
            modem_devices.sort(key=lambda x: 0 if "11301" in x else 1)
            port = f"/dev/{modem_devices[0]}"
            print(f"Using first available usbmodem device: {port}")
            return port
        return "/dev/cu.usbmodem11301"  # common macos port for CLI interface
    else:
        return "/dev/ttyACM0"  # common linux port for cli interface

def port_exists(port_name):
    """check if a port exists in the system."""
    ports = [p.device for p in serial.tools.list_ports.comports()]
    return port_name in ports

def read_and_display_data(ser, connection_status):
    """continuously read and display data from the serial port."""
    while connection_status["connected"]:
        try:
            # check if there's data to read
            if ser.in_waiting > 0:
                # read all available data
                data = ser.read(ser.in_waiting).decode('utf-8', errors='replace')
                
                # display the data
                sys.stdout.write(data)
                sys.stdout.flush()
            
            # sleep a short time to avoid hogging CPU
            time.sleep(0.05)  # Increased sleep time
            
        except (serial.SerialException, OSError) as e:
            print(f"\nconnection lost in read thread: {e}")
            connection_status["connected"] = False
            break

def monitor_connection(ser, port, connection_status):
    """monitor the connection and set status to false if connection is lost."""
    while connection_status["connected"]:
        try:
            # check if port still exists
            if not port_exists(port):
                print(f"\nport {port} no longer exists!")
                connection_status["connected"] = False
                break
                
        except (serial.SerialException, OSError) as e:
            print(f"\nconnection lost: {e}")
            connection_status["connected"] = False
            break
            
        time.sleep(0.5)  # check every half second

def run_terminal(port):
    """run the terminal interface with reconnection capability."""
    # keep track of connection status
    connection_status = {"connected": False}
    
    while True:
        try:
            # check if port exists before trying to connect
            if not port_exists(port):
                print(f"port {port} not available. waiting for it to appear...")
                while not port_exists(port):
                    time.sleep(RECONNECT_DELAY)
                print(f"port {port} is now available!")
            
            # open serial port with optimized settings for better reliability
            ser = serial.Serial(
                port=port,
                baudrate=BAUD_RATE,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1.0,
                write_timeout=1.0,
                xonxoff=False,     # disable software flow control
                rtscts=False,      # disable hardware (RTS/CTS) flow control
                dsrdtr=False,      # disable hardware (DSR/DTR) flow control
                inter_byte_timeout=None
            )
            print(f"connected to {port} with baud rate {BAUD_RATE}")
            
            # update connection status
            connection_status["connected"] = True
            
            # start monitoring thread
            monitor_thread = threading.Thread(
                target=monitor_connection, 
                args=(ser, port, connection_status)
            )
            monitor_thread.daemon = True
            monitor_thread.start()
            
            # start data reading thread
            read_thread = threading.Thread(
                target=read_and_display_data,
                args=(ser, connection_status)
            )
            read_thread.daemon = True
            read_thread.start()
            
            # clear any pending data
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            
            # Try multiple times to get a prompt
            print("Attempting to get prompt...")
            for attempt in range(3):
                print(f"Prompt attempt {attempt+1}/3...")
                
                # Send a newline and wait for response
                ser.write(b'\r\n')
                ser.flush()
                
                # Wait for response with timeout
                start_time = time.time()
                response_received = False
                timeout = 1.0
                
                while time.time() - start_time < timeout:
                    if ser.in_waiting > 0:
                        data = ser.read(ser.in_waiting).decode('utf-8', errors='replace')
                        print(f"Received: {repr(data)}")
                        response_received = True
                        break
                    time.sleep(0.1)
                
                if response_received:
                    print("Got response, continuing...")
                    break
                else:
                    print(f"No response on attempt {attempt+1}, trying again...")
            
            # One final attempt with a different approach if previous attempts failed
            if not response_received:
                print("Trying alternative approach to wake up device...")
                # Send newline
                ser.write(b'\r\n')
                ser.flush()
                time.sleep(1.0)
                
                # Check if we got a response
                if ser.in_waiting > 0:
                    data = ser.read(ser.in_waiting).decode('utf-8', errors='replace')
                    print(f"Received response: {repr(data)}")
                    response_received = True
            
            # print welcome message
            print("\n" + "=" * 60)
            print("layerone 2025 glitch badge cli terminal")
            print("=" * 60)
            print("type 'help' for a list of commands")
            print("type 'exit' or press ctrl+c to exit")
            print("connection will automatically reconnect if lost")
            print("=" * 60 + "\n")
            
            # enable command history
            readline.parse_and_bind('tab: complete')
            
            # main loop
            while connection_status["connected"]:
                # show prompt
                sys.stdout.write(PROMPT)
                sys.stdout.flush()
                
                # get command from user
                try:
                    command = input()
                    
                    # check for exit command
                    if command.lower() == 'exit':
                        print("exiting...")
                        connection_status["connected"] = False
                        break
                    
                    # check for special debug command
                    if command.lower() == 'debug':
                        print("\n=== DEBUG INFORMATION ===")
                        print(f"Connected to: {port}")
                        print(f"Baud rate: {BAUD_RATE}")
                        print(f"Connection status: {connection_status}")
                        print(f"Serial port open: {ser.is_open}")
                        print(f"Serial port settings: {ser.get_settings()}")
                        print("=========================\n")
                        continue
                    
                    # check for reset command
                    if command.lower() == 'reset':
                        print("Resetting connection...")
                        ser.reset_input_buffer()
                        ser.reset_output_buffer()
                        # Send Ctrl+C followed by newline to reset the badge CLI
                        ser.write(b'\x03\r\n')
                        ser.flush()
                        time.sleep(1.0)
                        continue
                    
                    # check if connection is still active
                    if not connection_status["connected"]:
                        print("connection lost, attempting to reconnect...")
                        break
                    
                    # send command to device with improved handling
                    try:
                        # Clear input buffer before sending command
                        ser.reset_input_buffer()
                        
                        # Send command with explicit CR+LF and ensure it's flushed
                        cmd_bytes = (command + '\r\n').encode('utf-8')
                        bytes_written = ser.write(cmd_bytes)
                        ser.flush()  # Ensure data is sent immediately
                        
                        print(f"Sent command: {repr(command)}, bytes written: {bytes_written}")
                        
                        # Wait longer for the command to be processed
                        time.sleep(1.0)  # Increased wait time
                            
                    except (serial.SerialException, OSError) as e:
                        print(f"\nerror communicating with device: {e}")
                        connection_status["connected"] = False
                        break
                    
                except KeyboardInterrupt:
                    print("\nexiting...")
                    connection_status["connected"] = False
                    break
                except Exception as e:
                    print(f"\nerror: {e}")
                    if not connection_status["connected"]:
                        break
            
            # clean up
            try:
                if ser.is_open:
                    ser.close()
                    print("serial port closed")
            except Exception as e:
                print(f"error closing serial port: {e}")
            
            # if we're not connected anymore, try to reconnect
            if not connection_status["connected"]:
                print(f"connection lost. attempting to reconnect in {RECONNECT_DELAY} seconds...")
                time.sleep(RECONNECT_DELAY)
            
        except KeyboardInterrupt:
            print("\nexiting...")
            break
        except Exception as e:
            print(f"unexpected error: {e}")
            time.sleep(RECONNECT_DELAY)

def main():
    """main function."""
    # get port from command line or auto-detect
    if len(sys.argv) > 1:
        port = sys.argv[1]
        print(f"using port {port} from command line")
    else:
        port = find_badge_port()
        print(f"auto-detected port: {port}")
    
    # run terminal
    run_terminal(port)

if __name__ == "__main__":
    main()