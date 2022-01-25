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

#include "mbed_bme680.h"

namespace CMC
{
    I2C* _i2c_dev;

    BME680::BME680(uint8_t adr, I2C* i2c_dev, uint8_t temperature_os,
                   uint8_t pressure_os, uint8_t humidity_os,
                   uint8_t filter_size, uint16_t heater_temp,
                   uint16_t heater_time) : _adr(adr),
                                           _temperature_os(temperature_os), _pressure_os(pressure_os),
                                           _humidity_os(humidity_os), _filter_size(filter_size),
                                           _heater_temp(heater_temp), _heater_time(heater_time)
    {
        _filterEnabled = _tempEnabled = _humEnabled = _presEnabled = _gasEnabled = false;
        _i2c_dev = i2c_dev;
    }

    BME680::~BME680()
    {
    }

    int32_t BME680::Initialize()
    {
        int8_t result;

        gas_sensor.dev_id = _adr;
        gas_sensor.intf = BME680_I2C_INTF;
        gas_sensor.read = &BME680::i2c_read;
        gas_sensor.write = &BME680::i2c_write;
        gas_sensor.delay_ms = BME680::delay_msec;
        gas_sensor.power_mode = BME680_FORCED_MODE;

        setTemperatureOversampling(_temperature_os);
        setPressureOversampling(_pressure_os);
        setHumidityOversampling(_humidity_os);
        setIIRFilterSize(_filter_size);
        setGasHeater(_heater_temp, _heater_time); // 320*C for 150 ms

        result = bme680_init(&gas_sensor);

        if (result != BME680_OK)
            return -1;

        DBG_MSG("%s initialized\n", Name());

        return 0;
    }

    int32_t BME680::Uninitialize()
    {
        return 0;
    }

    int32_t BME680::Write(const void *data, size_t num)
    {
        return 0;
    }

    int32_t BME680::Read(void *data, size_t num)
    {
        if(bme680_get_data_type() == 0)
            return ReadInt16Data((int16_t *)data, num);
        else
            return ReadData((float *)data, num);
    }

    int32_t BME680::Control(uint32_t control, uint32_t arg)
    {
        if (control == SENSOR_CTRL_START)
        {
            int32_t ret = SetPowerMode(BME680_FORCED_MODE);
            if (ret == 0)
            {
                _send_data_ready.attach(callback(this, &BME680::SetDataReady), chrono::milliseconds(1000));
            }

            return ret;
        }
        else if (control == SENSOR_CTRL_STOP)
        {
            int32_t ret = SetPowerMode(BME680_SLEEP_MODE);
            if (ret == 0)
            {
                _send_data_ready.detach();
            }
            return ret;
        }
        else if (control == SENSOR_CTRL_SET_ODR)
        {
        }
        else if (control == SENSOR_CTRL_SELFTEST)
        {
            return SelftTest();
        }
        else if (control == SENSOR_CTRL_GET_ODR)
        {
            *((uint32_t *)arg) = 1; // only support 1Hz
        }
        else if (control == SENSOR_CTRL_SET_GAIN)
        {
        }
        else if(control == BME680_CTRL_SET_OUTPUT_DATA_TYPE)
        {
            bme680_set_data_type(arg);
        }

        return 0;
    }

    int32_t BME680::ReadData(float *data, uint32_t num)
    {
        if (performReading())
        {
            data[0] = bme680_data.f_temperature;
            data[1] = bme680_data.f_pressure;
            data[2] = bme680_data.f_humidity;
            if (this->isGasHeatingSetupStable())
            {
                data[3] = bme680_data.f_gas_resistance;
            }
            else
            {
                data[3] = 0;
            }

            return 4;
        }

        return -1;
    }

    int32_t BME680::ReadInt16Data(int16_t *data, uint32_t num )
    {
        if (performReading())
        {
            data[0] = bme680_data.temperature;
            data[1] = ((bme680_data.pressure) / BME680_PRESSURE_SCALE_VALUE) > INT16_MAX? INT16_MAX : ((bme680_data.pressure) / 5); //change range to int16_t
            data[2] = ((bme680_data.humidity) / BME680_HUMIDITY_SCALE_VALUE) > INT16_MAX? INT16_MAX : (bme680_data.humidity) / 5 ; //change range to int16_t
            if (this->isGasHeatingSetupStable())
            {
                data[3] =((bme680_data.gas_resistance) / BME680_GAS_SCALE_VALUE) > INT16_MAX? INT16_MAX : ((bme680_data.gas_resistance) / 10); //change range to int16_t
            }
            else
            {
                data[3] = 0;
            }

            return 4;
        }

        return -1;
    }

    int32_t BME680::SetPowerMode(uint8_t power_mode)
    {
        int8_t result;
        gas_sensor.power_mode = power_mode;
        /* Set the power mode */
        result = bme680_set_sensor_mode(&gas_sensor);

        if (result != BME680_OK)
        {
            return -1;
        }

        return 0;
    }

    /**
 * Performs a full reading of all 4 sensors in the BME680.
 * Assigns the internal BME680#temperature, BME680#pressure, BME680#humidity and BME680#gas_resistance member variables
 * @return True on success, False on failure
 */
    bool BME680::performReading(void)
    {
        uint8_t set_required_settings = 0;
        int8_t result;

        /* Select the power mode */
        /* Must be set before writing the sensor configuration */
        // gas_sensor.power_mode = BME680_FORCED_MODE;

        /* Set the required sensor settings needed */
        if (_tempEnabled)
            set_required_settings |= BME680_OST_SEL;
        if (_humEnabled)
            set_required_settings |= BME680_OSH_SEL;
        if (_presEnabled)
            set_required_settings |= BME680_OSP_SEL;
        if (_filterEnabled)
            set_required_settings |= BME680_FILTER_SEL;
        if (_gasEnabled)
            set_required_settings |= BME680_GAS_SENSOR_SEL;

        /* Set the desired sensor configuration */
        result = bme680_set_sensor_settings(set_required_settings, &gas_sensor);
        log("Set settings, result %d \r\n", result);
        if (result != BME680_OK)
            return false;

        /* Set the power mode */
        result = bme680_set_sensor_mode(&gas_sensor);
        log("Set power mode, result %d \r\n", result);
        if (result != BME680_OK)
            return false;

        /* Get the total measurement duration so as to sleep or wait till the
     * measurement is complete */
        uint16_t meas_period;
        bme680_get_profile_dur(&meas_period, &gas_sensor);

        /* Delay till the measurement is ready */
        delay_msec(meas_period);

        result = bme680_get_sensor_data(&bme680_data, &gas_sensor);
        log("Get sensor data, result %d \r\n", result);
        if (result != BME680_OK)
            return false;

        return true;
    }

    bool BME680::isGasHeatingSetupStable()
    {
        if (bme680_data.status & BME680_HEAT_STAB_MSK)
        {
            return true;
        }

        return false;
    }

    /**
 * Enable and configure gas reading + heater
 * @param heaterTemp Desired temperature in degrees Centigrade
 * @param heaterTime Time to keep heater on in milliseconds
 * @return True on success, False on failure
 */
    bool BME680::setGasHeater(uint16_t heaterTemp, uint16_t heaterTime)
    {
        gas_sensor.gas_sett.heatr_temp = heaterTemp;
        gas_sensor.gas_sett.heatr_dur = heaterTime;

        if ((heaterTemp == 0) || (heaterTime == 0))
        {
            // disabled!
            gas_sensor.gas_sett.run_gas = BME680_DISABLE_GAS_MEAS;
            _gasEnabled = false;
        }
        else
        {
            gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
            _gasEnabled = true;
        }
        return true;
    }

    /**
 * Setter for Temperature oversampling
 * @param oversample Oversampling setting, can be BME680_OS_NONE (turn off Temperature reading),
 * BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
 * @return True on success, False on failure
 */
    bool BME680::setTemperatureOversampling(uint8_t oversample)
    {
        if (oversample > BME680_OS_16X)
            return false;

        gas_sensor.tph_sett.os_temp = oversample;

        if (oversample == BME680_OS_NONE)
            _tempEnabled = false;
        else
            _tempEnabled = true;

        return true;
    }

    /**
 * Setter for Humidity oversampling
 * @param oversample Oversampling setting, can be BME680_OS_NONE (turn off Humidity reading),
 * BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
 * @return True on success, False on failure
 */
    bool BME680::setHumidityOversampling(uint8_t oversample)
    {
        if (oversample > BME680_OS_16X)
            return false;

        gas_sensor.tph_sett.os_hum = oversample;

        if (oversample == BME680_OS_NONE)
            _humEnabled = false;
        else
            _humEnabled = true;

        return true;
    }

    /**
 * Setter for Pressure oversampling
 * @param oversample Oversampling setting, can be BME680_OS_NONE (turn off Humidity reading),
 * BME680_OS_1X, BME680_OS_2X, BME680_OS_4X, BME680_OS_8X or BME680_OS_16X
 * @return True on success, False on failure
 */
    bool BME680::setPressureOversampling(uint8_t oversample)
    {
        if (oversample > BME680_OS_16X)
            return false;

        gas_sensor.tph_sett.os_pres = oversample;

        if (oversample == BME680_OS_NONE)
            _presEnabled = false;
        else
            _presEnabled = true;

        return true;
    }

    /**
 * Setter for IIR filter.
 * @param filter_seize Size of the filter (in samples).
 * Can be BME680_FILTER_SIZE_0 (no filtering), BME680_FILTER_SIZE_1, BME680_FILTER_SIZE_3, BME680_FILTER_SIZE_7,
 * BME680_FILTER_SIZE_15, BME680_FILTER_SIZE_31, BME680_FILTER_SIZE_63, BME680_FILTER_SIZE_127
 * @return True on success, False on failure
 */
    bool BME680::setIIRFilterSize(uint8_t filter_seize)
    {
        if (filter_seize > BME680_FILTER_SIZE_127)
            return false;

        gas_sensor.tph_sett.filter = filter_seize;

        if (filter_seize == BME680_FILTER_SIZE_0)
            _filterEnabled = false;
        else
            _filterEnabled = true;

        return true;
    }

    /**
 * Reads 8 bit values over I2C
 * @param dev_id Device ID (8 bits I2C address)
 * @param reg_addr Register address to read from
 * @param reg_data Read data buffer
 * @param len Number of bytes to read
 * @return 0 on success, non-zero for failure
 */
    int8_t BME680::i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
    {
        int8_t result;
        char data[1];

        data[0] = (char)reg_addr;

        log("[0x%X] I2C $%X => ", dev_id >> 1, data[0]);

        result = _i2c_dev->write(dev_id, data, 1);
        log("[W: %d] ", result);

        result = _i2c_dev->read(dev_id, (char *)reg_data, len);

        for (uint8_t i = 0; i < len; i++)
            log("0x%X ", reg_data[i]);

        log("[R: %d, L: %d] \r\n", result, len);

        return result;
    }

    /**
 * Writes 8 bit values over I2C
 * @param dev_id Device ID (8 bits I2C address)
 * @param reg_addr Register address to write to
 * @param reg_data Write data buffer
 * @param len Number of bytes to write
 * @return 0 on success, non-zero for failure
 */
    int8_t BME680::i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
    {
        int8_t result;
        char data[len + 1];

        data[0] = (char)reg_addr;

        for (uint8_t i = 1; i < len + 1; i++)
        {
            data[i] = (char)reg_data[i - 1];
        }

        log("[0x%X] I2C $%X <= ", dev_id >> 1, data[0]);

        result = _i2c_dev->write(dev_id, data, len + 1);

        for (uint8_t i = 1; i < len + 1; i++)
            log("0x%X ", data[i]);

        log("[W: %d, L: %d] \r\n", result, len);

        return result;
    }

    void BME680::delay_msec(uint32_t ms)
    {
        log(" * wait %d ms ... \r\n", ms);
        wait_ns(ms * 1000000);
    }

    void BME680::log(const char *format, ...)
    {
#ifdef BME680_DEBUG_MODE
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
#endif
    }

    int32_t BME680::SelftTest()
    {
        float data[4];
        ReadData(data, 4);
        printf("%.2f,%.2f,%.2f,%.2f\n", data[0], data[1], data[2], data[3]);

        return 1;
    }

}; //namespace CMC
