#!/usr/bin/env python3
"""
logic analyser for layerone 2025 badge

this script reads and displays gpio states from the layerone 2025 badge
when it's running in logic analyser mode (adc_stream la).

usage:
    python logic_analyser.py [port]

the script will automatically detect the adc stream interface port if not specified.
"""

import serial
import serial.tools.list_ports
import time
import threading
import sys
import os
import struct
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
import argparse
import re
import subprocess
import platform

# constants
HEADER_BYTE = 0xAA
PACKET_SIZE = 5
LA_MARKER = 0x1A  # marker byte for logic analyser packets
GPIO_COUNT = 12

# data storage 
max_samples = 50000  # increased buffer size for 25khz, 10khz is about the limit for this with the packet size used.
gpio_data = {f'GP{i}': deque(maxlen=max_samples) for i in range(GPIO_COUNT)}
timestamps = deque(maxlen=max_samples)
sample_count = 0
start_time = time.time()

# performance tracking
packet_buffer = []  # batch processing buffer
last_performance_log = 0

# threading control
running = False
serial_thread = None

def find_adc_stream_port():
    """find the adc stream interface port automatically."""
    
    def find_with_system_commands():
        """use system-specific commands to find the port."""
        system = platform.system()
        
        if system == "Darwin":  # macOS
            return find_macos_port()
        elif system == "Windows":
            return find_windows_port()
        elif system == "Linux":
            return find_linux_port()
        return None

    def find_macos_port():
        """use macos system commands to find the adc stream interface port."""
        try:
            # use ioreg to find usb devices
            result = subprocess.run(['ioreg', '-p', 'IOUSB', '-l'], capture_output=True, text=True, timeout=10)
            if result.returncode == 0:
                # look for adc stream interface
                match = re.search(r'"kUSBString" = "ADC Stream Interface"', result.stdout)
                if match:
                    print("Found ADC Stream Interface in ioreg output.")
                    
                    # look for corresponding tty devices
                    for port in serial.tools.list_ports.comports():
                        # on macos, look for usbmodem ports (usually the third interface)
                        if 'usbmodem' in port.device:
                            # check if it's likely the third interface
                            port_num = port.device.split('usbmodem')[-1]
                            if port_num.endswith('5'):  # common pattern for third interface
                                print(f"Found ADC Stream Interface port using system commands: {port.device}")
                                return port.device
        except Exception as e:
            print(f"Error using macOS system commands: {e}")
        
        return "/dev/cu.usbmodem1234565"  # fallback

    def find_windows_port():
        """use windows system commands to find the adc stream interface port."""
        try:
            # use wmic to get com port information
            result = subprocess.run(['wmic', 'path', 'Win32_SerialPort', 'get', 'DeviceID,Description'], 
                                  capture_output=True, text=True, timeout=10)
            if result.returncode == 0:
                # look for COM port with ADC Stream Interface
                match = re.search(r'FriendlyName.*ADC Stream Interface.*\(COM(\d+)\)', result.stdout)
                if match:
                    port = f"COM{match.group(1)}"
                    print(f"Found ADC Stream Interface port using system commands: {port}")
                    return port
                
                # look for COM5 (common for ADC Stream Interface)
                if 'COM5' in result.stdout:
                    port = "COM5"
                    print(f"Found ADC Stream Interface port using system commands: {port}")
                    return port
        except Exception as e:
            print(f"Error using Windows system commands: {e}")
        
        return "COM5"  # fallback

    def find_linux_port():
        """use linux system commands to find the adc stream interface port."""
        try:
            # use udevadm to get device information
            result = subprocess.run(['udevadm', 'info', '--export-db'], capture_output=True, text=True, timeout=10)
            if result.returncode == 0:
                # look for adc stream interface
                lines = result.stdout.split('\n')
                for i, line in enumerate(lines):
                    if "ADC Stream Interface" in line:
                        # look backwards for the device name
                        for j in range(i-1, max(0, i-10), -1):
                            if lines[j].startswith('N: '):
                                device_name = lines[j].split(': ')[1]
                                if device_name.startswith('ttyACM'):
                                    port = f"/dev/{device_name}"
                                    print(f"Found ADC Stream Interface port using system commands: {port}")
                                    return port
                
            # look for ttyACM2 (common for ADC Stream Interface)
            if os.path.exists("/dev/ttyACM2"):
                print(f"Found ADC Stream Interface port using system commands: /dev/ttyACM2")
                return "/dev/ttyACM2"
        except Exception as e:
            print(f"Error using Linux system commands: {e}")
        
        return "/dev/ttyACM2"  # fallback

    # first try using pyserial to find ports by description
    try:
        available_ports = serial.tools.list_ports.comports()
        print(f"Found {len(available_ports)} serial ports")
        
        # first, try to find a port with "ADC Stream Interface" in the description
        for port in available_ports:
            if "ADC Stream Interface" in port.description:
                print(f"Found ADC Stream Interface by name: {port.device}")
                return port.device
                
        # group ports by USB VID/PID for multi-interface devices
        usb_groups = {}
        for port in available_ports:
            if hasattr(port, 'vid') and hasattr(port, 'pid') and port.vid and port.pid:
                key = (port.vid, port.pid, getattr(port, 'serial_number', ''))
                if key not in usb_groups:
                    usb_groups[key] = []
                usb_groups[key].append(port)
        
        # look for LayerOne badge (assuming it creates multiple interfaces)
        for group in usb_groups.values():
            if len(group) >= 3:  # Badge typically creates 3+ interfaces
                # use different strategies based on platform
                system = platform.system()
                if system == "Linux":
                    # on Linux, try to find ports with specific patterns
                    for port in group:
                        if 'ACM2' in port.device or port.device.endswith('2'):
                            print(f"Found ADC Stream interface (by pattern): {port.device}")
                            return port.device
                    
                    # if we have 3 interfaces, the third one is often the ADC Stream Interface
                    if len(group) >= 3:
                        group.sort(key=lambda p: p.device)  # Sort to get consistent ordering
                        print(f"Found ADC Stream interface (third interface): {group[2].device}")
                        return group[2].device
                        
                elif system == "Windows":
                    # first, try to find ports with "ADC Stream Interface" in the description
                    for port in group:
                        if "ADC Stream Interface" in port.description:
                            print(f"Found ADC Stream Interface by name: {port.device}")
                            return port.device
                    
                    # then, look for COM ports with specific patterns (COM5 is often the ADC Stream Interface)
                    for port in group:
                        if 'COM5' in port.device:
                            print(f"Found ADC Stream interface (by pattern): {port.device}")
                            return port.device
                    
                    # Ggoup ports by USB device and look for third interface
                    if len(group) >= 3:
                        # Sort COM ports numerically
                        group.sort(key=lambda p: int(re.findall(r'\d+', p.device)[-1]) if re.findall(r'\d+', p.device) else 0)
                        print(f"Found ADC Stream interface (third interface): {group[2].device}")
                        return group[2].device
                        
                elif system == "Darwin":  # macOS
                    # first, try to find ports with "ADC Stream Interface" in the description
                    for port in group:
                        if "ADC Stream Interface" in port.description:
                            print(f"Found ADC Stream Interface by name: {port.device}")
                            return port.device
                    
                    # then, look for ACM ports with specific patterns (ttyACM2 is often the ADC Stream Interface)
                    for port in group:
                        if 'usbmodem' in port.device and port.device.endswith('5'):
                            print(f"Found ADC Stream interface (by pattern): {port.device}")
                            return port.device
                    
                    # group ports by USB device and look for third interface  
                    if len(group) >= 3:
                        group.sort(key=lambda p: p.device)
                        print(f"Found ADC Stream interface (third interface): {group[2].device}")
                        return group[2].device
        
        # try system-specific detection as fallback
        system_port = find_with_system_commands()
        if system_port:
            return system_port
            
    except Exception as e:
        print(f"Error during automatic port detection: {e}")
    
    # fallbacks based on common patterns per platform
    system = platform.system()
    if system == "Windows":
        return "COM5"  # common Windows port for ADC streaming
    elif system == "Darwin":  # macOS
        return "/dev/cu.usbmodem1234565"  # common macOS port for ADC streaming
    else:  # Linux
        return "/dev/ttyACM2"  # common Linux port for ADC streaming

def decode_gpio_packet(data):
    """decode a 5-byte gpio state packet."""
    if len(data) != PACKET_SIZE:
        return None
        
    header, seq, gpio_low, gpio_mid, marker = struct.unpack('BBBBB', data)
    
    if header != HEADER_BYTE:
        return None
    
    # check if this is a logic analyzer packet (marker byte should be 0x1A)
    if marker != LA_MARKER:
        return None
    
    # reconstruct 12-bit GPIO states
    gpio_states = gpio_low | ((gpio_mid & 0x0F) << 8)
    
    return {
        'sequence': seq,
        'gpio_states': gpio_states,
        'timestamp': time.time()
    }

def process_packet_batch(packets):
    """process a batch of packets efficiently."""
    global sample_count, last_performance_log
    
    current_time = time.time()
    for result in packets:
        gpio_states = result['gpio_states']
        timestamp = result['timestamp'] - start_time
        
        # add timestamp
        timestamps.append(timestamp)
        
        # add GPIO states for each pin (optimized single loop)
        for i in range(GPIO_COUNT):
            gpio_data[f'GP{i}'].append(1 if (gpio_states & (1 << i)) else 0)
        
        sample_count += 1
    
    # performance logging (less frequent for high-speed)
    if current_time - last_performance_log > 2.0:  # Every 2 seconds
        if len(packets) > 0:
            rate = sample_count / (current_time - start_time)
            print(f"Received {sample_count} samples @ {rate:.0f} Hz, latest: 0x{packets[-1]['gpio_states']:03X}")
        last_performance_log = current_time

def read_serial_data(port_name):
    """thread function to read serial data - optimised for high-speed."""
    global running, packet_buffer
    
    buffer = bytearray()
    batch_packets = []
    last_batch_time = time.time()
    
    try:
        # open serial connection with optimized settings
        with serial.Serial(port_name, 115200, timeout=0.1) as ser:
            print(f"Connected to {port_name}")
            print("Reading high-speed logic analyzer data... (Ctrl+C to stop)")
            
            while running:
                # read all available data at once
                if ser.in_waiting > 0:
                    data = ser.read(ser.in_waiting)
                    buffer.extend(data)
                
                # process complete packets in batch
                packets_processed = 0
                while len(buffer) >= PACKET_SIZE and packets_processed < 100:  # Batch limit
                    # look for header byte
                    header_idx = buffer.find(HEADER_BYTE)
                    if header_idx == -1:
                        # no header found, clear buffer
                        buffer.clear()
                        break
                    
                    # remove data before header
                    if header_idx > 0:
                        buffer = buffer[header_idx:]
                    
                    # check if we have a complete packet
                    if len(buffer) >= PACKET_SIZE:
                        packet_data = bytes(buffer[:PACKET_SIZE])
                        buffer = buffer[PACKET_SIZE:]
                        
                        # ecode packet
                        result = decode_gpio_packet(packet_data)
                        if result:
                            batch_packets.append(result)
                            packets_processed += 1
                
                #pProcess batch periodically or when full
                current_time = time.time()
                if (len(batch_packets) > 50 or 
                    (len(batch_packets) > 0 and current_time - last_batch_time > 0.01)):
                    
                    process_packet_batch(batch_packets)
                    batch_packets.clear()
                    last_batch_time = current_time
                
                # ,uch smaller delay for high-speed capture
                if ser.in_waiting == 0:  # only sleep if no data waiting
                    time.sleep(0.0001)  # 100μs instead of 1ms
                
    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"Error reading serial data: {e}")
    finally:
        # parocess any remaining packets
        if len(batch_packets) > 0:
            process_packet_batch(batch_packets)

def create_logic_analyzer_plot():
    """create a logic analyser style plot."""
    fig, axes = plt.subplots(GPIO_COUNT, 1, figsize=(15, 12), sharex=True)
    if GPIO_COUNT == 1:
        axes = [axes]
    
    fig.suptitle('layerOne 2025 logic analyser', fontsize=16, fontweight='bold')
    
    # configure each subplot for a GPIO pin
    lines = []
    for i in range(GPIO_COUNT):
        ax = axes[i]
        ax.set_ylabel(f'GP{i}', rotation=0, ha='right', va='center')
        ax.set_ylim(-0.5, 1.5)
        ax.set_yticks([0, 1])
        ax.set_yticklabels(['0', '1'])
        ax.grid(True, alpha=0.3)
        
        # create line for this GPIO
        line, = ax.plot([], [], linewidth=2, drawstyle='steps-post')
        lines.append(line)
        
        # set colors for better visibility
        colors = plt.cm.tab10(np.linspace(0, 1, GPIO_COUNT))
        line.set_color(colors[i])
    
    # configure bottom axis
    axes[-1].set_xlabel('Time (seconds)')
    
    # tight layout
    plt.tight_layout()
    plt.subplots_adjust(top=0.95)
    
    return fig, axes, lines

def update_plot(frame, fig, axes, lines):
    """animation function to update the plot"""
    if len(timestamps) == 0:
        return lines
    
    # get current time window
    current_time = time.time() - start_time
    time_window = 5.0  # seconds - shorter window for high-speed
    start_time_window = max(0, current_time - time_window)
    
    # convert to numpy arrays for faster operations
    try:
        times = np.array(timestamps)
        
        # find indices within time window
        mask = times >= start_time_window
        if np.sum(mask) == 0:
            return lines
        
        windowed_times = times[mask]
        
        # update each GPIO line with optimized array operations
        for i in range(GPIO_COUNT):
            gpio_values = np.array(gpio_data[f'GP{i}'])
            if len(gpio_values) > 0:
                # ensure arrays are same length (handle race conditions)
                min_len = min(len(gpio_values), len(times))
                if min_len > 0:
                    # ise only the matching portion
                    gpio_subset = gpio_values[:min_len]
                    mask_subset = mask[:min_len]
                    
                    if np.sum(mask_subset) > 0:
                        windowed_values = gpio_subset[mask_subset]
                        windowed_times_subset = times[:min_len][mask_subset]
                        
                        if len(windowed_values) > 0:
                            lines[i].set_data(windowed_times_subset, windowed_values)
        
        # update x-axis limits
        if len(windowed_times) > 0:
            axes[0].set_xlim(windowed_times[0], windowed_times[-1])
            
        # update title with sample count and rate (less frequent updates)
        if sample_count > 0 and frame % 10 == 0:  # update title every 10th frame
            elapsed_time = current_time
            if elapsed_time > 0:
                sample_rate = sample_count / elapsed_time
                fig.suptitle(f'layerone 2025 logic analyser - {sample_count} samples @ {sample_rate:.0f} hz', 
                            fontsize=16, fontweight='bold')
    
    except (IndexError, ValueError) as e:
        # handle race conditions during high-speed data updates
        pass
    
    return lines

def print_gpio_states():
    """print current gpio states in a nice format."""
    if len(timestamps) == 0:
        return
        
    # get latest values
    latest_states = {}
    for i in range(GPIO_COUNT):
        if len(gpio_data[f'GP{i}']) > 0:
            latest_states[f'GP{i}'] = gpio_data[f'GP{i}'][-1]
        else:
            latest_states[f'GP{i}'] = 0
    
    # print in groups of 4 for readability
    print("\nCurrent GPIO States:")
    for start in range(0, GPIO_COUNT, 4):
        end = min(start + 4, GPIO_COUNT)
        state_str = " | ".join([f"GP{i:2d}:{latest_states[f'GP{i}']:1d}" for i in range(start, end)])
        print(f"  {state_str}")
    
    # print as hex
    gpio_hex = 0
    for i in range(GPIO_COUNT):
        if latest_states[f'GP{i}']:
            gpio_hex |= (1 << i)
    print(f"  Combined: 0x{gpio_hex:03X} ({gpio_hex:04b})")

def main():
    global running, serial_thread
    
    parser = argparse.ArgumentParser(description='logic analyser for layerone 2025 badge')
    parser.add_argument('port', nargs='?', help='Serial port (auto-detect if not specified)')
    parser.add_argument('--no-plot', action='store_true', help='disable plotting (text output only)')
    args = parser.parse_args()
    
    # determine port
    if args.port:
        port_name = args.port
        print(f"Using specified port: {port_name}")
    else:
        print("Auto-detecting ADC Stream Interface port...")
        port_name = find_adc_stream_port()
        print(f"Using auto-detected port: {port_name}")
    
    # start serial reading thread
    running = True
    serial_thread = threading.Thread(target=read_serial_data, args=(port_name,))
    serial_thread.daemon = True
    serial_thread.start()
    
    if not args.no_plot:
        # create and show plot
        try:
            fig, axes, lines = create_logic_analyzer_plot()
            
            # set up animation with faster updates for high-speed data
            ani = animation.FuncAnimation(fig, update_plot, fargs=(fig, axes, lines),
                                        interval=50, blit=False, cache_frame_data=False)  # 50ms = 20fps
            
            plt.show()
        except KeyboardInterrupt:
            pass
        except Exception as e:
            print(f"Plotting error: {e}")
    else:
        # text-only mode
        try:
            while True:
                time.sleep(2)
                print_gpio_states()
        except KeyboardInterrupt:
            pass
    
    # cleanup
    running = False
    if serial_thread.is_alive():
        serial_thread.join(timeout=2)
    
    print(f"\nCaptured {sample_count} samples total")

if __name__ == "__main__":
    main()