#include "sgp30.h"
#include "mbed.h"

namespace CMC
{
   

SGP30::SGP30(uint8_t addr, I2C* i2c_dev) 
{
  m_I2cdev =i2c_dev;
}

SGP30::~SGP30()
{
}

int32_t SGP30::Initialize()
{ 
    char data0[2]={0x20,0x03};
    //char command[2]={0x20,0x08};
    //start 
    m_I2cdev -> write(addr,data0,2);
    thread_sleep_for(250);
    //measure
    //m_I2cdev ->write(addr,command,2);
    DBG_MSG("%s initialized\n", Name() );
    return 0;
}

int32_t SGP30::Uninitialize()
{
    return  0;
}

int32_t SGP30::Write(const void *data, size_t num)
{
    return 0;
}

int32_t SGP30::Read(void *data, size_t num)
{
    return readdata((int16_t *)data);
}


int32_t SGP30::Control(uint32_t control, uint32_t arg) 
{
        if (control == SENSOR_CTRL_START)
        {
            _send_data_ready.attach(callback(this, &SGP30::SetDataReady),chrono::milliseconds(1000));
        }
        else if (control == SENSOR_CTRL_STOP)
        {
            _send_data_ready.detach();
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
            *((uint32_t *)arg) = 1 ; //only support 1HZ
        }
        else if (control == SENSOR_CTRL_SET_GAIN)
        {

        }

        return 0;
}


int32_t SGP30::readdata(int16_t *data)
{   
    char readd[6];
    char command[2]={0x20,0x08};
   
    //measure
    m_I2cdev ->write(addr,command,2);
    thread_sleep_for(50);
    //read data
    m_I2cdev -> read(addr,readd,6);

    char eco2_1 = readd[0];//eco2 data (8bit)
    char eco2_2 = readd[1];//eco2 data (8bit)
    char tvoc_1  = readd[3];//tvoc data (8bit)
    char tvoc_2  = readd[4];//tvoc data (8bit)
    
    int16_t  ECO2 =(eco2_1 <<8)|eco2_2;//eco2(16bit)
    int16_t  TVOC=(tvoc_1<<8)|tvoc_2;//tvoc(16bit)


    data[0] = (eco2_1 <<8)|eco2_2;
    data[1] = (tvoc_1<<8)|tvoc_2;
   
   
    return 4; //number of data len with uint8_t type
   
}


int32_t SGP30:: SelftTest()
{
    int16_t data[2];
    readdata(data);
    printf("eco2 %d ,tvoc %d \n",data[0],data[1]);

    return 0 ;
}

}