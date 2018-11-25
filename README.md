# brwatch [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

brwatch ist a small and portable service tool for [B&amp;R](https://www.br-automation.com) automation controls (PLC).
It runs on Microsoft Windows and enables you to list, watch and change process variables, start and stop task, search and reboot CPUs, change IP-settings.
Additionaly it logs PV values to CSV- Files.

It makes heavy use of PVI ('process visualisation interface') package from [B&amp;R]. 

![Screenshot 1](https://github.com/hilch/brwatch/blob/master/screenshot1.PNG)
![Screenshot 2](https://github.com/hilch/brwatch/blob/master/screenshot2.PNG)
![Video](https://github.com/hilch/brwatch/blob/master/how_to_use.gif)

# Disclaimer
brwatch comes „as is“, e.g. without support and warranty.
You can freely copy it but use it at your own risk.

# PVI
brwatch needs a previously installed [PVI Development Setup](https://www.br-automation.com/en/downloads/#categories=Software/Automation+NET%2FPVI) to run.
## INA/ANSL
brwatch requires **INA** protocol enabled in the PLC. 
since V1.1/PVI4.x the new **ANSL** protocol could be an alternative.
## License
Without a PVI license **1TG0500.02** (+ TG Guard e.g. 0TG1000.02) PVI will run for two hours. After this period brwatch will stop working and PVI-Manager must be stopped and restarted again. Do not blame brwatch for that and contact your local B&R office to buy a valid license.

# Development
The project is done in plain old 'C' language (not C++) and makes use of pure Win32-API-calls.

## Compiler
   Mingw32 (32-Bit)
   http://www.mingw.org/
   
## IDE
   Code::Blocks
   http://www.codeblocks.org/
   

