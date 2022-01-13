#include "mbed.h"
#include "mbed_bme680.h"
#include "SensorHub.h"
#include "ADS131E.h"
#include "global.h"
#include "NuSDBlockDevice.h"
#include "FATFileSystem.h"
#include "DebounceIn.h"
#include "lightEffect.hpp"


using namespace CMC;

SensorHub sensorHub;
LightEffect led_r(LED_RED);
FlashLED led_g(LED_GREEN);
FlashLED led_b(LED_BLUE);
DebounceIn sw2(SW2, PullUp);
DebounceIn sw3_2(SW3_2);
DebounceIn sw3_3(SW3_3);


int main()
{
    led_r.Off();
    printf("\nMbed OS version - %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    sensorHub.Start();

    led_r.Period(2);
    led_r.On();
    while (1)
    {
        ThisThread::sleep_for(chrono::milliseconds(1000));

    }

    return 0;
}
