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

#ifndef GMC306_H
#define GMC306_H

#include "SensorHub.h"

#define GMC306_ADC_CHANNELS     3
#define GMC306_DEFAULT_ODR      10

namespace CMC {


class GMC306 final : public Sensor
{
public:
    GMC306(I2C *i2c_dev, int odr = GMC306_DEFAULT_ODR);
    ~GMC306() override;
    int32_t Initialize() override final;
    int32_t Uninitialize() override final;
    int32_t Write(const void *data, size_t num) override final;
    int32_t Read(void *data, size_t num) override final;
    int32_t Control(uint32_t control, uint32_t arg) override final;
    virtual const char* Name() { return "gmc306"; }

private:
    int32_t ReadData(int16_t *data, size_t num);
    int32_t ReadRegisters(uint8_t reg_addr, uint8_t num, uint8_t *data);
    uint8_t ReadOneRegister(uint8_t reg_addr);
    int32_t WriteRegisters(uint8_t reg_addr, uint8_t num, const uint8_t *data);
    int32_t WriteOneRegister(uint8_t reg_addr, uint8_t data);
    int32_t SetODR(uint32_t arg);
    int32_t SelftTest();
    void TimerCallback();

private:
    I2C *m_I2CDev;
    bool m_isOn = false;
    int16_t m_SensorData[GMC306_ADC_CHANNELS];
    Ticker m_timer;
    int m_PID;
    int m_ODR;
    uint8_t m_OdrReg;
};

}

#endif //GMC306_H

