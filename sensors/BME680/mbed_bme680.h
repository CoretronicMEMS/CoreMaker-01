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

#ifndef BME680_H
#define BME680_H

#include "bme680.h"
#include "mbed.h"
#include "SensorHub.h"

#define BME680_HUMIDITY_SCALE_VALUE 5
#define BME680_PRESSURE_SCALE_VALUE 5
#define BME680_GAS_SCALE_VALUE 10
#define BME680_CTRL_SET_OUTPUT_DATA_TYPE 0x101

#define BME680_DEFAULT_ADDRESS (0x77 << 1) // The default I2C address (shifted for MBed 8 bit address)
// #define BME680_DEBUG_MODE  // Use this for enhance debug logs for I2C and more.

namespace CMC
{
    /**
 * BME680 Class for I2C usage.
 * Wraps the Bosch library for MBed usage.
 */

    class BME680 final : public Sensor
    {
    public:
        BME680(uint8_t adr, I2C* i2c_dev, uint8_t temperature_os = BME680_OS_8X,
               uint8_t pressure_os = BME680_OS_4X, uint8_t humidity_os = BME680_OS_8X,
               uint8_t filter_size = BME680_FILTER_SIZE_3, uint16_t heater_temp = 320,
               uint16_t heater_time = 150);
        ~BME680() override;
        int32_t Initialize() override final;
        int32_t Uninitialize() override final;
        int32_t Write(const void *data, size_t num) override final;
        int32_t Read(void *data, size_t num) override final;
        int32_t Control(uint32_t control, uint32_t arg) override final;
        virtual const char* Name() { return "bme680"; }

        static void PrintFormatedData(int16_t *data)
        {
            printf("bme680_sensor_data: %.2f degC, %d Pa, %.2f %%, %d Ohm\n",(float) data[0] / 100,
                             data[1] * BME680_PRESSURE_SCALE_VALUE,
                             (float) data[2] /1000 * BME680_HUMIDITY_SCALE_VALUE ,
                             data[3] * BME680_GAS_SCALE_VALUE);
        }

    private:
        bool _filterEnabled, _tempEnabled, _humEnabled, _presEnabled, _gasEnabled;
        int32_t _sensorID;
        struct bme680_dev gas_sensor;
        struct bme680_field_data bme680_data;
        uint8_t _adr;
        uint8_t _temperature_os;
        uint8_t _pressure_os;
        uint8_t _humidity_os;
        uint8_t _filter_size;
        uint16_t _heater_temp;
        uint16_t _heater_time;
        Ticker _send_data_ready;

        int32_t SetPowerMode(uint8_t power_mode);
        int32_t ReadData(float *data, uint32_t num);
        int32_t ReadInt16Data(int16_t *data, uint32_t num );
        static void log(const char *format, ...);
        bool setTemperatureOversampling(uint8_t os);
        bool setPressureOversampling(uint8_t os);
        bool setHumidityOversampling(uint8_t os);
        bool setIIRFilterSize(uint8_t fs);
        bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);
        bool performReading();
        bool isGasHeatingSetupStable();
        int32_t SelftTest();
        // BME680 - hardware interface
        static int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
        static int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
        static void delay_msec(uint32_t ms);
    };

}

#endif
