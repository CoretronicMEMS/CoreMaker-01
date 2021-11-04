
#include "mbed.h"
#include "SensorHub.h"
#include "ADS131E.h"
#include "global.h"

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

    while(1)
    {
        ThisThread::sleep_for(chrono::milliseconds(1000));
        printf("%ld, %ld, %ld, %ld, %ld, %ld\n", adc_data[0], adc_data[1], adc_data[2], adc_data[3], adc_data[4], adc_data[5]);
    }

    return 0;
}
