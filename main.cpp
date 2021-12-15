#include "mbed.h"
#include "mbed_bme680.h"
#include "SensorHub.h"
#include "ADS131E.h"
#include "global.h"

I2C i2c(PA_6, PA_7);
BME680 bme680(0x76 << 1);
Thread threadHub;

using namespace CMC;

// static BufferedSerial serial_port(PB_3, PB_2, 921600);
// FileHandle *mbed::mbed_override_console(int fd)
// {
//     return &serial_port;
// }

int main()
{
    printf("Mbed OS version - %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    threadHub.start(CMC::SensorHub_Task);

    if (!bme680.begin()) {
        printf("BME680 Begin failed \r\n");
        return 1;
    }

    while(1)
    {
        ThisThread::sleep_for(chrono::milliseconds(1000));
        printf("%ld, %ld, %ld, %ld, %ld, %ld\n", adc_data[0], adc_data[1], adc_data[2], adc_data[3], adc_data[4], adc_data[5]);

        if (bme680.performReading())
        {
            printf("%.2f degC,   ", bme680.getTemperature());
            printf("%.2f %%,   ", bme680.getHumidity());
            printf("%.2f hPa,   ", bme680.getPressure() / 100.0);
            printf("%.2f KOhms\r\n", bme680.getGasResistance() / 1000.0);
        }
    }

    return 0;
}
