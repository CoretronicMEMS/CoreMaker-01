
#include "mbed.h"
#include "NuSDBlockDevice.h"
#include "FATFileSystem.h"


// sd card
NuSDBlockDevice bd;
FATFileSystem fs("fs");

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
