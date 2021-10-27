

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

## Build
```
mbed-tools compile -m NUMAKER_IOT_M487 -t GCC_ARM
```
