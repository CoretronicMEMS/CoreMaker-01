#include "mbed_gmp102.h"
#include "global.h"

#define DEFAULT_ODR 256

namespace CMC
{
    I2C* _i2c_dev102;

    struct Config_to_Reg
    {
        uint32_t config;
        uint8_t reg_value;
    };

    const static Config_to_Reg gmp102_odr_list[] = 
    {
        {256, GMP102_P_OSR_256},
        {512, GMP102_P_OSR_512},
        {1024, GMP102_P_OSR_1024},
        {2048, GMP102_P_OSR_2048},
        {4096, GMP102_P_OSR_4096},
        {8192, GMP102_P_OSR_8192},
        {16384, GMP102_P_OSR_16384},
        {32768, GMP102_P_OSR_32768}
    };

    GMP102::GMP102(I2C* i2c_dev, int odr) : m_ODR(odr)
    {
        _i2c_dev102 = i2c_dev;
    }

    GMP102::~GMP102()
    {
    }

    int32_t GMP102::Initialize()
    {
        int8_t result;
        s8 s8Res;

        gmp102_bus.u8DevAddr = GMP102_7BIT_I2C_ADDR;
        gmp102_bus.bus_read = I2CRead;
        gmp102_bus.bus_write = I2CWrite;

        gmp102_bus_init(&gmp102_bus); //Initailze GMP102 bus to I2C

    	/* GMP102 soft reset */
	    s8Res = gmp102_soft_reset();

        /* Wait 100ms for reset complete */
	    wait_us(100*1000);

    /* GMP102 get the pressure calibration parameters */
        s8Res = gmp102_get_calibration_param(fCalibParam);
        // s8Res = gmp102_get_calibration_param_fixed_point(s16Value, u8Power);
        
        /* GMP102 initialization setup */
        s8Res = gmp102_initialization();

        SetODR(m_ODR);
        DBG_MSG("%s initialized\n", Name());

        return 0;
    }

    int32_t GMP102::Uninitialize()
    {
        return 0;
    }

    int32_t GMP102::Write(const void *data, size_t num)
    {
        return 0;
    }

    int32_t GMP102::Read(void *data, size_t num)
    {
        return ReadData((int16_t *)data);
    }

    int32_t GMP102::Control(uint32_t control, uint32_t arg)
    {
        if (control == SENSOR_CTRL_START)
        {
            _send_data_ready.attach(callback(this, &GMP102::SetDataReady), std::chrono::microseconds((int)(1000000/m_ODR)));
        }
        else if (control == SENSOR_CTRL_STOP)
        {
            _send_data_ready.detach();
        }
        else if (control == SENSOR_CTRL_SET_ODR)
        {
            return SetODR(arg);
        }
        else if (control == SENSOR_CTRL_SELFTEST)
        {
            return SelftTest();
        }
        else if (control == SENSOR_CTRL_GET_ODR)
        {
            *((uint32_t *)arg) = m_ODR;
        }
        else if (control == SENSOR_CTRL_SET_GAIN)
        {
            return 0;
        }

        return 0;
    }

    int32_t GMP102::ReadData(int16_t *data)
    {
        s16 s16T;
        s32 s32P, s32P64_Pa, s32P32_Pa, s32T_Celsius;
        s8 s8Res; 
        float fT_Celsius, fP_Pa;

        /* Measure P */
		s8Res = gmp102_measure_P(&s32P);
		// printf("P(code)=%d\n", s32P);
		
		/* Mesaure T */
		s8Res = gmp102_measure_T(&s16T);
		// printf("T(code)=%d\n", s16T);
		
		/* Compensation, choose one of the three below in actual implementation*/
		gmp102_compensation(s16T, s32P, fCalibParam, &fT_Celsius, &fP_Pa);
		// gmp102_compensation_fixed_point_s64(s16T, s32P, s16Value, u8Power, &s32T_Celsius, &s32P64_Pa);
		// gmp102_compensation_fixed_point_s32(s16T, s32P, s16Value, u8Power, &s32T_Celsius, &s32P32_Pa);

        data[0] = (int16_t)fT_Celsius;
        data[1] = (int16_t)(fP_Pa/GMP102_PRESSURE_SCALE_VALUE);

        return 4; //number of data len with uint8_t type
    }

    int32_t GMP102::SetODR(uint32_t arg)
    {
        uint32_t odr = arg;
        uint8_t reg = 0;
    
        if(odr > gmp102_odr_list[ARRAY_SIZE(gmp102_odr_list)-1].config)
            odr = gmp102_odr_list[ARRAY_SIZE(gmp102_odr_list)-1].config;

        for(uint32_t k=0; k<ARRAY_SIZE(gmp102_odr_list); k++)
        {
            if( gmp102_odr_list[k].config >= odr )
            {
                reg = gmp102_odr_list[k].reg_value;
                odr = gmp102_odr_list[k].config;
                m_ODR = odr;
                break;
            }
        }

        gmp102_set_P_OSR((GMP102_P_OSR_Type)reg);
        gmp102_set_T_OSR((GMP102_T_OSR_Type)reg);

        return m_ODR;
    }

 /**
 * Reads 8 bit values over I2C
 * @param dev_id Device ID (8 bits I2C address)
 * @param reg_addr Register address to read from
 * @param reg_data Read data buffer
 * @param len Number of bytes to read
 * @return 0 on success, non-zero for failure
 */
    int8_t GMP102::I2CRead(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint8_t len)
    {
        int8_t result;
        char data[1];

        data[0] = (char)reg_addr;

        // printf("[0x%X] I2C $%X => ", dev_id , data[0]);

        result = _i2c_dev102->write(dev_id << 1, data, 1);
        // printf("[W: %d] ", result);

        result = _i2c_dev102->read(dev_id << 1, (char *)reg_data, len);

        // for (uint8_t i = 0; i < len; i++)
        //     printf("0x%X ", reg_data[i]);

        // printf("[R: %d, L: %d] \r\n", result, len);

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
    int8_t GMP102::I2CWrite(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint8_t len)
    {
        int8_t result;
        char data[len + 1];

        data[0] = (char)reg_addr;

        for (uint8_t i = 1; i < len + 1; i++)
        {
            data[i] = (char)reg_data[i - 1];
        }

        // printf("[0x%X] I2C $%X <= ", dev_id, data[0]);

        result = _i2c_dev102->write(dev_id << 1, data, len + 1);//result = gmp102_i2c.write(dev_id, data, len + 1);

        // for (uint8_t i = 1; i < len + 1; i++)
        //     printf("0x%X ", data[i]);

        // printf("[W: %d, L: %d] \r\n", result, len);

        return result;
    }

    int32_t GMP102::SelftTest()
    {
        int16_t data[4];
        ReadData(data);
        printf("%d,%d,%d,%d\n", data[0], data[1], data[2], data[3]);

        return 0;
    }
};