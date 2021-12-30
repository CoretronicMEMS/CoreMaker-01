
#ifndef CMC_SENSOR_HUB_H
#define CMC_SENSOR_HUB_H

#include "mbed.h"
#include "global.h"


#define ENABLE_DEBUG_MSG

#ifdef ENABLE_DEBUG_MSG
#define DBG_MSG(...)       printf( __VA_ARGS__)
#else
#define DBG_MSG(...)
#endif


namespace CMC {

#define SENSOR_CTRL_START              0x01
#define SENSOR_CTRL_STOP               0x02
#define SENSOR_CTRL_SET_ODR            0x03
#define SENSOR_CTRL_SET_GAIN           0x04
#define SENSOR_CTRL_SELFTEST           0x05
#define SENSOR_CTRL_GET_ODR            0x13
#define SENSOR_CTRL_GET_GAIN           0x14


enum SensorType
{
    SENSOR_ACOUSTIC_NODE,
    SENSOR_BME680,
    SENSOR_GMC306,
    SENSOR_MAX
};

#define SENSOR_EVENT(id)    (1<<id)

class Sensor {

public:
    Sensor() : m_eventHande(NULL), m_flags(0) {};
    virtual ~Sensor() = default;
    virtual int32_t Initialize() = 0;
    virtual int32_t Uninitialize() = 0;
    virtual int32_t Write(const void *data, uint32_t num) = 0;
    virtual int32_t Read(void *data, uint32_t num) = 0;
    virtual int32_t Control(uint32_t control, uint32_t arg) = 0;
    virtual const char* Name() { return ""; }

    void SetAsyncEvent(EventFlags *eventHandle, uint32_t flags)
    {
        m_eventHande = eventHandle;
        m_flags = flags;
    }

protected:
    void SetDataReady()
    {
        if(m_eventHande)
        {
            m_eventHande->set(m_flags);
        }
    }
    void ClearDataReady()
    {
        if(m_eventHande)
            m_eventHande->clear(m_flags);
    }


    EventFlags *m_eventHande;
    uint32_t m_flags;
};


int32_t  SensorHub_Control(SensorType sensor_id, uint32_t control, uint32_t arg);
void     SensorHub_Initial();
void     SensorHub_Task();


extern Sensor* sensors[];
extern int32_t adc_data[6];
extern float bme680_sensor_data[4];

};

#endif
