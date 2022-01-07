

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

