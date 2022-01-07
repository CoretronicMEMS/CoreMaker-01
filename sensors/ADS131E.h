

#ifndef ADS131E_H
#define ADS131E_H

#include "SensorHub.h"

#define ADS131E_ADC_CHANNELS            6

#define ADS131E_CTRL_SET_MUX            0x101
#define ADS131E_CTRL_GET_SAMPLE_COUNT   0x102


namespace CMC {


class ADS131E final : public Sensor
{
public:
    ADS131E(SPI *spi_dev, PinName START, PinName PWDN, PinName DRDY, int odr=32000);
    ~ADS131E() override;
    int32_t Initialize() override final;
    int32_t Uninitialize() override final;
    int32_t Write(const void *data, size_t num) override final;
    int32_t Read(void *data, size_t num) override final;
    int32_t Control(uint32_t control, uint32_t arg) override final;
    virtual const char* Name() { return "ads131e"; }

    void DRDY_ISR();
    void SPI_Done(int val);

private:
    int32_t ReadData(int32_t *data, uint32_t num);
    int32_t ReadRegisters(uint8_t reg_addr, uint8_t num, uint8_t *data);
    uint8_t ReadRegisterByte(uint8_t reg_addr);
    void WriteCommand(uint8_t cmd);
    int32_t WriteRegisters(uint8_t reg_addr, uint8_t num, uint8_t *data);
    int32_t WriteRegisterByte(uint8_t reg_addr, uint8_t data);
    int32_t SetODR(uint32_t arg);
    int32_t SetGain(uint32_t arg);
    int32_t SelftTest();

private:
    SPI *m_SPIDev;
    uint8_t m_isOn;
    uint32_t m_intCount;
    int32_t m_ADCData[ADS131E_ADC_CHANNELS];
    uint32_t m_ODR;
    uint32_t m_gain;

    DigitalOut START_PIN;
    DigitalOut PWDN_PIN;
    InterruptIn DRDY_PIN;
    EventFlags m_SPI_done;
};

}

#endif //ADS131E_H

