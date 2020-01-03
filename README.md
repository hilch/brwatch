# brwatch
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

brwatch ist a small and portable service tool for [B&amp;R](https://www.br-automation.com) automation controls (PLC).
It runs on Microsoft Windows and enables you to list, watch and change process variables, start and stop task, search and reboot CPUs, change IP-settings.
Additionaly it logs PV values to CSV- Files.

[Download](https://github.com/hilch/brwatch/releases)

It makes heavy use of PVI ('process visualisation interface') package from [B&amp;R](https://www.br-automation.com). 

![Screenshot 1](/doc/screenshot1.PNG)
![Screenshot 2](/doc/screenshot2.PNG)
![Video](/doc/how_to_use.gif)

# Disclaimer
brwatch comes „as is“, e.g. without support and warranty.
You can freely copy it but use it at your own risk.

## PVI
brwatch needs a previously installed [PVI Development Setup](https://www.br-automation.com/en/downloads/#categories=Software/Automation+NET%2FPVI) to run.
- ### INA/ANSL
All B&R Controls support **INA** protocol for online communication, but newer plc support **ANSL**, too.
Since version 4.x PVI supports both INA and ANSL. Brwatch supports both as well but **not simultaneously**.
So starting with V1.1/PVI4.x the newer ANSL protocol could be an alternative by setting 'ANSL=1' in brwatch.ini.
In this case, of course, you will not be able to contact old AR 3.x based PLC.

- ### PVI License
Without a PVI license **1TG0500.02** (+ TG Guard e.g. 0TG1000.02) PVI will run for two hours. After this period brwatch will stop working and PVI-Manager must be stopped and restarted again. Do not blame brwatch for that and contact your local B&R office to buy a valid license.

## Usage

1. [download EXE](https://github.com/hilch/brwatch/releases) 
2. [see brief manual](https://github.com/hilch/brwatch/blob/master/manual.md)

## Development
The project is done in plain old 'C' language (not C++) and makes use of pure Win32-API-calls.
Find compiled EXE in [releases](https://github.com/hilch/brwatch/releases)

### Compiler
   Mingw32 (32-Bit)
   http://www.mingw.org/
   
### IDE
   Code::Blocks
   http://www.codeblocks.org/
   

