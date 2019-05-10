<h2 align="center"><br>Hardware Development</h2>
<br>

This directory contains the working development of the firmware for an M5Stack Fire integrated microcontroller device. The source code for this platform can be found [on GitHub](https://github.com/m5stack/M5Stack).

### Overview

This directory uses [PlatformIO](https://platformio.org) (as [an extension](https://docs.platformio.org/en/latest/ide/vscode.html#quick-start) to VSCode). The `platformio.ini` file is configured to build straight to device.

Please note this directory is standalone to other directories. This means you must **open this directory alone, not the project level directory in VSCode**.

1. PlatformIO needs to have the `hardware/` directory set in the workspace. This is so it can "see" the `platformio.ini` file and `src/` directory, etc.
2. The `hardware/` directory cannot be active in the workspace alongside the `MHML/` project directory. This is due to [a known bug](https://github.com/Microsoft/vscode-cpptools/issues/1073#issuecomment-460797478) with vscode-cpptools, which causes files to be ruined when formatting C++ files.

### Bluetooth Low Energy

The hardware is based on an ESP32 for which [Neil Kolban](https://github.com/nkolban) built an incredibly useful library \[[1](https://github.com/nkolban/ESP32_BLE_Arduino)\]\[[2](https://github.com/nkolban/esp32-snippets/tree/master/cpp_utils)\] with [documentation](https://github.com/nkolban/esp32-snippets/blob/master/Documentation/BLE%20C%2B%2B%20Guide.pdf) to use the BLE capabilities of the chip.

The specification used for development of the BLE functionality can be found [here](https://github.com/nebbles/MHML/blob/develop/docs/BLE_Specification.md).

