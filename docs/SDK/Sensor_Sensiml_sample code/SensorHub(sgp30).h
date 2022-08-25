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

#ifndef CMC_SENSOR_HUB_H
#define CMC_SENSOR_HUB_H

#include "mbed.h"
#include "global.h"


#define ENABLE_DEBUG_MSG

#ifdef ENABLE_DEBUG_MSG
#define DBG_MSG(...)       printf( __VA_ARGS__)
#else
#define DBG_MSG(...)
#endif


namespace CMC {

#define SENSOR_CTRL_START              0x01
#define SENSOR_CTRL_STOP               0x02
#define SENSOR_CTRL_SET_ODR            0x03
#define SENSOR_CTRL_SET_GAIN           0x04
#define SENSOR_CTRL_SELFTEST           0x05
#define SENSOR_CTRL_GET_ODR            0x13
#define SENSOR_CTRL_GET_GAIN           0x14

#define SENSORHUB_CTRL_TESTMODE        0x101
#define SENSORHUB_CTRL_STOPTEST        0x102


enum SensorType
{
    SENSOR_ACOUSTIC_NODE,
    SENSOR_BME680,
    SENSOR_KX122,
    SENSOR_GMC306,
    SENSOR_GMP102,
    SENSOR_SGP30,
    SENSOR_MAX,
    SENSOR_TEST,
    SENSOR_HUB,
};

enum DCL_ConnStatus
{
    DCL_DISCONNECT,
    DCL_CONNECTING,
    DCL_CONNECTED,
};

#define SENSOR_EVENT(id)    (1<<id)

class Sensor {

public:
    Sensor() : m_eventHande(NULL), m_flags(0) {};
    virtual ~Sensor() = default;
    virtual int32_t Initialize() = 0;
    virtual int32_t Uninitialize() = 0;
    virtual int32_t Write(const void *data, size_t num) = 0;
    /**
     * @brief read sensor data
     * 
     * @param data data buffer
     * @param num data buffer length in byte
     * @return int32_t received data length in byte
     */
    virtual int32_t Read(void *data, size_t num) = 0;
    virtual int32_t Control(uint32_t control, uint32_t arg) = 0;
    virtual const char* Name() { return ""; }

    void SetAsyncEvent(EventFlags *eventHandle, uint32_t flags)
    {  
        m_eventHande = eventHandle;
        m_flags = flags;
    }

protected:
    void SetDataReady()
    {
        if(m_eventHande)
        {
            m_eventHande->set(m_flags);
        }
    }
    void ClearDataReady()
    {
        if(m_eventHande)
            m_eventHande->clear(m_flags);
    }


    EventFlags *m_eventHande;
    uint32_t m_flags;
};

class SensorHub {
public:
    SensorHub();
    virtual ~SensorHub() = default;

    void Initial();
    void Start();
    int32_t Control(SensorType sensor_id, uint32_t control, uint32_t arg = 0);
    uint32_t SetODR(SensorType sensor_id, uint32_t odr);
    void DclConnect(DCL_ConnStatus st);
    void JsonGenerator();
    void ButtonPress();
    void SelectSensor(SensorType sensorId);
    SensorType SelectedSensor();
    static void PrintRawData(int16_t *data, int len)
    {
        printf("Raw: ");
        for(int i = 0; i<len; i++)
            printf("%d ", data[i]);
        printf("\n");
    }

    static Sensor* sensors[];
   
protected:
    void HubTask();

    Thread m_thread;
    EventFlags sensorEvent;
    EventFlags m_SwEvent;
    SensorType m_SensorSel = SENSOR_MAX;
    bool m_SensorStart = 0;
    DCL_ConnStatus m_DCLStatus = DCL_DISCONNECT;
    int m_DCLJsonCnt = 0;
    int16_t m_dataBuffer[1024];
    bool m_testMode = false;
};


};

#endif
