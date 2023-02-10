# Spot-Welder
Spot welder on PIC12F675.

### Folders and files:

    Hard       Electronic circuits and printed circuit boards for PCAD, as well as printable files in pdf format.
    Pictures   Photos taken during the manufacture of the device.
    Soft       Files with C program (MPLAB Xpress & XC8 compiler) and firmware for PIC12F675.
    README.md  This file.
---
### How it all started:

It all started with the fact that they brought in for repair a cheap Chinese welding machine assembled on five MOSFET 4N04S4.
I drew a diagram on the circuit board, found the cause of the malfunction (one of the transistors was broken).
Replaced, checked, took oscillograms. The device has 3 modes of operation (duration of the output pulse of welding 7, 11, 15 ms), the indication of which is implemented on a three-color LED.
After a while, I needed to connect 18650 batteries for PowerBank, so I decided to make my own welding machine, like the Chinese, but with minor modifications.

The element base got out of the principle of what is available, I didn’t want to buy anything.

PIC12F675 was chosen as a microprocessor, for mode indication I took a 7-segment LED and a KР514ID2 (CD4511) decoder, a TC4429 driver, MOSFET transistors AUFS8409-7P.
    
### Description:

The device is powered by a 12V car battery and has reverse polarity protection.

![Control circuit](https://github.com/nva1773/Spot-Welder/blob/main/Pictures/spot-welder-control.JPG)

![Power circuit](https://github.com/nva1773/Spot-Welder/blob/main/Pictures/spot-welder-power.JPG)

Implemented 8 modes of operation, 0 - the output of the welding pulse is disabled, 1 - 7 the output of the welding pulse from 7 to 24 ms with a step of 3 ms.
The welding process can be started in automatic mode or manually (selected by jumper on JP1).
For automatic start, just touch the electrodes to the parts to be welded and after 500 ms a welding pulse will be emitted. If you continue to press the electrodes, then a second pulse will be given after 1s.
In manual mode, the start of the welding pulse is given by pressing and holding an external start button, such as a foot pedal.

### Making:

I spread the circuit and the printed circuit board in PCAD, making a seal using a simple "ironing" method - we print on chalk paper using a laser printer, transfer the toner to foil textolite with an iron, etch (for example, in ferric chloride), clean it, drill and tin:

![Making of the control board](https://github.com/nva1773/Spot-Welder/blob/main/Pictures/pcb-build-1.jpg)

We solder the parts and apply a voltage, without a microprocessor, the number 7 should be on the display::

![Making of the power board](https://github.com/nva1773/Spot-Welder/blob/main/Pictures/pcb-build-2.jpg)

### Getting firmware:

The program was written in Microchip's free development environment [MPLAB Xpress](https://mplabxpress.microchip.com/mplabcloud/ide) in C and compiled with the free XC8 compiler. The process of creating a project is very simple, register on the site, press the "New project" button, select the "Standalone project" project, press "Next", select the CPU by entering PIC12F675 in the "Device" field, press "Next", enter the project name (for example: Spot -Welder) and click "Finish". In the project properties, select the "XC8" compiler. Import the Header (.h) and Source (.c) files and build the project by clicking "Build Project". After compilation, click "Make and Program Device", as a result of which the firmware file (spot-welder.hex) will be downloaded to the computer.

![MPLAB Xpress Project](https://github.com/nva1773/Spot-Welder/blob/main/Pictures/www.mplab-xpresside.micorchip.com.JPG)

To automate the receipt of output data for the CPU (delay in issuing a control pulse), a table [Excel file](https://github.com/nva1773/Spot-Welder/blob/main/Soft/DurationPulse.xlsm) was created with a macro, which is needed to simplify the receipt of data in the form of a list of values in text form. The data is copied into the "table_pulse" array of the spotwelder.c file.
