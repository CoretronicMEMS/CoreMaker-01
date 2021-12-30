
#include "GMC306.h"
#include "global.h"


#define GMC306_ADDRESS      (0x0C<<1)
#define GMC306_PID          0x13
#define GMC306A_PID         0x43
#define DEFAULT_ODR         10

// GMC306 registers
#define REG_ID              0x50
#define REG_CNTL1           0x10
#define REG_CNTL2           0x11
#define REG_HXH             0x01
#define REG_HXL             0x02
#define REG_HYH             0x03
#define REG_HYL             0x04
#define REG_HZH             0x05
#define REG_HZL             0x06


namespace CMC {


struct Config_to_Reg
{
    uint32_t config;
    uint8_t reg_value;
};

const static Config_to_Reg gmc306_odr_list[] = 
{
    {  10, 0x02},
    {  20, 0x04},
    {  50, 0x06},
    { 100, 0x08},
    { 200, 0x0C}
};

GMC306::GMC306(I2C *i2c_dev) : m_I2CDev(i2c_dev)
{

}

GMC306::~GMC306()
{

}

int32_t GMC306::Initialize()
{
    ThisThread::sleep_for(std::chrono::milliseconds(10));

    //wait_us(10);

	m_PID = ReadOneRegister(REG_ID);
    DBG_MSG("Probe gmc306: 0x%02X\n", m_PID);

    WriteOneRegister(REG_CNTL2, 1);
    ThisThread::sleep_for(std::chrono::milliseconds(10));

    SetODR(DEFAULT_ODR);

    return 0;
}

int32_t GMC306::Uninitialize()
{
    return 0;
}

int32_t GMC306::Write(const void *data, uint32_t num)
{
    return 0;
}

int32_t GMC306::Read(void *data, uint32_t num)
{
    ReadData(m_SensorData, GMC306_ADC_CHANNELS);
    memcpy(data, m_SensorData, num<sizeof(m_SensorData)?num:sizeof(m_SensorData));

    return 0;
}

int32_t GMC306::Control(uint32_t control, uint32_t arg)
{
    if(control == SENSOR_CTRL_START)
    {
        uint8_t reg = ReadOneRegister(REG_CNTL1);
        reg = (reg & 0xE0) | 0x40 | m_OdrReg;
        WriteOneRegister(REG_CNTL1, reg);
        m_timer.attach(callback(this, &GMC306::TimerCallback), std::chrono::milliseconds((int)(1000/m_ODR)));

        m_isOn = true;
    }
    else if(control == SENSOR_CTRL_STOP)
    {
        uint8_t reg = ReadOneRegister(REG_CNTL1);
        reg = (reg & 0xE0);
        WriteOneRegister(REG_CNTL1, reg);
        m_timer.detach();

        m_isOn = false;
    }
    else if(control == SENSOR_CTRL_SET_ODR)
    {
        SetODR(arg);
        return m_ODR;
    }
    else if(control == SENSOR_CTRL_SELFTEST)
    {
        return SelftTest();
    }
    else if(control == SENSOR_CTRL_GET_ODR)
    {
        return m_ODR;
    }
    else if(control == SENSOR_CTRL_SET_GAIN)
    {
        return 0;
    }


    return 0;
}

int32_t GMC306::ReadData(int32_t *data, int num)
{
    uint8_t buf[6];

    ReadRegisters(REG_HXH, sizeof(buf), buf);
    
    for(int i=0; i<GMC306_ADC_CHANNELS && i<num; i++)
        data[i] = (int16_t)((buf[i*2+0]<<8) | buf[i*2+1]);

    return GMC306_ADC_CHANNELS;
}

int32_t GMC306::ReadRegisters(uint8_t reg_addr, uint8_t num, uint8_t *data)
{
    m_I2CDev->write(GMC306_ADDRESS, (const char *)&reg_addr, 1, true);
    m_I2CDev->read(GMC306_ADDRESS, (char *)data, num);

	return num;
}

uint8_t GMC306::ReadOneRegister(uint8_t reg_addr)
{
    uint8_t reg = 0;
    ReadRegisters(reg_addr, 1, &reg);
    return reg;
}

int32_t GMC306::WriteRegisters(uint8_t reg_addr, uint8_t num, const uint8_t *data)
{
    char buf[10];

    buf[0] = reg_addr;
    memcpy(buf+1, data, num<9?num:9);
    m_I2CDev->write(GMC306_ADDRESS, (const char *)buf, num+1);

	return num;
}

int32_t GMC306::WriteOneRegister(uint8_t reg_addr, uint8_t data)
{
    WriteRegisters(reg_addr, 1, &data);
    return 1;
}

int32_t GMC306::SetODR(uint32_t arg)
{
    uint32_t odr = arg;
    
    if(odr > gmc306_odr_list[ARRAY_SIZE(gmc306_odr_list)-1].config)
        odr = gmc306_odr_list[ARRAY_SIZE(gmc306_odr_list)-1].config;

    for(uint32_t k=0; k<ARRAY_SIZE(gmc306_odr_list); k++)
    {
        if( gmc306_odr_list[k].config >= odr )
        {
            m_OdrReg = gmc306_odr_list[k].reg_value;
            m_ODR = gmc306_odr_list[k].config;
            break;
        }
    }

    if(m_isOn)
    {
        Control(SENSOR_CTRL_STOP, 0);
        Control(SENSOR_CTRL_START, 0);
    }
    return m_ODR;
}

int32_t GMC306::SelftTest()
{
    return 1;
}

void GMC306::TimerCallback()
{
    // I2C operation can't use in ISR
    // just send event to sensor hub, let it perform I2C transmission
    SetDataReady();

    if(m_isOn)
        m_timer.attach(callback(this, &GMC306::TimerCallback), std::chrono::milliseconds((int)(1000/m_ODR)));
}

}; //namespace CMC


