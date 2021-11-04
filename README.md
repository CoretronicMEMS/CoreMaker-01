

## Requirements

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

## Git clone
```
git clone --recurse-submodules <repo url>
```

## Build project

### Configure and build in a single step
```
mbed-tools compile -m NUMAKER_IOT_M487 -t GCC_ARM
```

### Build the project with CMake (advanced)

```
mbed-tools configure -m NUMAKER_IOT_M487 -t GCC_ARM
cmake -S . -B cmake_build/NUMAKER_IOT_M487/develop/GCC_ARM -GNinja
cmake --build ./cmake_build/NUMAKER_IOT_M487/develop/GCC_ARM
```

## Recommend editor

### VSCode
   https://code.visualstudio.com/
   #### Extensions
   * C/C++
   * CMake

   #### VSCode configuration

   ##### tasks.json
   ```
   "tasks": [
      {
         "type": "shell",
         "label": "CMake Build",
         "command": "cmake.exe",
         "args": [
               "--build",
               "cmake_build/NUMAKER_IOT_M487/develop/GCC_ARM/"
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
   ##### settings.json
   ```
   "cmake.generator": "Ninja",
   "cmake.buildDirectory": "${workspaceFolder}/cmake_build/NUMAKER_IOT_M487/develop/GCC_ARM",
   ```

