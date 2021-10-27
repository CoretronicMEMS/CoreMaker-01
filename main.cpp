
#include "mbed.h"


int main()
{
    printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    DigitalOut led(LED1);

    while (true) {
        led = !led;
        ThisThread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}