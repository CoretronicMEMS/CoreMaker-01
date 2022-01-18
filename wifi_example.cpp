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
#include "mbed_bme680.h"
#include "USBSerial.h"
#include "SensorHub.h"
#include "ADS131E.h"
#include "global.h"
#include "NuSDBlockDevice.h"
#include "FATFileSystem.h"
#include "DebounceIn.h"
#include "OPL1000Interface.h"

using namespace CMC;

#define TCP_MODE 1 // 1:TCP ,0: UDP

#if (TCP_MODE)
TCPSocket socket;
#else
UDPSocket socket;
#endif

char rbuffer[64];
char sbuffer[] = "1234567890";

DigitalOut wifi_en(PC_3);

ATCmdParser *_parser;
OPL1000Interface wifi(PB_15, PB_14, 0);
SocketAddress a("10.11.12.167", 8181); //TCP server address & port.

void wifi_chip_enable()
{
    wifi_en = 0;
    ThisThread::sleep_for(20);
    wifi_en = 1;

    ThisThread::sleep_for(2000); //Wait for chip reset finish.
}

int main()
{
    printf("Mbed OS version - %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    int scount = -1, rcount = -1, ret = -1;

    printf("WiFi example\n");

    wifi_chip_enable();

    ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD,
                       NSAPI_SECURITY_WPA_WPA2);
    if (ret == 0)
    {
        printf("\nMAC: %s\r\n", wifi.get_mac_address());

        ret = socket.open(&wifi);
        if (ret != 0)
        {
            printf("socket.open fail...\n");
            return -1;
        }
#if (TCP_MODE)
        ret = socket.connect(a);
#else
        ret = socket.bind(a);
#endif
        if (ret == 0)
        {
#if (TCP_MODE)
            scount = socket.send(sbuffer, sizeof sbuffer);
#else
            scount = socket.sendto(a, sbuffer, sizeof sbuffer);
#endif
            printf("sent %d [%.*s]\n", scount, strstr(sbuffer, "\r\n") - sbuffer,
                   sbuffer);
            // #if (TCP_MODE)
            //       rcount = socket.recv(rbuffer, sizeof rbuffer);
            // #else
            //       rcount = socket.recvfrom(&a, rbuffer, sizeof rbuffer);
            // #endif
            //       printf("recv %d [%.*s]\n", rcount, strstr(rbuffer, "\r\n") -
            //       rbuffer,
            //              rbuffer);
            socket.close();
        }
        else
        {
            printf("socket.connect fail, ret = %d\n", ret);
            return -2;
        }
        ret = wifi.disconnect();
        if (ret != 0)
        {
            printf("wifi.disconnect fail, ret = %d\n", ret);
            return -3;
        }
    }
    else
    {
        printf("Connect to AP failed...\r\n");
        return -4;
    }

    printf("Done\n");

    return 0;
}