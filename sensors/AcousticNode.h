

#ifndef AcousticNode_H
#define AcousticNode_H

#include "SensorHub.h"

#define AN_ADC_CHANNELS            2

#define AN_CTRL_GET_SAMPLE_COUNT   0x102


namespace CMC {


class AcousticNode final : public Sensor
{
public:
    AcousticNode(PinName AUDIO_IN_PIN, int odr=48000);
    ~AcousticNode() override;
    int32_t Initialize() override final;
    int32_t Uninitialize() override final;
    int32_t Write(const void *data, size_t num) override final;
    int32_t Read(void *data, size_t num) override final;
    int32_t Control(uint32_t control, uint32_t arg) override final;
    virtual const char* Name() { return "spu0410"; }

private:
    int32_t ReadData(uint16_t *data, uint32_t num);
    int32_t SetODR(uint32_t arg);
    int32_t SetGain(uint32_t arg);
    int32_t SelftTest();
    void TimerCallback();

private:
    bool m_isOn = false;
    int32_t m_ADCData;
    uint32_t m_ODR;
    uint32_t m_gain = 1;
    Ticker m_timer;

    AnalogIn AUDIO_DATA;
};

}

#endif //AcousticNode_H

