
Menu
====

- [Menu](#menu)
- [Requirements](#requirements)
- [Git clone](#git-clone)
- [Build project](#build-project)
    - [Configure and build in a single step](#configure-and-build-in-a-single-step)
    - [Build the project with CMake (advanced)](#build-the-project-with-cmake-advanced)
    - [Flash programming](#flash-programming)
- [Recommend editor](#recommend-editor)
  - [VSCode](#vscode)
    - [Extensions](#extensions)
    - [VSCode configuration](#vscode-configuration)
      - [CMake for mbed setting](#cmake-for-mbed-setting)
      - [C/C++ intellisense](#cc-intellisense)
      - [VSCode Tasks (Optional)](#vscode-tasks-optional)

---

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

* GNU Arm Embedded Toolchain
   * https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

* NuMicro_ICP_Programming_Tool
   * https://www.nuvoton.com/support/tool-and-software/software/programmer/?__locale=zh_TW

---

Git clone
=========

```
git clone --recurse-submodules <repo url>
```

---

Build project
=============

### Configure and build in a single step

```
mbed-tools compile -m AIOT2101 -t GCC_ARM
```

### Build the project with CMake (advanced)

```
mbed-tools configure -t GCC_ARM -m AIOT2101
cmake -S . -B cmake_build/AIOT2101/develop/GCC_ARM -GNinja
cmake --build ./cmake_build/AIOT2101/develop/GCC_ARM
```

### Flash programming

  1. Open NuMicro_ICP_Programming_Tool
  2. Select `M480 series`
  3. Select `APROM` file (*.bin or *.hex)
     ```./cmake_build/AIOT2101/develop/GCC_ARM/AIOT_2101.bin```
  4. Enable `APROM` check box
  5. Press `Start`

---

Recommend editor
================

## VSCode
   https://code.visualstudio.com/
   ### Extensions
   * C/C++
   * CMake

   ### VSCode configuration

   #### CMake for mbed setting
   
   Add `.vscode/settings.json`
   ```json
   "cmake.generator": "Ninja",
   "cmake.buildDirectory": "${workspaceFolder}/cmake_build/AIOT2101/develop/GCC_ARM",
   ```

   #### C/C++ intellisense

   Add `.vscode/c_cpp_properties.json`
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

   #### VSCode Tasks (Optional)

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