#ifndef KX122_H
#define KX122_H

#include "SensorHub.h"

namespace CMC
{
    class KX122 final : public Sensor
    {
    public:
        KX122(SPI *spi_obj, PinName drdy_pin);
        ~KX122();
        int32_t Initialize() override final;
        int32_t Uninitialize() override final;
        int32_t Write(const void *data, uint32_t num) override final;
        int32_t Read(void *data, uint32_t num) override final;
        int32_t Control(uint32_t control, uint32_t arg) override final;
        void DRDY_ISR();

    private:
        void SetOperatingMode(void);
        void SetStandByMode(void);
        int32_t SelftTest();
        int32_t SetODR(uint32_t arg);
        void ReadData(float *buf);
        uint8_t SPIReadRegister(uint8_t reg_addr);
        void SPIWriteRegister(uint8_t reg_addr, uint8_t data);
        void ChangeBits(uint8_t reg_addr, uint8_t mask, uint8_t bits);

        SPI *spi_bus;
        uint32_t m_ODR;
        uint16_t resolution_divider;
        InterruptIn DRDY_PIN;
    };
}
#endif