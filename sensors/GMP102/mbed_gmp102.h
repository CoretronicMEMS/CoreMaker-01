#ifndef GMP102_H
#define GMP102_H

#include "gmp102.h"
#include "mbed.h"
#include "SensorHub.h"

#define GMP102_CTRL_GET_SAMPLE_COUNT 0x102

#define GMP102_DEFAULT_ADDRESS 0x6C<<1 // The default I2C address (shifted for MBed 8 bit address)

#define GMP102_PRESSURE_SCALE_VALUE 5.0f

#define GMP102_DEFAULT_ODR 256

namespace CMC
{
    class GMP102 final : public Sensor
    {
    public:
        GMP102(I2C* i2c_dev, int odr=GMP102_DEFAULT_ODR);
        ~GMP102();
        int32_t Initialize() override final;
        int32_t Uninitialize() override final;
        int32_t Write(const void *data, size_t num) override final;
        int32_t Read(void *data, size_t num) override final;
        int32_t Control(uint32_t control, uint32_t arg) override final;
        virtual const char *Name() { return "gmp102"; }
        static void PrintFormatedData(int16_t *data)
        {
            printf("gmp102_sensor_data: %d degC, %f Pa\n",data[0], (float)(data[1] * GMP102_PRESSURE_SCALE_VALUE));
        }
    
    private:
        int32_t ReadData(int16_t *data);
        int32_t SetODR(uint32_t arg);
        int32_t SelftTest();

        //GMP102 - hardware interface
        static int8_t I2CWrite(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint8_t len);
        static int8_t I2CRead(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint8_t len);

        Ticker _send_data_ready;
        int m_ODR;

        //Jeff
        bus_support_t gmp102_bus;
        float fCalibParam[GMP102_CALIBRATION_PARAMETER_COUNT];
        s16 s16Value[GMP102_CALIBRATION_PARAMETER_COUNT];
        u8 u8Power[GMP102_CALIBRATION_PARAMETER_COUNT];
    };

}
#endif