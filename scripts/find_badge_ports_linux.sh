#!/bin/bash
# find_badge_ports_linux.sh - Script to find GLiTCh Badge 2025 serial ports on Linux
# 
# This script uses lsusb and udevadm to identify the USB interfaces
# of the GLiTCh Badge 2025 and their corresponding serial ports.

# ANSI colour codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

echo -e "${BOLD}${BLUE}LayerOne 2025 GLiTCh BadgE Port Finder${RESET}"
echo -e "${CYAN}=======================================${RESET}\n"

# Check if the badge is connected
echo -e "${BOLD}Checking for GLiTCh Badge 2025...${RESET}"
if ! lsusb | grep -q "1209:2025"; then
    echo -e "${RED}GLiTCh Badge 2025 not found. Please connect the device and try again.${RESET}"
    exit 1
fi

echo -e "${GREEN}GLiTCh Badge 2025 found!${RESET}\n"

# Get USB information
echo -e "${BOLD}USB Device Information:${RESET}"
lsusb -d 1209:2025 -v | grep -E "idVendor|idProduct|iManufacturer|iProduct|iSerial|bInterfaceClass|bInterfaceSubClass|bInterfaceProtocol|iInterface"

# Get serial port information
echo -e "\n${BOLD}Available Serial Ports:${RESET}"
ls -la /dev/ttyACM* 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}No ACM serial ports found.${RESET}"
fi

# Show interface to port mapping
echo -e "\n${CYAN}Interface Name -> Serial Port Mapping:${RESET}"
echo -e "${CYAN}------------------------------------${RESET}"
echo -e "${BLUE}CLI Interface (Interface 0) ->${BOLD} /dev/ttyACM0${RESET} ${YELLOW}(Command Line)${RESET}"
echo -e "${MAGENTA}Debug Interface (Interface 2) ->${BOLD} /dev/ttyACM1${RESET} ${YELLOW}(Debug Messages)${RESET}"
echo -e "${GREEN}ADC Stream Interface (Interface 4) ->${BOLD} /dev/ttyACM2${RESET} ${YELLOW}(ADC Streaming)${RESET}"

# Get detailed USB interface information
echo -e "\n${BOLD}Detailed USB Interface Information:${RESET}"
for dev in /sys/bus/usb/devices/*; do
    if [ -e "$dev/idVendor" ] && [ -e "$dev/idProduct" ]; then
        vendor=$(cat "$dev/idVendor")
        product=$(cat "$dev/idProduct")
        
        if [ "$vendor" == "1209" ] && [ "$product" == "2025" ]; then
            echo -e "${YELLOW}Found GLiTCh Badge 2025 at $dev${RESET}"
            
            # Look for interface subdirectories
            for interface in "$dev"/*:*; do
                if [ -d "$interface" ]; then
                    if [ -e "$interface/bInterfaceNumber" ]; then
                        interface_num=$(cat "$interface/bInterfaceNumber")
                        echo -e "Interface $interface_num"
                        
                        # Check if this interface has a tty device
                        for tty in "$interface"/tty*; do
                            if [ -d "$tty" ]; then
                                tty_name=$(basename "$tty")
                                echo -e "  TTY: /dev/$tty_name"
                            fi
                        done
                    fi
                fi
            done
        fi
    fi
done

echo -e "\n${BOLD}Recommended Port for ADC Streaming:${RESET}"
ls -la /dev/ttyACM2 2>/dev/null || echo -e "${RED}No port matching pattern ttyACM2 found.${RESET}"

echo -e "\n${CYAN}=======================================${RESET}"
echo -e "${BOLD}${BLUE}Use the ADC Stream Interface port with adc_stream_reader.py${RESET}"
echo -e "${CYAN}=======================================${RESET}"