# brwatch

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Made For B&R](https://github.com/hilch/BandR-badges/blob/main/Made-For-BrAutomation.svg)](https://www.br-automation.com)

[brwatch](http://hilch.github.io/brwatch) ist a small and portable service tool for [B&amp;R](https://www.br-automation.com) automation controls (PLC).
It runs on Microsoft Windows and enables you to list, watch and change process variables, start and stop task, search and reboot CPUs, change IP-settings.
Additionaly it logs PV values to CSV- Files.

[Download](https://github.com/hilch/brwatch/releases)

It makes heavy use of PVI ('process visualisation interface') package from [B&amp;R](https://www.br-automation.com).

![Screenshot 1](/doc/screenshot1.PNG)
![Screenshot 2](/doc/screenshot2.PNG)
![Video](/doc/how_to_use.gif)

## Disclaimer

brwatch comes „as is“, e.g. without support and warranty.
You can freely copy it but use it at your own risk.

## PVI

brwatch needs a previously installed [PVI Development Setup](https://www.br-automation.com/en/downloads/#categories=Software/Automation+NET%2FPVI) to run.

- ### INA/ANSL

since PVI 6.x INA2000 protocol becomes obsolete so brwatch >= 2.x is not able to handle INA anymore.

- ### PVI License

Without a PVI license **1TG0500.02** (+ TG Guard e.g. 0TG1000.02) PVI will run for two hours. After this period brwatch will stop working and PVI-Manager must be stopped and restarted again. Do not blame brwatch for that and contact your local B&R office to buy a valid license.

## Usage

1. [download EXE](https://github.com/hilch/brwatch/releases)
2. [see brief manual](http://hilch.github.io/brwatch/doc/manual.md)

([for brwatch V1.x see brief manual](http://hilch.github.io/brwatch/doc/manual-v1x.md))

## Development

The application is written in C++14 and uses the Win32 API. Bundled zlib sources remain C.
Find compiled EXE in [releases](https://github.com/hilch/brwatch/releases)

### Build from Source

#### Prerequisites

- MSYS2 with the mingw64 (x86-64) C and C++ compilers
- PVI development headers and the 64-bit import library (`libraries/PviCom64.lib`) from B&R
- The matching 64-bit PVI runtime to run the application
- Resource compiler (windres)
- make utility

#### Setup MSYS2 mingw64

1. Install [MSYS2](https://www.msys2.org/) following the official guide
2. Install required packages:

```bash
pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-binutils mingw-w64-x86_64-make mingw-w64-x86_64-winpthreads
```

For VS Code debugging, install GDB as well:

```bash
pacman -S --needed mingw-w64-x86_64-gdb
```

#### Build Instructions

**Using PowerShell Script:**

```powershell
.\build.ps1                 # Standard build
.\build.ps1 -Clean         # Clean and rebuild
.\build.ps1 -Verbose       # Show all compiler output
```

**Using Makefile:**

```bash
mingw32-make.exe -f Makefile SHELL=cmd clean all
```

**Using make in the MSYS2 MinGW 64-bit shell:**

```bash
cd /c/path/to/brwatch
mingw32-make.exe -f Makefile SHELL=cmd clean all
```

The 64-bit executable will be generated as `build/brwatch.exe`. Run a clean build
when switching from the previous 32-bit build to discard incompatible object files.

The VS Code debug build tasks define `__DEBUG__` and enable a console. Debug
launch configurations open a separate console window (not the Debug Console).
When an Ethernet CPU search starts, debug builds print the adapter count and
each adapter's description (name), IP address, and subnet mask before SNMP
discovery begins. Release builds omit this output and the console.
Use the clean build tasks when switching between debug and release builds.

Ethernet CPU searches also probe `127.0.0.1:11169` to discover local controllers,
including when no Ethernet adapter is available. Only `127.0.0.1` is probed on
the loopback network.

Every build checks Git metadata and updates the About dialog's commit hash and
the tag pointing at the current commit. Commits without a tag show `untagged`.
Builds without Git or repository metadata show `unknown` for the hash.
