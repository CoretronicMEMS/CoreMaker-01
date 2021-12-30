#include "mbed.h"
#include "mbed_bme680.h"
#include "USBSerial.h"
#include "SensorHub.h"
#include "ADS131E.h"
#include "global.h"
#include "NuSDBlockDevice.h"
#include "FATFileSystem.h"
#include "DebounceIn.h"


using namespace CMC;

SensorHub sensorHub;
DigitalOut led_r(LED_RED);
DigitalOut led_g(LED_GREEN);
DigitalOut led_b(LED_BLUE);
DebounceIn sw2(SW2, PullUp);
DigitalIn sw3_2(SW3_2);
DigitalIn sw3_3(SW3_3);
USBCDC serial(false);

int main()
{
    led_r = 0;
    led_b = 1;
    printf("\nMbed OS version - %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    serial.connect();
    sensorHub.Start();

    while (1)
    {
        ThisThread::sleep_for(chrono::milliseconds(1000));

        // printf("%ld, %ld, %ld, %ld, %ld, %ld\n", adc_data[0], adc_data[1], adc_data[2], adc_data[3], adc_data[4], adc_data[5]);
        // printf("bme680_sensor_data:%.2f, %.2f, %.2f, %.2f\n", bme680_sensor_data[0], bme680_sensor_data[1], bme680_sensor_data[2], bme680_sensor_data[3]);
        // printf("kx122_data:%.2f, %.2f, %.2f\n", kx122_data[0], kx122_data[1], kx122_data[2]);

        serial.send((uint8_t *)"hello\r\n", 7);
    }

    return 0;
}
