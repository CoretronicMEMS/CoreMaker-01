/* 
 * MIT License
 *
 * Copyright (c) 2022 CoretronicMEMS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

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

int main()
{
    printf("Mbed OS version - %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    // sd card
    int sd_card_test_data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    write_sd_card_txt_data("/fs/numbers.txt", sd_card_test_data, 10);
    read_sd_card_txt_data("/fs/numbers.txt");

    return 0;
}

