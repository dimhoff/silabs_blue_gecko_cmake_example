# Silabs Blue Gecko Example
This is an example project for Silabs BGM111/BGM113 Blue Gecko module using
CMake and GCC. It blinks a LED and has an echo service on the UART

At the moment Silabs don't officially support building with GCC for the EFR32
processor, even though Simplicity Studio will suggest to install GCC for your
hardware. Because of this it took me way to much time to get any code to
compile for the platform. To save other people from this struggle I'm sharing
this example project.

Most of the example code is based on the GCC example posted to the Silabs
community forum:
http://community.silabs.com/t5/Bluetooth-Wi-Fi-Knowledge-Base/GCC-example-BLE-SDK-2-0-test-only/ta-p/179139


**Important Notes**:
 - I do NOT give any support for this example. Please complain to Silabs if you
   have issues with the Blue Gecko platform.
 - I will NOT maintain this example. The situation regarding GCC support in
   Simplicity Studio will probably change in the future. I suggest you look
   into this first.
 - Setting CMake up for cross compiling is always a bit of a struggle. How I
   solved it in this project is probably not the ideal way. But it works for
   now...
 - Only tested under Linux

## Prerequisites
You'll need Simplicity Studio (v4) and from within Simplicity Studio you'll
need to install at least the "Bluetooth Smart SDK - v2.0.0.0" and
"GNU ARM Toolchain (v4.9.2015.q3)" packages.

## Project configuration
There are two important configuration variable available:
 - SIMPLICITY_STUDIO_ROOT: This should point to the path where Simplicity
   Studio is installed. default is: $(HOME)/opt/SimplicityStudio_v4
 - TARGET_BOARD: Specifies the radio board to use. See CMakeLists.txt for
   possible values.

## Building/Loading Firmware
To build the project run:

    # cd build/
    # cmake -DSIMPLICITY_STUDIO_ROOT=<path_to_simplicity> -DTARGET_BOARD=BRD4301A ../
    # make

To load the firmware after build run:

    # make load

## Files
### GATT database
In the gatt_db/ directory you'll find the GATT definitions.
See "UG118: Blue Gecko Bluetooth® Profile Toolkit Developer's Guide" for more
information.

The build system will automatically convert these files into a C file and
header and will link them into the firmware.

### EMDRV Config
The configuration files for the EMDRV library are available in the
emdrv_config/ directory.

Some parts of EMDRV are disabled in this project because they didn't compile.
See CMakeLists.build_emdrv.txt.

### InitDevice.c
The InitDevice.c file can be generated with the Hardware Configurator in
Simplicity Studio.


## UART
This example has a echo service on the UART. Note that by default the
SLWSTK6101B board has the VCOM connected to the radio board UART. This means
you can't use the UART RX pin on the expansion header since it is driven by the
VCOM stuff(I think...).

To use VCOM serial port under Linux just open the /dev/ttyACMx device. For
example with minicom:

    # minicom -b 115200 -D /dev/ttyACM0

Note: the example doesn't use flow control so make sure you disable all flow
control in you minicom configuration file.
