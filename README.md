# brwatch [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

brwatch ist a small and portable debugging tool for Microsoft Windows to watch process variables („PV“) on [B&amp;R] PLCs.
It makes use of [B&amp;R](https://www.br-automation.com) PVI ('process visualisation interface'). 
Additionaly it enables to log PV values into CSV- Files.

![Screenshot 1](https://github.com/hilch/brwatch/blob/master/screenshot1.PNG)
![Screenshot 2](https://github.com/hilch/brwatch/blob/master/screenshot2.PNG)
![Video](https://github.com/hilch/brwatch/blob/master/how_to_use.gif)

# INA
brwatch requires **INA** protocol enabled. 
Currently **ANSL** is not supported.

# PVI
brwatch needs [PVI](https://www.br-automation.com/en/downloads/#categories=Software/Automation+NET%2FPVI) to run.
Beware: if you do not own a PVI license **1TG0500.02** PVI will run for two hours only. After this period one must restart it.
So do not blame brwatch for it :-)

# Development
## Compiler
   Mingw32 (32-Bit)
   http://www.mingw.org/
   
## IDE
   Code::Blocks
   http://www.codeblocks.org/
   

