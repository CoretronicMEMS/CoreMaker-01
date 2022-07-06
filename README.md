
Menu
====

- [Board overview](#board-overview)
  - [Sensor](#sensor)
  - [LED](#led)
  - [Switch](#switch)
  - [Connector](#connector)
- [架設環境(Requirements)](#requirements) 
- [下載CoreMaker-01檔案(Git clone)](#git-clone) 
- [編譯檔案(Build project)](#build-project) 
- [Recommend editor](#recommend-editor)
  - [VSCode](#vscode)
- [Function Test](#function-test)
  - [Wifi](#wifi)
  - [SD card](#sd-card)
---

Board overview
==============

![alt text](docs/board_top_view_V1.1.png)

## Sensor

| Sensor | define         | description   |
| ------ | -------------- | ------------- |
| U1     | SPU0410LR5H-QB | Microphone    |
| U3     | GMC306A        | Geomagnetic   |
| U4     | GMP102         | Pressure      |
| U5     | BME680         | Environment   |
| U6     | KX122-103      | Accelerometer |

## LED

| LED | pin | define     |
| --- | --- | ---------- |
| D5  | PC7 | Red LED    |
| D6  | PC6 | Green LED  |
| D7  | PA5 | Yellow LED |

## Switch

| Switch | pin  | define           |
| ------ | ---- | ---------------- |
| SW1    | x    | Reset button     |
| SW2    | PA4  | PA4, user define |
| SW3_1  | X    |                  |
| SW3_2  | PF4  |                  |
| SW3_3  | PF5  |                  |
| SW3_4  | PA11 |                  |

## Connector

* J1

| pin | define  |
| --- | ------- |
| 1   | V33     |
| 2   | ICE_DAT |
| 3   | ICE_CLK |
| 4   | RESET   |
| 5   | GND     |

* J2: SD-Card

* J3

| pin | define          | pin | define          |
| --- | --------------- | --- | --------------- |
| 1   | EADC0_CH7_mikro | 2   | EPWM1_CH3_mikro |
| 3   | PC14_mikro      | 4   | PB13_mikro      |
| 5   | SPI3_SS_mikro   | 6   | UART2_RXD_mikro |
| 7   | SPI3_CLK_mikro  | 8   | UART2_TXD_mikro |
| 9   | SPI3_MISO_mikro | 10  | I2C0_SCL_mikro  |
| 11  | SPI3_MOSI_mikro | 12  | I2C0_SDA_mikro  |
| 13  | 3.3V            | 14  | 5V              |
| 15  | GND             | 16  | GND            |

* J4: USB

* J5

| pin | define       |
| --- | ------------ |
| 1   | Battery      |
| 2   | Boost_EN     |
| 3   | GND          |

* J6

| pin | define       |
| --- | ------------ |
| 1   | V5V_Battery  |
| 2   | V5V          |
| 3   | V5V_USB      |

* J7

| pin | define       |
| --- | ------------ |
| 1   | GND          |
| 2   | GND          |

* J8

| pin | define       |
| --- | ------------ |
| 1   | UART1_TXD    |
| 2   | UART1_RXD    |
| 3   | GND          |

* J11

| pin | define       |
| --- | ------------ |
| 1   | UART1_RX_ITM |
| 2   | UART1_TX_ITM |

* J15

| pin | define       |
| --- | ------------ |
| 1   | CMD_UART_TX  |
| 2   | CMD_UART_RX  |


Requirements
============

* Python 3.6 or newer.
* Pip
* CMake. Install version 3.19.0 or newer.
   * Windows: https://cmake.org/download/
   * Linux: `sudo apt install cmake`
* Ninja. Install version 1.0 or newer.
   ```
   python -m pip  install ninja
   ```
* Mbed CLI 2
   ```
   python -m pip install mbed-tools
   ```
   For more informations, please refer to https://os.mbed.com/docs/mbed-os/v6.15/build-tools/mbed-cli-2.html

* GNU Arm Embedded Toolchain
   * https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

* Programming tool: one of belows
  1. Novoton programmer
     * `NuLink` (https://direct.nuvoton.com/tw/nu-link)
     * `NuMicro_ICP_Programming_Tool`
       * https://www.nuvoton.com/support/tool-and-software/software/programmer/?__locale=zh_TW
       * Download ```NuMicro_ICP_Programming_Tool``` and install it.
  2. ARM DAPLink
     Please refer to https://os.mbed.com/docs/mbed-os/v6.15/debug-test/daplink.html
  3. Use CMC_ISP file
      * Download CoreMaker-01 and open CMC_ISP file
        [詳細內容請點此連結](https://github.com/CoretronicMEMS/CoreMaker-01/blob/master/docs/coremaker%E6%93%8D%E4%BD%9C%E6%8C%87%E5%8D%97_V1.1.pdf) 



* [詳細環境架設步驟](https://github.com/CoretronicMEMS/CoreMaker-01/blob/master/docs/coremaker%E6%93%8D%E4%BD%9C%E6%8C%87%E5%8D%97_V1.1.pdf)
   * 引導您將上述架設環境內容完成



---

Git clone
=========

```
git clone --recurse-submodules <repo url>
```
And then program with your favorite editor, like Notepad++, Sublime, VS Code, etc.

* [詳細下載步驟](https://github.com/CoretronicMEMS/CoreMaker-01/blob/master/docs/coremaker%E6%93%8D%E4%BD%9C%E6%8C%87%E5%8D%97_V1.1.pdf)
   * 引導您將上述下載步驟完成

Build project
=============

* Configure and build in a single step

   ```
   mbed-tools compile -m AIOT2101 -t GCC_ARM
   ```

* Build the project with CMake (advanced)

   ```
   mbed-tools configure -t GCC_ARM -m AIOT2101
   cmake -S . -B cmake_build/AIOT2101/develop/GCC_ARM -GNinja
   cmake --build ./cmake_build/AIOT2101/develop/GCC_ARM
   ```
   For more informations, please refer to https://os.mbed.com/docs/mbed-os/v6.15/build-tools/use.html

### Flash programming

   * Use Nuvoton Nulink
     1. Open NuMicro_ICP_Programming_Tool
     2. Select `M480 series`
     3. Select `APROM` file (*.bin or *.hex)
        ```./cmake_build/AIOT2101/develop/GCC_ARM/AIOT_2101.bin```
     4. Enable `APROM` check box
     5. Press `Start`
   * Use DAPLink
     Please refer to https://os.mbed.com/docs/mbed-os/v6.15/debug-test/daplink.html
   * Use CMC_ISP
         







*  [詳細編譯步驟](https://github.com/CoretronicMEMS/CoreMaker-01/blob/master/docs/coremaker%E6%93%8D%E4%BD%9C%E6%8C%87%E5%8D%97_V1.1.pdf)
   * 引導您將上述的編譯步驟完成
---


Recommend editor
================

## VSCode
   https://code.visualstudio.com/
   * ### Extensions
      * C/C++
      * CMake

   * ### VSCode configuration

      * #### CMake for mbed setting
      
      Add `.vscode/settings.json`
      ```json
      "cmake.generator": "Ninja",
      "cmake.buildDirectory": "${workspaceFolder}/cmake_build/AIOT2101/develop/GCC_ARM",
      ```

      * #### C/C++ intellisense

      Add `.vscode/c_cpp_properties.json`
      According to your environment to replace "compilerPath" setting.
      ``` json
      {
      "configurations": [
         {
               "name": "CortexM",
               "includePath": [
                  "${workspaceFolder}/**"
               ],
               "defines": [],
               "cStandard": "gnu17",
               "compilerPath": "D:\\projects\\gcc-arm-none-eabi-9-2020-q2-update-win32\\bin\\arm-none-eabi-gcc.exe",
               "cppStandard": "gnu++14",
               "intelliSenseMode": "linux-gcc-arm",
               "compileCommands": "cmake_build/AIOT2101/develop/GCC_ARM/compile_commands.json",
               "configurationProvider": "ms-vscode.cmake-tools"
         }
      ],
      "version": 4
      }
      ```

      * #### VSCode Tasks (Optional)

      Add `.vscode/tasks.json`
      ```json
      "tasks": [
         {
            "type": "shell",
            "label": "CMake Build",
            "command": "cmake.exe",
            "args": [
                  "--build",
                  "cmake_build/AIOT2101/develop/GCC_ARM/"
            ],
            "problemMatcher": [
                  "$gcc"
            ],
            "group": {
                  "kind": "build",
                  "isDefault": true
            },
            "detail": "編譯器: cmake"
         }
      ]
     ```

* ### [詳細Visual Studio Code 編譯環境設定步驟](https://github.com/CoretronicMEMS/CoreMaker-01/blob/master/docs/CoreMaker-01%20Visual%20Studio%20Code%20%E7%B7%A8%E8%AD%AF%E7%92%B0%E5%A2%83%E8%A8%AD%E5%AE%9A%E8%AA%AA%E6%98%8E.pdf)
   *  引導您將上述Visual Studio Code 編譯環境設定內容完成

---

Function Test
================

## WiFi
  1. Open the netassist application and start the tcp server
  2. Modify the main.cpp string to wifi_example.cpp string in CMakeLists.txt
  2. Modify the settings of wifi-ssid and wifi-password in mbed_app.json according to the router settings
  3. Convert the IP address and port on line 60 in wifi_example.cpp into the IP address and port of the tcp server
  4. Re-build image and burn image to pcb board
  5. After waiting for a period of time, confirm whether the tcp server has received the data, if so, the function is normal
  * [WIFI 運行指南](https://github.com/CoretronicMEMS/CoreMaker-01/blob/master/docs/CoreMaker-01%20wife%20%E9%81%8B%E8%A1%8C%E6%8C%87%E5%8D%97.pdf)

## SD Card
  1. Prepare a sdcard with fat32 format and plug into pcb board
  2. Modify the main.cpp string to sdcard_example.cpp string in CMakeLists.txt
  2. Re-build image and burn image to pcb board
  3. Check whether there is a file numbers.txt in the sdcard, and the content is a number from 0 to 9. If so, the sdcard function is normal
  * [SD Card 運行指南](https://github.com/frank1201/CoreMaker-01/blob/master/docs/CoreMaker-01%20Sd%20card%E9%81%8B%E8%A1%8C%E6%8C%87%E5%8D%97.pdf)
  