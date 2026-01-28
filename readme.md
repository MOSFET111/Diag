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

## CLI Commands
- Usage:
- - Diag.exe [options];

- Options:
- - -h, --help           Show the help message
- -   --out <path>         Output JSON path (default: report.json)
- -   --log-echo           Echo logs to console (default: on)
- -   --no-log-echo        Disable console log echo

- Examples:
- -   Diag.exe --out out/report.json
- -   Diag.exe --no-log-echo


## Build
```bash
cmake -S . -B build
cmake --build build