## User Guide
1. cp 0001-Add-OPL1000-driver.patch mbed-os/
2. cd mbed-os/ && git am 0001-Add-OPL1000-driver.patch
3. cd ..
4. mbed-tools compile -m NUMAKER_IOT_M487 -t GCC_ARM
