/* mbed Microcontroller Library
 * Copyright (c) 2021 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

// Create a BufferedSerial object to be used by the system I/O retarget code.
// static BufferedSerial serial_port(PB_3, PB_2, 115200);

// FileHandle *mbed::mbed_override_console(int fd)
// {
//     return &serial_port;
// }

int main()
{
    printf("Mbed OS version %d.%d.%d\r\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    printf("Hello, Mbed!: %d\n");

    DigitalOut led(LED1);

    while (true) {
        led = !led;
        ThisThread::sleep_for(100);
    }

    return 0;
}