# brwatch [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

brwatch ist a small and portable service tool for [B&amp;R](https://www.br-automation.com) automation controls (PLC).
It runs on Microsoft Windows and enables you to list, watch and change process variables, start and stop task, search and reboot CPUs, change IP-settings.
Additionaly it enables to log PV values into CSV- Files.

It makes heavily use of PVI ('process visualisation interface') package from [B&amp;R]. 

![Screenshot 1](https://github.com/hilch/brwatch/blob/master/screenshot1.PNG)
![Screenshot 2](https://github.com/hilch/brwatch/blob/master/screenshot2.PNG)
![Video](https://github.com/hilch/brwatch/blob/master/how_to_use.gif)

# Disclaimer
BRWATCH comes „as is“, e.g. without support and warranty.
You can freely copy it but use it at your own risk.

# INA/ANSL
brwatch requires **INA** protocol enabled. 
since V1.1/PVI4.x the new **ANSL** protocol could be an alternative.

# PVI
brwatch needs a previously installed [PVI Development Setup](https://www.br-automation.com/en/downloads/#categories=Software/Automation+NET%2FPVI) to run.

Beware: if you do not own a PVI license **1TG0500.02** (+ TG Guard e.t. 0TG1000.02) PVI will run for two hours only. After this period brwatch will stop working and PVI-Manager must be stopped and restarted again. So, do not blame brwatch for that and contact your local B&R office to buy it.

# Development
The project is done in plain old 'C' language (not C++) and makes use of pure Win32-API-calls.

## Compiler
   Mingw32 (32-Bit)
   http://www.mingw.org/
   
## IDE
   Code::Blocks
   http://www.codeblocks.org/
   

