#include "KX122.h"
#include "kx123_registers.h"

#define CMD_RREG 0x80
#define CMD_WREG 0x00

namespace CMC
{
    struct Config_to_Reg
    {
        uint32_t config;
        uint8_t reg_value;
    };

    const static Config_to_Reg kx122_odr_list[] =
        {
            {25, KX122_ODCNTL_OSA_25},
            {50, KX122_ODCNTL_OSA_50},
            {100, KX122_ODCNTL_OSA_100},
            {200, KX122_ODCNTL_OSA_200},
            {400, KX122_ODCNTL_OSA_400},
            {800, KX122_ODCNTL_OSA_800},
            {1600, KX122_ODCNTL_OSA_1600},
            {3200, KX122_ODCNTL_OSA_3200},
            {6400, KX122_ODCNTL_OSA_6400},
            {12800, KX122_ODCNTL_OSA_12800},
            {25600, KX122_ODCNTL_OSA_25600},
    };

    KX122::KX122(SPI *spi_obj, PinName drdy_pin)
        : spi_bus(spi_obj),
          DRDY_PIN(drdy_pin)
    {
        spi_bus->frequency(10000000); //10MHz
        spi_bus->format(8, 0);
        spi_bus->set_default_write_value(0x0F);
    }

    KX122::~KX122()
    {
    }

    void KX122::DRDY_ISR()
    {
        SetDataReady();
    }

    int32_t KX122::Initialize()
    {
        DRDY_PIN.rise(callback(this, &KX122::DRDY_ISR));
        uint8_t buf;
        buf = SPIReadRegister(KX122_WHO_AM_I);
        if (buf == KX122_WHO_AM_I_WAI_ID)
        {
            //First set CNTL1 PC1-bit to stand-by mode, after that setup can be made -->important
            SPIWriteRegister(KX122_CNTL1, 0);

            // Enable physical interrupt pin and set interrupt latch to pulse mode
            SPIWriteRegister(KX122_INC1, (KX122_INC1_IEN1 | KX122_INC1_IEL1));
            // printf("KX122_INC1 = 0x%X\n", SPIReadRegister(KX122_INC1));

            // Enable data ready interrupt
            SPIWriteRegister(KX122_INC4, (KX122_INC4_DRDYI1 /* | KX122_INC4_WMI1*/));
            // printf("KX122_INC4 = 0x%X\n", SPIReadRegister(KX122_INC4));

            // SPIWriteRegister(KX122_BUF_CNTL1, 12);
            // printf("KX122_BUF_CNTL1 = 0x%X\n", SPIReadRegister(KX122_BUF_CNTL1));

            // SPIWriteRegister(KX122_BUF_CNTL2, (KX122_BUF_CNTL2_BUFE | KX122_BUF_CNTL2_BRES | KX122_BUF_CNTL2_BUF_M_FIFO));
            // printf("KX122_BUF_CNTL2 = 0x%X\n", SPIReadRegister(KX122_BUF_CNTL2));

            //ODCNTL: Output Data Rate control (ODR)
            SetODR(25600);
            // printf("KX122_ODCNTL = 0x%x\n", SPIReadRegister(KX122_ODCNTL));

            //Setup G-range and 8/16-bit resolution + set CNTL1 PC1-bit to operating mode (also WUF_EN, TP_EN and DT_EN)
            SPIWriteRegister(KX122_CNTL1, (/*KX122_CNTL1_PC1 | */ KX122_CNTL1_GSEL_8G | KX122_CNTL1_RES | KX122_CNTL1_DRDYE));
            // printf("KX122_CNTL1 = 0x%x\n", SPIReadRegister(KX122_CNTL1));

            //resolution_divider = 32768/2;  //KX122_CNTL1_GSEL_2G
            //resolution_divider = 32768/4;  //KX122_CNTL1_GSEL_4G
            resolution_divider = 32768 / 8; //KX122_CNTL1_GSEL_8G

            return 0;
        }
        else
        {
            printf("KX122 not found (WAI %d, not %d). ", buf, KX122_WHO_AM_I_WAI_ID);

            return -1;
        }

        return 0;
    }

    int32_t KX122::Uninitialize()
    {
        return 0;
    }

    int32_t KX122::Write(const void *data, uint32_t num)
    {
        return 0;
    }

    int32_t KX122::Read(void *data, uint32_t num)
    {
        ReadData((float *)data);

        return 0;
    }

    int32_t KX122::Control(uint32_t control, uint32_t arg)
    {
        if (control == SENSOR_CTRL_START)
        {
            SetOperatingMode();
        }
        else if (control == SENSOR_CTRL_STOP)
        {
            SetStandByMode();
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

    void KX122::SetOperatingMode(void)
    {
        ChangeBits(KX122_CNTL1, KX122_CNTL1_PC1, KX122_CNTL1_PC1);
    }

    void KX122::SetStandByMode(void)
    {
        ChangeBits(KX122_CNTL1, KX122_CNTL1_PC1, 0);
    }

    int32_t KX122::SetODR(uint32_t arg)
    {
        uint32_t odr = arg;
        uint8_t reg = 0;

        if (odr > kx122_odr_list[ARRAY_SIZE(kx122_odr_list) - 1].config)
            odr = kx122_odr_list[ARRAY_SIZE(kx122_odr_list) - 1].config;

        for (uint32_t k = 0; k < ARRAY_SIZE(kx122_odr_list); k++)
        {
            if (kx122_odr_list[k].config >= odr)
            {
                reg = kx122_odr_list[k].reg_value;
                odr = kx122_odr_list[k].config;
                m_ODR = odr;
                break;
            }
        }

        SPIWriteRegister(KX122_ODCNTL, CMD_WREG | reg);

        return odr;
    }

    int32_t KX122::SelftTest()
    {
        uint8_t cotr_value;
        bool read_ok;
        DRDY_PIN.disable_irq();
        //first read to make sure COTR is in default value
        cotr_value = SPIReadRegister(KX122_COTR);
        if (cotr_value != 0x55)
        {
            DRDY_PIN.enable_irq();
            return -1;
        }
        ChangeBits(KX122_CNTL2, KX122_CNTL2_COTC, KX122_CNTL2_COTC);
        cotr_value = SPIReadRegister(KX122_COTR);
        if (cotr_value != 0xAA)
        {
            DRDY_PIN.enable_irq();
            return -2;
        }
        DRDY_PIN.enable_irq();
        return 0;
    }

    void KX122::ReadData(float *buf)
    {
        int16_t xyz[3];
        uint8_t tmp[6]; //XYZ (lhlhlh)

        tmp[0] = SPIReadRegister(KX122_XOUT_L);
        tmp[1] = SPIReadRegister(KX122_XOUT_H);
        tmp[2] = SPIReadRegister(KX122_YOUT_L);
        tmp[3] = SPIReadRegister(KX122_YOUT_H);
        tmp[4] = SPIReadRegister(KX122_ZOUT_L);
        tmp[5] = SPIReadRegister(KX122_ZOUT_H);

        xyz[0] = (tmp[1] << 8) | tmp[0];
        xyz[1] = (tmp[3] << 8) | tmp[2];
        xyz[2] = (tmp[5] << 8) | tmp[4];

        buf[0] = (float)xyz[0] / resolution_divider; //X
        buf[1] = (float)xyz[1] / resolution_divider; //Y
        buf[2] = (float)xyz[2] / resolution_divider; //Z
    }

    uint8_t KX122::SPIReadRegister(uint8_t reg_addr)
    {
        uint8_t data = 0;

        spi_bus->select();
        spi_bus->write(CMD_RREG | reg_addr);
        data = spi_bus->write(0x00);
        spi_bus->deselect();
        spi_bus->clear_transfer_buffer();

        return data;
    }

    void KX122::SPIWriteRegister(uint8_t reg_addr, uint8_t data)
    {
        spi_bus->select();
        spi_bus->write(CMD_WREG | reg_addr);
        spi_bus->write(data);
        spi_bus->deselect();
        spi_bus->clear_transfer_buffer();

        return;
    }

    void KX122::ChangeBits(uint8_t reg_addr, uint8_t mask, uint8_t bits)
    {
        uint8_t value;

        value = SPIReadRegister(reg_addr);
        value = value & ~mask;
        value = value | (bits & mask);
        SPIWriteRegister(reg_addr, value);
    }

}; //namespace CMC