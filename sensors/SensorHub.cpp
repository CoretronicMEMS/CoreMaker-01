#include "SensorHub.h"
#include "USBSerial.h"
#include "ADS131E.h"
#include "AcousticNode.h"
#include "mbed_bme680.h"
#include "KX122.h"
#include "GMC306.h"
#include "DebounceIn.h"

extern DigitalOut led_r;
extern DigitalOut led_g;
extern DigitalOut led_b;
extern DebounceIn sw2;
extern DigitalIn sw3_2;
extern DigitalIn sw3_3;
extern USBCDC serial;

#define SW_EVENT    0x8000

namespace CMC
{
    SPI spi0(PA_0, PA_1, PA_2, PA_3, mbed::use_gpio_ssel);
    I2C i2c1(I2C_SDA, I2C_SCL);

    AcousticNode acoustic_node(PB_6, 48000);
    BME680 bme680(0x76 << 1, &i2c1);
    GMC306 gmc306(&i2c1);
    KX122 kx122(&spi0, PA_10);

    int32_t adc_data[6];
    float bme680_sensor_data[4];
    int magnet_data[GMC306_ADC_CHANNELS];
    float kx122_data[3];

    int GetSwitchSelect()
    {
        int sw_sel;
        sw_sel = (sw3_2<<0) | (sw3_3<<1);
        return sw_sel;
    }

    /**
     * @brief Sensor list. 
     * The arrangement must be the same with enum SensorType
     * 
     */
    Sensor *SensorHub::sensors[] =
    {
        &acoustic_node,
        &bme680,
        &kx122,
        &gmc306
    };

    SensorHub::SensorHub() : sensorEvent("sensorEvent")
    {

    }

    /**
     *@brief initialize all sensors
     *
     */
    void SensorHub::Initial()
    {
        for (uint32_t i = 0; i < ARRAY_SIZE(sensors); i++)
        {
            sensors[i]->Initialize();
            sensors[i]->SetAsyncEvent(&sensorEvent, SENSOR_EVENT(i));
        }
    }

    /**
     * @brief Start sensor hub thread
     * 
     */
    void SensorHub::Start()
    {
        m_thread.start(callback(this, &SensorHub::HubTask));
    }

    /**
     *@brief set the ODR of specific sensor
     *
     *@param sensor_id  sensor ID
     *@param odr        output data rate
     *@return uint32_t  ODR actually set
     */
    uint32_t SensorHub::SetODR(SensorType sensor_id, uint32_t odr)
    {
        uint32_t new_odr;
        sensors[sensor_id]->Control(SENSOR_CTRL_SET_ODR, odr);
        sensors[sensor_id]->Control(SENSOR_CTRL_GET_ODR, (uint32_t)&new_odr);
        return new_odr;
    }

    /**
     *@brief Control specific sensor
     *
     *@param sensor_id  specifies the sensor
     *@param control    control command
     *@param arg        command related parameters
     *@return int32_t   execution result
     */
    int32_t SensorHub::Control(SensorType sensor_id, uint32_t control, uint32_t arg = 0)
    {
        if (sensor_id < SENSOR_MAX)
        {
            switch (control)
            {
            case SENSOR_CTRL_START:
                DBG_MSG("SensorHub: sensor%d %s on\n", sensor_id, sensors[sensor_id]->Name());
                m_SensorStart = true;
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_STOP:
                DBG_MSG("SensorHub: sensor%d %s off\n", sensor_id, sensors[sensor_id]->Name());
                m_SensorStart = false;
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_SET_ODR:
                return SetODR(sensor_id, arg);
            case SENSOR_CTRL_SET_GAIN:
                break;
            case SENSOR_CTRL_SELFTEST:
                DBG_MSG("SensorHub_SensorTest\n");
                return sensors[sensor_id]->Control(control, 0);
            case SENSOR_CTRL_GET_ODR:
                uint32_t odr;
                sensors[sensor_id]->Control(control, (uint32_t)&odr);
                return odr;
            case SENSOR_CTRL_GET_GAIN:
            default:
                break;
            }
            return 0;
        }

        return -1;
    }

    /**
     * @brief Sensor hub thread
     * 
     */
    void SensorHub::HubTask()
    {
        Initial();

        m_SensorSel = (SensorType)GetSwitchSelect();
        DBG_MSG("Sensor select: %d\n", m_SensorSel);
        Control(m_SensorSel, SENSOR_CTRL_START);
    
        sw2.fall(callback(this, &SensorHub::SW2PressISR));

        int count = 0;
        while (true)
        {
            uint flags = sensorEvent.wait_any(0xFFFF, 100);
            if (!(flags & osFlagsError))
            {
                if (flags & SENSOR_EVENT(SENSOR_ACOUSTIC_NODE))
                {
                    sensors[SENSOR_ACOUSTIC_NODE]->Read(&adc_data, sizeof(adc_data));
                    //printf("%ld, %ld, %ld, %ld, %ld, %ld\n", adc_data[0], adc_data[1], adc_data[2], adc_data[3], adc_data[4], adc_data[5]);
                }
                if (flags & SENSOR_EVENT(SENSOR_BME680))
                {
                    sensors[SENSOR_BME680]->Read(&bme680_sensor_data, sizeof(bme680_sensor_data));
                    printf("bme680_sensor_data: %.2f, %.2f, %.2f, %.2f\n", bme680_sensor_data[0], bme680_sensor_data[1], bme680_sensor_data[2], bme680_sensor_data[3]);
                }
                if (flags & SENSOR_EVENT(SENSOR_GMC306))
                {
                    sensors[SENSOR_GMC306]->Read(&magnet_data, sizeof(magnet_data));
                    printf("magnet: %d, %d, %d\n", magnet_data[0], magnet_data[1], magnet_data[2]);
                }
                if (flags & SENSOR_EVENT(SENSOR_KX122))
                {
                    sensors[SENSOR_KX122]->Read(&kx122_data, sizeof(kx122_data));
                    printf("kx122_data: %.2f, %.2f, %.2f\n", kx122_data[0], kx122_data[1], kx122_data[2]);
                }
                if (flags & SW_EVENT)
                {
                    ButtonPress();
                }
                led_g = !led_g;
            }
            else if (flags == osFlagsErrorTimeout) // No event
            {
                led_g = 1;
            }
        }
    }

    void SensorHub::SW2PressISR()
    {
        sensorEvent.set(SW_EVENT);
    }

    void SensorHub::ButtonPress()
    {
        if (m_SensorStart)
        {
            Control(m_SensorSel, SENSOR_CTRL_STOP);
            m_SensorStart = false;
        }
        else
        {
            m_SensorSel = (SensorType)GetSwitchSelect();
            Control(m_SensorSel, SENSOR_CTRL_START);
            m_SensorStart = true;
        }
        led_b = !m_SensorStart;
    }
}
