


#include "AcousticNode.h"
#include "global.h"

namespace CMC {

AcousticNode::AcousticNode(PinName AUDIO_IN_PIN, int odr):
        m_ODR(odr),
        AUDIO_DATA(AUDIO_IN_PIN)
{
    
}

AcousticNode::~AcousticNode()
{

}

int32_t AcousticNode::Initialize()
{
    SetODR(m_ODR);
    SetGain(1);
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
        m_timer.attach(callback(this, &AcousticNode::TimerCallback), std::chrono::milliseconds((int)(1000/m_ODR)));
        m_isOn = true;
    }
    else if(control == SENSOR_CTRL_STOP)
    {
        m_timer.detach();
        m_isOn = false;
    }
    else if(control == SENSOR_CTRL_SET_ODR)
    {
        if(m_isOn)
            m_timer.detach();
        int32_t odr = SetODR(arg);
        if(m_isOn)
            m_timer.attach(callback(this, &AcousticNode::TimerCallback), std::chrono::milliseconds((int)(1000/m_ODR)));
        return odr;
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

    return 0;
}

int32_t AcousticNode::ReadData(int32_t *data, uint32_t num)
{
    uint16_t adc = AUDIO_DATA.read_u16();
    data[0] = adc;

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
    return m_gain;
}

int32_t AcousticNode::SelftTest()
{
    uint16_t adc = AUDIO_DATA.read_u16();

    if(adc > 0x7F00 && adc < 0x8100)
        printf("SelftTest OK, %u\n", adc);
    else
        printf("SelftTest NG, %u\n", adc);

    return 1;
}

void AcousticNode::TimerCallback()
{
    SetDataReady();
}

}; //namespace CMC


