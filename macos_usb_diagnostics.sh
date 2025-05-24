#!/bin/bash
# macos_usb_diagnostics.sh - Comprehensive USB diagnostic script for macOS
# 
# This script provides detailed information about USB devices, interfaces,
# serial ports, and disk devices on macOS systems.

# ANSI colour codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

echo -e "${BOLD}${BLUE}macOS USB Diagnostic Tool${RESET}"
echo -e "${CYAN}=======================================${RESET}\n"

# Function to print section headers
print_header() {
    echo -e "\n${BOLD}${MAGENTA}$1${RESET}"
    echo -e "${CYAN}$(printf '=%.0s' $(seq 1 ${#1}))${RESET}\n"
}

# Check for specific device
check_device() {
    local vendor_id=$1
    local product_id=$2
    local name=$3
    
    print_header "Checking for $name (VID=$vendor_id PID=$product_id)"
    
    if system_profiler SPUSBDataType | grep -q -E "Vendor ID: 0x$vendor_id|Product ID: 0x$product_id"; then
        echo -e "${GREEN}Device with VID=$vendor_id PID=$product_id found!${RESET}"
        system_profiler SPUSBDataType | grep -A 20 -B 5 -E "Vendor ID: 0x$vendor_id|Product ID: 0x$product_id"
    else
        echo -e "${RED}Device with VID=$vendor_id PID=$product_id not found.${RESET}"
    fi
}

# 1. List all USB devices
print_header "All USB Devices"
system_profiler SPUSBDataType

# 2. Check for GLiTCh Badge specifically
check_device "0d28" "2488" "CMSIS-DAP (GLiTCh Badge)"

# 3. List all serial ports
print_header "Available Serial Ports"
ls -la /dev/cu.* 2>/dev/null || echo -e "${RED}No serial ports found.${RESET}"

# 4. List USB interfaces
print_header "USB Interfaces"
echo -e "${YELLOW}This may take a moment...${RESET}"
ioreg -c IOUSBHostInterface -r | grep -E "class IOUSBHostInterface|idVendor|idProduct|bInterfaceNumber|bInterfaceClass|kUSBString"

# 5. List USB devices
print_header "USB Devices"
echo -e "${YELLOW}This may take a moment...${RESET}"
ioreg -c IOUSBHostDevice -r | grep -E "class IOUSBHostDevice|idVendor|idProduct|USB Product Name|USB Vendor Name|USB Serial Number"

# 6. Check for disk devices associated with USB devices
print_header "USB Mass Storage Devices"
DISK_DEVS=$(system_profiler SPUSBDataType | grep -A 1 "Media:" | grep "BSD Name:" | awk '{print $3}')
if [ -n "$DISK_DEVS" ]; then
    echo -e "${GREEN}Found USB disk devices:${RESET}"
    for disk in $DISK_DEVS; do
        echo -e "\n${BOLD}Disk: /dev/$disk${RESET}"
        diskutil info $disk | grep -E "Device Node|Volume Name|File System|Disk Size|Protocol|Mount Point"
    done
else
    echo -e "${RED}No USB disk devices found.${RESET}"
fi

# 7. Check for serial ports associated with our device
print_header "Serial Ports for CMSIS-DAP"
SERIAL_PORTS=$(ls -la /dev/cu.usbmodem* 2>/dev/null)
if [ -n "$SERIAL_PORTS" ]; then
    echo -e "${GREEN}Found serial ports:${RESET}"
    echo "$SERIAL_PORTS"
    
    # Try to match serial ports to interfaces
    echo -e "\n${YELLOW}Matching serial ports to interfaces:${RESET}"
    for port in $(echo "$SERIAL_PORTS" | awk '{print $NF}'); do
        echo -e "${CYAN}$port${RESET} - "
        port_num=$(echo "$port" | sed 's/.*usbmodem\([0-9]*\).*/\1/')
        if [[ "$port_num" == *1 ]]; then
            echo -e "  ${GREEN}Likely CLI Interface${RESET}"
        elif [[ "$port_num" == *3 ]]; then
            echo -e "  ${GREEN}Likely Debug Interface${RESET}"
        elif [[ "$port_num" == *5 ]]; then
            echo -e "  ${GREEN}Likely ADC Stream Interface${RESET}"
        else
            echo -e "  ${YELLOW}Unknown interface${RESET}"
        fi
    done
else
    echo -e "${RED}No serial ports found for CMSIS-DAP.${RESET}"
fi

# 8. Detailed interface information for specific device
print_header "Detailed Interface Information for CMSIS-DAP"
ioreg -c IOUSBHostInterface -r | grep -A 30 -B 5 "CMSIS-DAP" || echo -e "${RED}No CMSIS-DAP interfaces found.${RESET}"

# 8. Check for CDC interfaces
print_header "CDC Interfaces"
CDC_INTERFACES=$(ioreg -c IOUSBHostInterface -r | grep -A 2 -B 2 '"bInterfaceClass" = 2')
if [ -n "$CDC_INTERFACES" ]; then
    echo -e "${GREEN}Found CDC interfaces:${RESET}"
    echo "$CDC_INTERFACES"
else
    echo -e "${RED}No CDC interfaces found.${RESET}"
fi

# 9. Check for MSC interfaces
print_header "MSC Interfaces"
MSC_INTERFACES=$(ioreg -c IOUSBHostInterface -r | grep -A 2 -B 2 '"bInterfaceClass" = 8')
if [ -n "$MSC_INTERFACES" ]; then
    echo -e "${GREEN}Found MSC interfaces:${RESET}"
    echo "$MSC_INTERFACES"
else
    echo -e "${RED}No MSC interfaces found.${RESET}"
fi

echo -e "\n${CYAN}=======================================${RESET}"
echo -e "${BOLD}${BLUE}USB Diagnostic Complete${RESET}"
echo -e "${CYAN}=======================================${RESET}"