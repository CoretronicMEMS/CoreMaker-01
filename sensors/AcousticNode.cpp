


#include "AcousticNode.h"
#include "global.h"

namespace CMC {

AcousticNode::AcousticNode(PinName AUDIO_IN_PIN, int odr):
        m_ODR(odr),
        AUDIO_DATA(AUDIO_IN_PIN, 3.3)
{
    
}

AcousticNode::~AcousticNode()
{

}

void AcousticNode::DRDY_ISR()
{
    //ReadData(m_ADCData, ADS131E_ADC_CHANNELS);
    SetDataReady();
}

int32_t AcousticNode::Initialize()
{
    SetODR(m_ODR);
    SetGain(1);
    DBG_MSG("Acoustic Node ODR: %d\n", m_ODR);
    return 0;
}

int32_t AcousticNode::Uninitialize()
{
    return 0;
}

int32_t AcousticNode::Write(const void *data, uint32_t num)
{
    return 0;
}

int32_t AcousticNode::Read(void *data, uint32_t num)
{
    ReadData((int32_t*)data, num);
    return 0;
}

int32_t AcousticNode::Control(uint32_t control, uint32_t arg)
{
    if(control == SENSOR_CTRL_START)
    {

    }
    else if(control == SENSOR_CTRL_STOP)
    {

    }
    else if(control == SENSOR_CTRL_SET_ODR)
    {
        return SetODR(arg);
    }
    else if(control == SENSOR_CTRL_SELFTEST)
    {
        return SelftTest();
    }
    else if(control == SENSOR_CTRL_GET_ODR)
    {
        *((uint32_t*)arg) = m_ODR;
    }
    else if(control == SENSOR_CTRL_SET_GAIN)
    {
        return SetGain(arg);
    }
    else if(control == AN_CTRL_GET_SAMPLE_COUNT)
    {
        *((uint32_t*)arg) = m_intCount;
        m_intCount = 0;
        return 1;
    }

    return 0;
}

int32_t AcousticNode::ReadData(int32_t *data, uint32_t num)
{
    data[0] = AUDIO_DATA.read_u16() * m_gain;
    //float_t temp = AUDIO_DATA.read_voltage() * 100;
    //printf("Acoustic Node - Input Voltage: %d\n", (int)temp);
    //printf("Acoustic Node - ADC value (16 bits): (DEC)%d (HEX)0x%x\n", AUDIO_DATA.read_u16(), AUDIO_DATA.read_u16());
    m_intCount++;
    return 1;
}

int32_t AcousticNode::SetODR(uint32_t arg)
{
    m_ODR = arg;
    return m_ODR;
}

int32_t AcousticNode::SetGain(uint32_t arg)
{
    m_gain = arg;
    DBG_MSG("Acoustic Node Gain: %d\n", m_gain);
    return m_gain;
}

int32_t AcousticNode::SelftTest()
{
    int32_t adc[1];
    ReadData(adc, 1);
    printf("%ld\n", adc[0]);

    SetGain(m_gain);
    SetODR(m_ODR);

    return 1;
}

}; //namespace CMC


