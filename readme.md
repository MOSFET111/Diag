# Diag

A portable Windows diagnostic and hardware inspection tool written in C++.

## Features
- Hardware serial number collection
- CPU, RAM, disk, GPU, and battery information
- Disk health indicators (SMART – planned)
- Export reports in JSON, CSV, HTML, and TXT
- Portable execution (USB-friendly)

## Platform
- Windows 10 / 11
- Administrator privileges required

## Build
```bash
cmake -S . -B build
cmake --build build