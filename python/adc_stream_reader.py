#!/usr/bin/env python3
"""
ADC Stream Reader for LayerOne 2025 Badge

This script reads and decodes the ADC stream from the LayerOne 2025 badge.
The badge streams ADC0 and ADC1 values at 1000Hz over USB CDC2.

Packet format:
- Byte 0: Header byte (0xAA) for synchronization
- Byte 1: Sequence number (increments with each packet)
- Byte 2: Low 8 bits of ADC0
- Byte 3: High 4 bits of ADC0 and low 4 bits of ADC1
- Byte 4: High 8 bits of ADC1

Usage:
    python adc_stream_reader.py [port]

    If port is not specified, the script will try to auto-detect the port.
"""

import sys
import time
import re
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import threading

# constants
HEADER_BYTE = 0xAA
PACKET_SIZE = 5
BUFFER_SIZE = 1000  # number of samples to display
RECONNECT_DELAY = 2  # seconds to wait between reconnection attempts

# global variables for plotting
adc0_values = np.zeros(BUFFER_SIZE)
adc1_values = np.zeros(BUFFER_SIZE)
lost_packets = 0
total_packets = 0
connection_status = {"connected": False, "port": None}

def find_macos_badge_port_using_system_commands():
    """Use macOS system commands to find the ADC Stream Interface port."""
    import subprocess
    import re
    import os
    
    # Check if the badge is connected using system_profiler
    try:
        result = subprocess.run(['system_profiler', 'SPUSBDataType'], capture_output=True, text=True)
        if "GLiTCh Badge 2025" not in result.stdout:
            print("GLiTCh Badge 2025 not found using system_profiler.")
            return None
    except Exception as e:
        print(f"Error running system_profiler: {e}")
        return None
    
    # Try to find the ADC Stream Interface using ioreg
    try:
        # Get interface names
        result = subprocess.run(['ioreg', '-c', 'IOUSBHostInterface', '-r'], capture_output=True, text=True)
        
        # Look for ADC Stream Interface
        match = re.search(r'"kUSBString" = "ADC Stream Interface"', result.stdout)
        if match:
            print("Found ADC Stream Interface in ioreg output.")
            
            # Now find the corresponding serial port
            try:
                # Get a list of all usbmodem devices
                devices = os.listdir('/dev')
                modem_devices = [d for d in devices if d.startswith('cu.usbmodem')]
                
                # Look for port ending with 5
                for device in modem_devices:
                    if device.endswith('5') or '1234565' in device:
                        port = f"/dev/{device}"
                        print(f"Found ADC Stream Interface port using system commands: {port}")
                        return port
                
                print("No port ending with '5' found in /dev")
            except Exception as e:
                print(f"Error listing /dev directory: {e}")
    except Exception as e:
        print(f"Error running ioreg: {e}")
    
    return None

def find_windows_badge_port_using_system_commands():
    """Use Windows system commands to find the ADC Stream Interface port."""
    import subprocess
    import re
    
    try:
        # Use PowerShell to get USB devices
        result = subprocess.run(['powershell', '-command',
                                '& {Get-PnpDevice | Where-Object { $_.FriendlyName -like "*GLiTCh Badge 2025*" } | Format-List}'],
                                capture_output=True, text=True)
        
        if "GLiTCh Badge 2025" not in result.stdout:
            print("GLiTCh Badge 2025 not found using PowerShell.")
            return None
            
        # Get COM ports
        result = subprocess.run(['powershell', '-command',
                                '& {Get-PnpDevice -Class Ports | Format-List}'],
                                capture_output=True, text=True)
        
        # Look for COM port with ADC Stream Interface
        match = re.search(r'FriendlyName.*ADC Stream Interface.*\(COM(\d+)\)', result.stdout)
        if match:
            port = f"COM{match.group(1)}"
            print(f"Found ADC Stream Interface port using system commands: {port}")
            return port
            
        # Look for COM5 (common for ADC Stream Interface)
        match = re.search(r'FriendlyName.*GLiTCh Badge 2025.*\(COM(\d+)\)', result.stdout)
        if match and match.group(1) == "5":
            port = "COM5"
            print(f"Found ADC Stream Interface port using system commands: {port}")
            return port
    except Exception as e:
        print(f"Error running PowerShell commands: {e}")
    
    return None

def find_linux_badge_port_using_system_commands():
    """Use Linux system commands to find the ADC Stream Interface port."""
    import subprocess
    import re
    
    try:
        # Check if the badge is connected using lsusb
        result = subprocess.run(['lsusb'], capture_output=True, text=True)
        if "1209:2025" not in result.stdout:
            print("GLiTCh Badge 2025 not found using lsusb.")
            return None
            
        # Get more detailed USB info
        result = subprocess.run(['lsusb', '-v', '-d', '1209:2025'], capture_output=True, text=True)
        
        # Look for ADC Stream Interface
        interfaces = re.findall(r'iInterface\s+\d+\s+(.*)', result.stdout)
        adc_interface_found = False
        for interface in interfaces:
            if "ADC Stream Interface" in interface:
                adc_interface_found = True
                break
                
        if adc_interface_found:
            # Find ACM devices
            result = subprocess.run(['ls', '-la', '/dev/ttyACM*'], capture_output=True, text=True)
            
            # Look for ttyACM2 (common for ADC Stream Interface)
            for line in result.stdout.splitlines():
                if '/dev/ttyACM2' in line:
                    print(f"Found ADC Stream Interface port using system commands: /dev/ttyACM2")
                    return '/dev/ttyACM2'
    except Exception as e:
        print(f"Error running Linux commands: {e}")
    
    return None

def port_exists(port_name):
    """check if a port exists in the system."""
    ports = [p.device for p in serial.tools.list_ports.comports()]
    return port_name in ports

def find_badge_port():
    """try to find the layerone 2025 badge port automatically."""
    import platform
    
    # try platform-specific system commands first
    if platform.system() == "Darwin":  # macos
        macos_port = find_macos_badge_port_using_system_commands()
        if macos_port:
            return macos_port
    elif platform.system() == "Windows":  # windows
        windows_port = find_windows_badge_port_using_system_commands()
        if windows_port:
            return windows_port
    elif platform.system() == "Linux":  # linux
        linux_port = find_linux_badge_port_using_system_commands()
        if linux_port:
            return linux_port
    
    # If system commands didn't work, fall back to pyserial detection
    print("System command detection didn't find the port, falling back to pyserial detection...")
    
    # Get all available ports
    ports = list(serial.tools.list_ports.comports())
    
    if not ports:
        print("No serial ports found. Is the device connected?")
        sys.exit(1)
    
    print(f"Found {len(ports)} serial ports:")
    for i, port in enumerate(ports):
        print(f"{i+1}. {port.device} - {port.description} ({port.hwid})")
    
    # First, try to find the LayerOne 2025 badge by VID:PID
    badge_ports = []
    for port in ports:
        if "1209:2025" in port.hwid.lower():  # GLiTCh Badge 2025 VID:PID
            badge_ports.append(port)
        elif "2e8a:000c" in port.hwid.lower():  # Raspberry Pi Pico VID:PID (fallback)
            badge_ports.append(port)
    
    if badge_ports:
        print(f"\nFound {len(badge_ports)} GLiTCh Badge 2025 or Pico device(s)")
        
        # First, try to find a port with "ADC Stream Interface" in the description
        for port in badge_ports:
            if "ADC Stream Interface" in port.description:
                print(f"Found ADC Stream Interface by name: {port.device}")
                return port.device
        
        # Platform-specific detection methods
        if platform.system() == "Darwin":  # macOS
            # Group ports by device base name to identify related interfaces
            port_groups = {}
            for port in badge_ports:
                # Extract base name (remove trailing numbers)
                base = ''.join(c for c in port.device if not c.isdigit())
                if base not in port_groups:
                    port_groups[base] = []
                port_groups[base].append(port)
            
            # First, try to find ports with specific patterns that indicate ADC Stream Interface
            for port in badge_ports:
                if "usbmodem" in port.device and (port.device.endswith("5") or port.device.endswith("1234565")):
                    print(f"Found ADC Stream interface (by pattern): {port.device}")
                    return port.device
            
            # Fallback to looking for the third interface in each group
            for base, group in port_groups.items():
                if len(group) >= 3:
                    # Sort by the numeric suffix
                    group.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device.replace(base, '')))))
                    # The third interface (index 2) should be CDC2
                    print(f"Found ADC Stream interface (third interface): {group[2].device}")
                    return group[2].device
        
        elif platform.system() == "Windows":  # Windows
            # First, try to find ports with "ADC Stream Interface" in the description
            for port in badge_ports:
                if "ADC Stream Interface" in port.description:
                    print(f"Found ADC Stream Interface by name: {port.device}")
                    return port.device
            
            # Then, look for COM ports with specific patterns (COM5 is often the ADC Stream Interface)
            for port in badge_ports:
                if port.device.endswith("5"):  # COM5, COM15, etc.
                    print(f"Found ADC Stream interface (by pattern): {port.device}")
                    return port.device
            
            # On Windows, try to group COM ports by their location ID
            port_groups = {}
            for port in badge_ports:
                # Extract location ID from hwid if possible
                loc_match = re.search(r'LOCATION=(\S+)', port.hwid)
                location = loc_match.group(1) if loc_match else "unknown"
                if location not in port_groups:
                    port_groups[location] = []
                port_groups[location].append(port)
            
            # For each group, try to find the third interface
            for location, group in port_groups.items():
                if len(group) >= 3:
                    # Sort by COM port number
                    group.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device))))
                    # The third interface (index 2) should be CDC2
                    print(f"Found ADC Stream interface (third interface): {group[2].device}")
                    return group[2].device
            
            # Fallback to looking for the highest numbered COM port
            if badge_ports:
                badge_ports.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device))))
                highest_port = badge_ports[-1].device
                print(f"Using highest numbered COM port: {highest_port}")
                return highest_port
        
        elif platform.system() == "Linux":  # Linux
            # first, try to find ports with "ADC Stream Interface" in the description
            for port in badge_ports:
                if "ADC Stream Interface" in port.description:
                    print(f"Found ADC Stream Interface by name: {port.device}")
                    return port.device
            
            # then, look for ACM ports with specific patterns (ttyACM2 is often the ADC Stream Interface)
            acm_ports = [p for p in badge_ports if "ttyACM" in p.device]
            for port in acm_ports:
                if port.device.endswith("2") or port.device.endswith("5"):  # ttyACM2, ttyACM5, etc.
                    print(f"Found ADC Stream interface (by pattern): {port.device}")
                    return port.device
            
            # on Linux, try to group ACM ports by their location
            port_groups = {}
            for port in badge_ports:
                if "ttyACM" in port.device:
                    # extract location from hwid if possible
                    loc_match = re.search(r'LOCATION=(\S+)', port.hwid)
                    location = loc_match.group(1) if loc_match else "unknown"
                    if location not in port_groups:
                        port_groups[location] = []
                    port_groups[location].append(port)
            
            # for each group, try to find the third interface
            for location, group in port_groups.items():
                if len(group) >= 3:
                    # Sort by ACM number
                    group.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device))))
                    # The third interface (index 2) should be CDC2
                    print(f"Found ADC Stream interface (third interface): {group[2].device}")
                    return group[2].device
            
            # fallback to looking for the highest numbered ACM port
            if acm_ports:
                acm_ports.sort(key=lambda p: int(''.join(filter(str.isdigit, p.device))))
                highest_port = acm_ports[-1].device
                print(f"Using highest numbered ACM port: {highest_port}")
                return highest_port
        
        # if we couldn't find the specific CDC2 interface, just use the first badge port
        print(f"Couldn't identify CDC2 interface, using first badge port: {badge_ports[0].device}")
        return badge_ports[0].device
    
    # if we can't find the badge by VID:PID, look for likely CDC ports
    cdc_ports = []
    for port in ports:
        if "cdc" in port.description.lower() or "uart" in port.description.lower():
            cdc_ports.append(port)
    
    if cdc_ports:
        print(f"\nFound {len(cdc_ports)} CDC/UART ports")
        print(f"Using first CDC/UART port: {cdc_ports[0].device}")
        return cdc_ports[0].device
    
    # if we still can't find it, return a default based on OS
    print("\nCouldn't find any suitable ports, using default for this OS")
    if platform.system() == "Windows":
        return "COM5"  # common Windows port for ADC streaming
    elif platform.system() == "Darwin":
        return "/dev/cu.usbmodem1234565"  # common macOS port for ADC streaming
    else:
        return "/dev/ttyACM2"  # common Linux port for ADC streaming

def unpack_adc_values(buffer):
    """Unpack ADC values from the 5-byte buffer."""
    if len(buffer) != PACKET_SIZE:
        return None, None, None
    
    if buffer[0] != HEADER_BYTE:
        return None, None, None
    
    sequence = buffer[1]
    adc0_low = buffer[2]
    mixed = buffer[3]
    adc1_high = buffer[4]
    
    adc0_high = mixed & 0x0F
    adc1_low = (mixed >> 4) & 0x0F
    
    adc0 = (adc0_high << 8) | adc0_low
    adc1 = (adc1_high << 4) | adc1_low
    
    return sequence, adc0, adc1

def init_plot():
    """initialise the plot."""
    plt.style.use('dark_background')
    
    # create figure with more height
    fig = plt.figure(figsize=(12, 12))
    
    # create a special axis for the title
    title_ax = fig.add_axes([0, 0.95, 1, 0.05])
    title_ax.axis('off')  # hide axis
    
    # create two subplots with specific positions
    ax1 = fig.add_axes([0.1, 0.55, 0.8, 0.35])  # [left, bottom, width, height]
    ax2 = fig.add_axes([0.1, 0.1, 0.8, 0.35])
    
    # add title text object that we can update later
    title_text = title_ax.text(0.5, 0.7, 'layerOne 2025 badge adc stream',
                               horizontalalignment='center',
                               verticalalignment='center',
                               fontsize=14)
    
    # connection status text - positioned on the left side
    conn_text = title_ax.text(0.2, 0.3, 'connecting...',
                              horizontalalignment='left',
                              verticalalignment='center',
                              fontsize=12,
                              color='yellow')
    
    # stats text object - positioned on the right side
    stats_text = title_ax.text(0.8, 0.3, 'initializing...',
                               horizontalalignment='right',
                               verticalalignment='center',
                               fontsize=12)
    
    # adc0 plot
    ax1.set_ylim(0, 4096)
    ax1.set_title('adc0 values', pad=10)
    ax1.set_ylabel('adc value')
    ax1.grid(True, alpha=0.3)
    line1, = ax1.plot([], [], 'g-', linewidth=1.5)
    
    # adc1 plot
    ax2.set_ylim(0, 4096)
    ax2.set_title('adc1 values', pad=10)
    ax2.set_xlabel('sample')
    ax2.set_ylabel('adc value')
    ax2.grid(True, alpha=0.3)
    line2, = ax2.plot([], [], 'c-', linewidth=1.5)
    
    return fig, (line1, line2), title_text, stats_text, conn_text

def monitor_connection(ser, conn_text):
    """monitor the connection and update status."""
    global connection_status
    
    while connection_status["connected"]:
        try:
            # check if port still exists
            if not port_exists(connection_status["port"]):
                print(f"\nport {connection_status['port']} no longer exists!")
                conn_text.set_text(f"disconnected: port {connection_status['port']} removed")
                conn_text.set_color('red')
                connection_status["connected"] = False
                break
                
            # try to read from the port to check if it's still working
            try:
                ser.read(1)  # non-blocking due to timeout
            except (serial.SerialException, OSError) as e:
                print(f"\nconnection lost: {e}")
                conn_text.set_text(f"disconnected: {str(e)}")
                conn_text.set_color('red')
                connection_status["connected"] = False
                break
            
        except Exception as e:
            print(f"error in monitor thread: {e}")
            
        time.sleep(0.5)  # check every half second

def update_plot(frame, ser, lines, title_text, stats_text, conn_text):
    """update the plot with new data."""
    global adc0_values, adc1_values, lost_packets, total_packets, connection_status
    last_seq = 0  # initialise last_seq
    
    # check connection status
    if not connection_status["connected"]:
        # update connection status text
        conn_text.set_text("disconnected - waiting to reconnect...")
        conn_text.set_color('red')
        return None
    
    # read available data
    try:
        if ser.in_waiting >= PACKET_SIZE:
            # look for header byte
            while ser.in_waiting > 0:
                header = ser.read(1)[0]
                if header == HEADER_BYTE:
                    # read the rest of the packet
                    data = ser.read(PACKET_SIZE - 1)
                    if len(data) == PACKET_SIZE - 1:
                        buffer = bytearray([header]) + data
                        seq, adc0, adc1 = unpack_adc_values(buffer)
                        
                        if seq is not None and adc0 is not None and adc1 is not None:
                            # shift values and add new ones
                            adc0_values = np.roll(adc0_values, -1)
                            adc1_values = np.roll(adc1_values, -1)
                            adc0_values[-1] = adc0
                            adc1_values[-1] = adc1
                            
                            # check for lost packets
                            if total_packets > 0:
                                expected_seq = (last_seq + 1) % 256
                                if seq != expected_seq:
                                    lost_packets += 1
                            
                            total_packets += 1
                            last_seq = seq
                            
                            # update stats text
                            stats_text.set_text(f'seq: {seq}, lost: {lost_packets}, total: {total_packets}')
                            
                            # update connection status
                            conn_text.set_text(f"connected to {connection_status['port']}")
                            conn_text.set_color('green')
                            break
    except (serial.SerialException, OSError) as e:
        print(f"error reading from serial port: {e}")
        conn_text.set_text(f"error: {str(e)}")
        conn_text.set_color('red')
        connection_status["connected"] = False
        return None
    
    # update the plot data
    x = np.arange(BUFFER_SIZE)
    lines[0].set_data(x, adc0_values)
    lines[1].set_data(x, adc1_values)
    
    # adjust x-axis limits
    for ax in plt.gcf().get_axes()[1:3]:  # skip the title axis
        ax.set_xlim(0, BUFFER_SIZE - 1)
    
    # we're not using blit=true anymore, so we don't need to return the artists
    return None

def console_mode(port):
    """run in console mode (no plotting) with reconnection capability."""
    global connection_status
    
    print("reading adc values (press ctrl+c to exit)...")
    print("seq\tadc0\tadc1")
    print("-" * 20)
    
    while True:
        try:
            # check if port exists before trying to connect
            if not port_exists(port):
                print(f"port {port} not available. waiting for it to appear...")
                while not port_exists(port):
                    time.sleep(RECONNECT_DELAY)
                print(f"port {port} is now available!")
            
            # open serial port
            ser = serial.Serial(port, 250000, timeout=0.1)
            print(f"connected to {port}")
            
            # update connection status
            connection_status["connected"] = True
            connection_status["port"] = port
            
            last_seq = None
            lost_packets = 0
            total_packets = 0
            
            try:
                while connection_status["connected"]:
                    try:
                        # check if port still exists
                        if not port_exists(port):
                            print(f"\nport {port} no longer exists!")
                            connection_status["connected"] = False
                            break
                        
                        # look for header byte
                        while ser.in_waiting > 0:
                            header = ser.read(1)[0]
                            if header == HEADER_BYTE:
                                # read the rest of the packet
                                data = ser.read(PACKET_SIZE - 1)
                                if len(data) == PACKET_SIZE - 1:
                                    buffer = bytearray([header]) + data
                                    seq, adc0, adc1 = unpack_adc_values(buffer)
                                    
                                    if seq is not None and adc0 is not None and adc1 is not None:
                                        # check for lost packets
                                        if last_seq is not None:
                                            expected_seq = (last_seq + 1) % 256
                                            if seq != expected_seq:
                                                lost_packets += 1
                                                print(f"lost packet(s): expected {expected_seq}, got {seq}")
                                        
                                        total_packets += 1
                                        last_seq = seq
                                        
                                        # print values
                                        print(f"{seq}\t{adc0}\t{adc1}")
                                        break
                        
                        time.sleep(0.01)  # small delay to prevent cpu hogging
                        
                    except (serial.SerialException, OSError) as e:
                        print(f"\nconnection error: {e}")
                        connection_status["connected"] = False
                        break
                
            except KeyboardInterrupt:
                print("\nexiting...")
                break
            
            # clean up
            try:
                if ser.is_open:
                    ser.close()
                    print("serial port closed")
            except:
                pass
            
            # if we're exiting by keyboard interrupt, break out of the reconnection loop
            if not connection_status["connected"] and 'KeyboardInterrupt' in locals():
                break
                
            # otherwise wait and try to reconnect
            print(f"connection lost. attempting to reconnect in {RECONNECT_DELAY} seconds...")
            time.sleep(RECONNECT_DELAY)
            print("attempting to reconnect...")
            
        except serial.SerialException as e:
            print(f"error opening serial port: {e}")
            print(f"retrying in {RECONNECT_DELAY} seconds...")
            time.sleep(RECONNECT_DELAY)
        except KeyboardInterrupt:
            print("\nexiting...")
            break
        except Exception as e:
            print(f"unexpected error: {e}")
            print(f"retrying in {RECONNECT_DELAY} seconds...")
            time.sleep(RECONNECT_DELAY)
    
    print(f"total packets: {total_packets}")
    print(f"lost packets: {lost_packets}")
    if total_packets > 0:
        print(f"packet loss rate: {lost_packets / total_packets * 100:.2f}%")

def graphical_mode(port):
    """run in graphical mode with matplotlib plotting and reconnection capability."""
    global connection_status
    
    # initialise plot
    fig, lines, title_text, stats_text, conn_text = init_plot()
    
    # serial connection variable
    ser = None
    ani = None
    
    # function to check connection status and update UI
    def check_connection_status(frame):
        nonlocal ser, ani
        
        # if we're not connected, try to connect
        if not connection_status["connected"]:
            # update connection status text
            conn_text.set_text(f"attempting to connect to {port}...")
            conn_text.set_color('yellow')
            
            # check if port exists
            if not port_exists(port):
                conn_text.set_text(f"waiting for port {port} to appear...")
                return
            
            try:
                # close previous connection if it exists
                if ser is not None and ser.is_open:
                    try:
                        ser.close()
                    except:
                        pass
                
                # open new connection
                ser = serial.Serial(port, 1*1024*1024, timeout=0.1)
                print(f"connected to {port}")
                
                # update connection status
                connection_status["connected"] = True
                connection_status["port"] = port
                conn_text.set_text(f"connected to {port}")
                conn_text.set_color('green')
                
            except serial.SerialException as e:
                print(f"error opening serial port: {e}")
                conn_text.set_text(f"error: {str(e)}")
                conn_text.set_color('red')
                time.sleep(RECONNECT_DELAY)
            except Exception as e:
                print(f"unexpected error: {e}")
                conn_text.set_text(f"error: {str(e)}")
                conn_text.set_color('red')
                time.sleep(RECONNECT_DELAY)
        
        # if we are connected, check if the connection is still valid
        else:
            try:
                # check if port still exists
                if not port_exists(port):
                    print(f"\nport {port} no longer exists!")
                    conn_text.set_text(f"disconnected: port {port} removed")
                    conn_text.set_color('red')
                    connection_status["connected"] = False
                    return
                
                # try to read from the port to check if it's still working
                try:
                    ser.read(1)  # non-blocking due to timeout
                except (serial.SerialException, OSError) as e:
                    print(f"\nconnection lost: {e}")
                    conn_text.set_text(f"disconnected: {str(e)}")
                    conn_text.set_color('red')
                    connection_status["connected"] = False
                    return
                
                # if we're connected and the connection is valid, update the plot
                if ser.in_waiting >= PACKET_SIZE:
                    update_plot_data(ser, lines, stats_text, conn_text)
                
            except Exception as e:
                print(f"error checking connection: {e}")
                conn_text.set_text(f"error: {str(e)}")
                conn_text.set_color('red')
                connection_status["connected"] = False
    
    # set up animation that checks connection status and updates plot
    ani = FuncAnimation(fig, check_connection_status, interval=50,
                       blit=False, cache_frame_data=False)
    
    # show plot (this blocks until the window is closed)
    plt.show()
    
    # cleanup
    connection_status["connected"] = False
    if ser is not None and ser.is_open:
        ser.close()
        print("serial port closed")

def update_plot_data(ser, lines, stats_text, conn_text):
    """update the plot with new data from the serial port."""
    global adc0_values, adc1_values, lost_packets, total_packets, connection_status
    last_seq = 0  # initialise last_seq
    
    try:
        # look for header byte
        while ser.in_waiting > 0:
            header = ser.read(1)[0]
            if header == HEADER_BYTE:
                # read the rest of the packet
                data = ser.read(PACKET_SIZE - 1)
                if len(data) == PACKET_SIZE - 1:
                    buffer = bytearray([header]) + data
                    seq, adc0, adc1 = unpack_adc_values(buffer)
                    
                    if seq is not None and adc0 is not None and adc1 is not None:
                        # shift values and add new ones
                        adc0_values = np.roll(adc0_values, -1)
                        adc1_values = np.roll(adc1_values, -1)
                        adc0_values[-1] = adc0
                        adc1_values[-1] = adc1
                        
                        # check for lost packets
                        if total_packets > 0:
                            expected_seq = (last_seq + 1) % 256
                            if seq != expected_seq:
                                lost_packets += 1
                        
                        total_packets += 1
                        last_seq = seq
                        
                        # update stats text
                        stats_text.set_text(f'seq: {seq}, lost: {lost_packets}, total: {total_packets}')
                        
                        # update connection status
                        conn_text.set_text(f"connected to {connection_status['port']}")
                        conn_text.set_color('green')
                        break
        
        # update the plot data
        x = np.arange(BUFFER_SIZE)
        lines[0].set_data(x, adc0_values)
        lines[1].set_data(x, adc1_values)
        
        # adjust x-axis limits
        for ax in plt.gcf().get_axes()[1:3]:  # skip the title axis
            ax.set_xlim(0, BUFFER_SIZE - 1)
            
    except (serial.SerialException, OSError) as e:
        print(f"error reading from serial port: {e}")
        connection_status["connected"] = False

def main():
    """main function."""
    # get port from command line or auto-detect
    if len(sys.argv) > 1:
        port = sys.argv[1]
    else:
        port = find_badge_port()
        if port:
            print(f"auto-detected port: {port}")
        else:
            print("no suitable port found. please connect the device and try again.")
            sys.exit(1)
    
    # check if matplotlib is available for plotting
    try:
        # use graphical mode
        graphical_mode(port)
    except ImportError:
        # fall back to console mode if matplotlib is not available
        console_mode(port)

if __name__ == "__main__":
    main()