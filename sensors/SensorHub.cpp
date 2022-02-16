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

#include "SensorHub.h"
#include "USBSerial.h"
#include "ADS131E.h"
#include "AcousticNode.h"
#include "mbed_bme680.h"
#include "KX122.h"
#include "GMC306.h"
#include "DebounceIn.h"
#include "lightEffect.hpp"
#include "jWrite.h"
#include "libsensiml/kb.h"
#include "libsensiml/kb_defines.h"
#include "libsensiml/kb_debug.h"

extern LightEffect led_r;
extern FlashLED led_g;
extern FlashLED led_b;
extern DebounceIn sw2;
extern DebounceIn sw3_2;
extern DebounceIn sw3_3;
extern USBSerial serial;



typedef int (*RUN_MODEL)(SENSOR_DATA_T *, int , int );

namespace CMC
{
    SPI spi0(PA_0, PA_1, PA_2, PA_3, mbed::use_gpio_ssel);
    I2C i2c1(I2C_SDA, I2C_SCL);

    AcousticNode acoustic_node(PB_6, 2000);
    BME680 bme680(0x76 << 1, &i2c1);
    GMC306 gmc306(&i2c1, 10);
    KX122 kx122(&spi0, PA_10, 3200);



    /**
     * @brief Sensor list. 
     * The arrangement must be the same with enum SensorType
     * 
     */
    Sensor *SensorHub::sensors[] =
    {
        &acoustic_node,
        &bme680,
        &kx122,
        &gmc306
    };

    RUN_MODEL run_ai_model[] = 
    {
        kb_run_model,
        NULL,
        NULL,
        NULL
    };

    SensorHub::SensorHub() : sensorEvent("sensorEvent")
    {

    }

    /**
     *@brief initialize all sensors
     *
     */
    void SensorHub::Initial()
    {
        for (uint32_t i = 0; i < ARRAY_SIZE(sensors); i++)
        {
            sensors[i]->Initialize();
            sensors[i]->SetAsyncEvent(&sensorEvent, SENSOR_EVENT(i));
        }
        // SensiML library initialization
		kb_model_init();
    }

    void SensorHub::SelectSensor(SensorType sensorId)
    {
        if(sensorId != m_SensorSel)
        {
            if (m_SensorStart)
            {
                Control(m_SensorSel, SENSOR_CTRL_STOP);
                m_DCLStatus = DCL_DISCONNECT;
            }

            m_SensorSel = sensorId;
            DBG_MSG("Sensor select %d: %s\n", m_SensorSel, sensors[m_SensorSel]->Name());
        }
    }

    SensorType SensorHub::SelectedSensor()
    {
        return m_SensorSel;
    }

    /**
     * @brief Start sensor hub thread
     * 
     */
    void SensorHub::Start()
    {
        m_thread.start(callback(this, &SensorHub::HubTask));
    }

    /**
     *@brief set the ODR of specific sensor
     *
     *@param sensor_id  sensor ID
     *@param odr        output data rate
     *@return uint32_t  ODR actually set
     */
    uint32_t SensorHub::SetODR(SensorType sensor_id, uint32_t odr)
    {
        uint32_t new_odr;
        sensors[sensor_id]->Control(SENSOR_CTRL_SET_ODR, odr);
        sensors[sensor_id]->Control(SENSOR_CTRL_GET_ODR, (uint32_t)&new_odr);

        if(odr == new_odr)
            DBG_MSG("Set %s ODR = %d OK\n", sensors[sensor_id]->Name(), (int)odr);
        else
            DBG_MSG("Set %s ODR = %d fail, odr = %d\n", sensors[sensor_id]->Name(), (int)odr, (int)new_odr);
        return new_odr;
    }

    /**
     *@brief Control specific sensor
     *
     *@param sensor_id  specifies the sensor
     *@param control    control command
     *@param arg        command related parameters
     *@return int32_t   execution result
     */
    int32_t SensorHub::Control(SensorType sensor_id, uint32_t control, uint32_t arg)
    {
        if (sensor_id < SENSOR_MAX)
        {
            switch (control)
            {
            case SENSOR_CTRL_START:
                DBG_MSG("SensorHub: sensor%d %s on\n", sensor_id, sensors[sensor_id]->Name());
                m_SensorStart = true;
                led_b = m_SensorStart;
                JsonGenerator();
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_STOP:
                DBG_MSG("SensorHub: sensor%d %s off\n", sensor_id, sensors[sensor_id]->Name());
                m_SensorStart = false;
                led_b = m_SensorStart;
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_SET_ODR:
                return SetODR(sensor_id, arg);
            case SENSOR_CTRL_SET_GAIN:
                break;
            case SENSOR_CTRL_SELFTEST:
                DBG_MSG("SensorHub_SensorTest\n");
                return sensors[sensor_id]->Control(control, 0);
            case SENSOR_CTRL_GET_ODR:
                uint32_t odr;
                sensors[sensor_id]->Control(control, (uint32_t)&odr);
                return odr;
            case SENSOR_CTRL_GET_GAIN:
            default:
                break;
            }
            return 0;
        }
        else if(sensor_id == SENSOR_TEST)
        {
            switch (control)
            {
            case SENSOR_CTRL_START:
                for(uint i=0; i<256; i++)
                    m_dataBuffer[i] = i;
                m_SensorStart = true;
                led_b = m_SensorStart;
                m_testMode = true;
                sensorEvent.set(SENSOR_EVENT(SENSOR_TEST));
                break;
            case SENSOR_CTRL_STOP:
                m_testMode = false;
                m_SensorStart = false;
                led_b = m_SensorStart;
                break;
            default:
                break;
            }
        }

        return -1;
    }

    /**
     * @brief Sensor hub event receiver thread
     * 
     */
    void SensorHub::HubTask()
    {
        while (true)
        {
            uint flags = sensorEvent.wait_any(0xFFFF, 1000);
            if (!(flags & osFlagsError))
            {
                int m_dataLen = 0;
                for(int i=0; i<SENSOR_MAX; i++)
                {
                    if (flags & SENSOR_EVENT(i))
                    {
                        m_dataLen = sensors[i]->Read(m_dataBuffer, sizeof(m_dataBuffer));
                        if(m_dataLen)
                        {
                            if(i == SENSOR_BME680)
                                bme680.PrintFormatedData(m_dataBuffer);
                            // else
                            //     PrintRawData(m_dataBuffer, m_dataLen/sizeof(short));

                            if(m_DCLStatus == DCL_CONNECTED)
                                serial.send((uint8_t*)m_dataBuffer, m_dataLen);
                            else if(run_ai_model[i])
                            {
                                int ret = run_ai_model[i]((SENSOR_DATA_T*)m_dataBuffer, m_dataLen/sizeof(short), 0);
                                if (ret > -1)
                                {
                                    printf("AI classification result: %d\n", ret);
                                    kb_reset_model(0); // Reset running model to initial state.
                                }
                                else if (ret == -2)
                                    printf("This segment has been filtered.\n");
                                else if (ret < -2)
                                    printf("AI error: %d\n", ret);
                            }
                        }
                    }
                }
                if (flags & SENSOR_EVENT(SENSOR_TEST))
                {
                    serial.send((uint8_t*)m_dataBuffer, 512);
                    if(m_testMode)
                        sensorEvent.set(SENSOR_EVENT(SENSOR_TEST));
                }

                led_g.Flash();
            }
            else if (flags == osFlagsErrorTimeout) // No event
            {
                if(m_DCLStatus == DCL_CONNECTING)
                {
                    JsonGenerator();
                    m_DCLJsonCnt++;
                    if(m_DCLJsonCnt >= 3)
                    {
                        m_DCLJsonCnt = 0;
                        m_DCLStatus = DCL_DISCONNECT;
                    }
                    led_g.Flash();
                }
            }
        }
    }

    void SensorHub::DclConnect(DCL_ConnStatus st)
    {
        if (st == DCL_CONNECTED)
        {
            m_DCLStatus = DCL_CONNECTED;
            Control(m_SensorSel, SENSOR_CTRL_START);
        }
        else if (st == DCL_CONNECTING)
        {
            JsonGenerator();
            m_DCLJsonCnt = 1;
        }
        else if (st == DCL_DISCONNECT)
        {
            if(m_testMode)
                Control(SENSOR_TEST, SENSOR_CTRL_STOP);
            else if (m_SensorStart)
                Control(m_SensorSel, SENSOR_CTRL_STOP);
            m_DCLStatus = DCL_DISCONNECT;
        }
    }

    void SensorHub::ButtonPress()
    {
        if (m_SensorStart)
        {
            Control(m_SensorSel, SENSOR_CTRL_STOP);
            m_DCLStatus = DCL_DISCONNECT;
            m_DCLJsonCnt = 0;
        }
        else if(m_DCLStatus == DCL_CONNECTING)
        {
            m_DCLStatus = DCL_DISCONNECT;
            m_DCLJsonCnt = 0;
        }
        else
        {
            if(serial.connected())
            {
                m_DCLStatus = DCL_CONNECTING;
            }
            else
            {
                Control(m_SensorSel, SENSOR_CTRL_START);
            }
        }
    }

    void SensorHub::JsonGenerator()
    {
        char json_string[1024];
        struct jWriteControl jwc;
        jwOpen(&jwc, json_string, sizeof(json_string), JW_OBJECT, JW_COMPACT); // open root node as object

        uint32_t new_odr;
        sensors[m_SensorSel]->Control(SENSOR_CTRL_GET_ODR, (uint32_t)&new_odr);

        jwObj_int(&jwc, "sample_rate", new_odr);
        jwObj_int(&jwc, "version", 1);
        if(m_SensorSel == SENSOR_ACOUSTIC_NODE)
            jwObj_int(&jwc, "samples_per_packet", 1);
        else if(m_SensorSel == SENSOR_BME680)
            jwObj_int(&jwc, "samples_per_packet", 4);
        else if(m_SensorSel == SENSOR_KX122 || m_SensorSel == SENSOR_GMC306)
            jwObj_int(&jwc, "samples_per_packet", 3);
        jwObj_object(&jwc, "column_location"); // object in the root object
        switch (m_SensorSel)
        {
        case SENSOR_ACOUSTIC_NODE:
            jwObj_int(&jwc, "Microphone", 0);
            break;
        case SENSOR_BME680:
            jwObj_int(&jwc, "Temperature", 0);
            jwObj_int(&jwc, "Pressure", 1);
            jwObj_int(&jwc, "Humidity", 2);
            jwObj_int(&jwc, "Gas resistance", 3);
            break;
        case SENSOR_KX122:
            jwObj_int(&jwc, "AccelerometerX", 0);
            jwObj_int(&jwc, "AccelerometerY", 1);
            jwObj_int(&jwc, "AccelerometerZ", 2);
            break;
        case SENSOR_GMC306:
            jwObj_int(&jwc, "MagnetX", 0);
            jwObj_int(&jwc, "MagnetY", 1);
            jwObj_int(&jwc, "MagnetZ", 2);
            break;
        default:
            break;
        }
        jwEnd(&jwc);             // end the object
        int err = jwClose(&jwc); // close root object - done
        serial.printf("%s\n", json_string);
    }

}
