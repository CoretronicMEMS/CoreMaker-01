#include "mbed.h"
#include "mbed_bme680.h"
#include "USBSerial.h"
#include "SensorHub.h"
#include "ADS131E.h"
#include "global.h"
#include "NuSDBlockDevice.h"
#include "FATFileSystem.h"

Thread threadHub;

// sd card
NuSDBlockDevice bd;
FATFileSystem fs("fs");

using namespace CMC;

// static BufferedSerial serial_port(PB_3, PB_2, 921600);
// FileHandle *mbed::mbed_override_console(int fd)
// {
//     return &serial_port;
// }
int read_sd_card_txt_data(const char *filename)
{
    printf("Mounting the filesystem... ");
    int err = fs.mount(&bd);
    printf("%s\n", (err ? "Fail" : "OK"));
    if (err)
    {
        return -1;
    }
    // Display the numbers file
    printf("Opening \"%s\"... ", filename);
    FILE *f = fopen(filename, "r");
    printf("%s\n", (!f ? "Fail" : "OK"));
    if (!f)
    {
        return -2;
    }

    printf("numbers:\n");
    while (!feof(f))
    {
        int c = fgetc(f);
        printf("%c", c);
    }

    printf("\rClosing \"%s\"... ", filename);
    err = fclose(f);
    printf("%s\n", (err < 0 ? "Fail" : "OK"));
    if (err < 0)
    {
        return -3;
    }

    printf("Unmounting... ");
    err = fs.unmount();
    printf("%s\n", (err < 0 ? "Fail" : "OK"));
    if (err < 0)
    {
        return -4;
    }

    return 0;
}

int write_sd_card_txt_data(const char *filename, int *data, unsigned int len)
{
    printf("Mounting the filesystem... ");
    int err = fs.mount(&bd);
    printf("%s\n", (err ? "Fail" : "OK"));
    if (err)
    {
        return -1;
    }
    // Display the numbers file
    printf("Opening \"%s\"... ", filename);
    FILE *f = fopen(filename, "w+");
    printf("%s\n", (!f ? "Fail" : "OK"));
    if (!f)
    {
        return -2;
    }

    for (unsigned int i = 0; i < len; i++)
    {
        printf("\rWriting numbers (%d/%d)... ", i, len);
        err = fprintf(f, "%d\n", data[i]);
        if (err < 0)
        {
            printf("Fail\n");
            printf("Writing data to sdcard failed!err=%d\n", err);
            return -3;
        }
    }
    printf("\rWriting numbers (%d/%d)... OK\n", len, len);

    printf("\rClosing \"%s\"... ", filename);
    err = fclose(f);
    printf("%s\n", (err < 0 ? "Fail" : "OK"));
    if (err < 0)
    {
        return -4;
    }

    printf("Unmounting... ");
    err = fs.unmount();
    printf("%s\n", (err < 0 ? "Fail" : "OK"));
    if (err < 0)
    {
        return -5;
    }

    return 0;
}

int main()
{
    printf("\nMbed OS version - %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    USBCDC serial(false);

    threadHub.start(CMC::SensorHub_Task);

    // sd card
    int sd_card_test_data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    write_sd_card_txt_data("/fs/numbers.txt", sd_card_test_data, 10);
    read_sd_card_txt_data("/fs/numbers.txt");

    while (1)
    {
        ThisThread::sleep_for(chrono::milliseconds(1000));
        printf("%ld, %ld, %ld, %ld, %ld, %ld\n", adc_data[0], adc_data[1], adc_data[2], adc_data[3], adc_data[4], adc_data[5]);
        printf("%.2f, %.2f, %.2f, %.2f\n", bme680_sensor_data[0], bme680_sensor_data[1], bme680_sensor_data[2], bme680_sensor_data[3]);
    }

    return 0;
}
