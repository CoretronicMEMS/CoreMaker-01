#ifndef SGP30_H
#define SGP30_H

#include "SensorHub.h"
#include "mbed.h"

#define SGP30__DEFAULT_ADDRESS 0xb0

namespace CMC
{
    class SGP30 final : public Sensor
    {
        public:
        SGP30(uint8_t addr, I2C* i2c_dev);
        ~SGP30();
        int32_t Initialize() override final;
        int32_t Uninitialize() override final;
        int32_t Write(const void *data, size_t num) override final;
        int32_t Read(void *data, size_t num) override final;
        int32_t Control(uint32_t control, uint32_t arg) override final; 
        virtual const char* Name() { return "sgp30"; }   
        static void PrintFormatedData(int16_t *data)
        {
                printf("eCO2 %d ppm, tvo2 %d ppb\n",data[0],data[1]);
        }

        private:
        I2C *m_I2cdev;
        int32_t readdata(int16_t *data);
        int32_t SelftTest();
        const int addr =  0x58<<1;
        Ticker _send_data_ready;


    };
}
#endif



