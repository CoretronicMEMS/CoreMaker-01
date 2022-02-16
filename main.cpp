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
#include "lightEffect.hpp"


using namespace CMC;

SensorHub sensorHub;
LightEffect led_r(LED_RED);
FlashLED led_g(LED_GREEN);
FlashLED led_b(LED_BLUE);
DebounceIn sw2(SW2, PullUp);
DebounceIn sw3_2(SW3_2);
DebounceIn sw3_3(SW3_3);
USBSerial serial(false);
EventFlags mainEvent;
char serialRxBuf[100];
uint serialRxLen = 0;

void SW2PressISR();
void SW3_ISR();
void SwitchChanged();
void SerialReceiveISR();
void onSerialReceived();
void CmdHandler();
int GetSwitchSelect();


#define SW2_EVENT       0x8000
#define SW3_EVENT       0x4000
#define UART_EVENT      0x2000


int main()
{
    printf("\nMbed OS version - %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    serial.connect();
    serial.attach(SerialReceiveISR);

    sw2.fall(SW2PressISR);
    sw3_2.fall(SW3_ISR);
    sw3_2.rise(SW3_ISR);
    sw3_3.fall(SW3_ISR);
    sw3_3.rise(SW3_ISR);

    sensorHub.Initial();
    sensorHub.SelectSensor((SensorType)GetSwitchSelect());
    sensorHub.Start();

    led_r.Period(2);
    led_r.On();
    while (1)
    {
        uint flags = mainEvent.wait_any(0xFFFF, 1000);
        if (!(flags & osFlagsError))
        {
            if (flags & SW3_EVENT)
                SwitchChanged();
            if (flags & SW2_EVENT)
                sensorHub.ButtonPress();
            if (flags & UART_EVENT)
                onSerialReceived();
        }
        else if (flags == osFlagsErrorTimeout) // No event
        {
        }
    }

    return 0;
}

int GetSwitchSelect()
{
    int sw_sel;
    sw_sel = (sw3_2<<0) | (sw3_3<<1);
    return sw_sel;
}

void SW2PressISR()
{
    mainEvent.set(SW2_EVENT);
}

void SW3_ISR()
{
    mainEvent.set(SW3_EVENT);
}

void SwitchChanged()
{
    SensorType newSel = (SensorType)GetSwitchSelect();
    sensorHub.SelectSensor(newSel);
}

void SerialReceiveISR()
{
    mainEvent.set(UART_EVENT);
}

void onSerialReceived()
{
    if(!serial.readable())
        return;

    led_g.Flash();
    while(serial.readable())
    {
        if(serialRxLen < sizeof(serialRxBuf)-1)
            serialRxLen += serial.read(serialRxBuf + serialRxLen, 1);
    }
    if(serialRxBuf[serialRxLen-1] == '\n' || serialRxLen == sizeof(serialRxBuf)-1 || strncmp(serialRxBuf, "connect", 7) == 0)
    {
        serialRxBuf[serialRxLen] = 0;
        CmdHandler();
        serialRxLen = 0;
    }
}

void CmdHandler()
{
    printf("%s", serialRxBuf);

    if (strncmp(serialRxBuf, "connect", 7) == 0)
    {
        sensorHub.DclConnect(DCL_CONNECTED);
    }
    else if (strncmp(serialRxBuf, "disconnect", 10) == 0)
    {
        sensorHub.DclConnect(DCL_DISCONNECT);
    }
    else if (strncmp(serialRxBuf, "list", 4) == 0)
    {
        sensorHub.JsonGenerator();
    }
    else if (strncmp(serialRxBuf, "test", 4) == 0)
    {
        sensorHub.Control(SENSOR_TEST, SENSOR_CTRL_START);
    }
    else if (strncmp(serialRxBuf, "stop", 4) == 0)
    {
        sensorHub.DclConnect(DCL_DISCONNECT);
    }
    else if (strncmp(serialRxBuf, "sensor=", 7) == 0)
    {
        int id = atoi(serialRxBuf+7);
        if(id >= 0 && id < SENSOR_MAX)
        {
            sensorHub.SelectSensor((SensorType)id);
        }
    }
    else if (strncmp(serialRxBuf, "odr=", 4) == 0)
    {
        int odr = atoi(serialRxBuf+4);
        if(odr >= 0 && odr < 999999)
        {
            sensorHub.Control(sensorHub.SelectedSensor(), SENSOR_CTRL_SET_ODR, odr);
        }
    }
}