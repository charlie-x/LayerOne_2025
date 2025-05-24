@echo off
:: find_badge_ports_windows.bat - Script to find GLiTCh Badge 2025 serial ports on Windows
:: 
:: This script uses PowerShell to identify the USB interfaces
:: of the GLiTCh Badge 2025 and their corresponding serial ports.

echo LayerOne 2025 GLiTCh BadgE Port Finder
echo =======================================
echo.

echo Checking for GLiTCh Badge 2025...
powershell -command "& {if (Get-PnpDevice | Where-Object { $_.FriendlyName -like '*GLiTCh Badge 2025*' }) { Write-Host 'GLiTCh Badge 2025 found!' -Foregroundcolour Green } else { Write-Host 'GLiTCh Badge 2025 not found. Please connect the device and try again.' -Foregroundcolour Red; exit 1 }}"
echo.

echo USB Device Information:
powershell -command "& {Get-PnpDevice | Where-Object { $_.FriendlyName -like '*GLiTCh Badge 2025*' } | Format-List}"
echo.

echo Available Serial Ports:
powershell -command "& {Get-PnpDevice -Class Ports | Format-Table -AutoSize}"
echo.

echo Interface Name -^> Serial Port Mapping:
echo ------------------------------------
echo CLI Interface (Interface 0) -^> COM3 (Command Line)
echo Debug Interface (Interface 2) -^> COM4 (Debug Messages)
echo ADC Stream Interface (Interface 4) -^> COM5 (ADC Streaming)
echo.

echo Recommended Port for ADC Streaming:
powershell -command "& {Get-PnpDevice -Class Ports | Where-Object { $_.FriendlyName -like '*COM5*' } | Format-Table -AutoSize}"
echo.

echo =======================================
echo Use the ADC Stream Interface port with adc_stream_reader.py
echo =======================================

pause