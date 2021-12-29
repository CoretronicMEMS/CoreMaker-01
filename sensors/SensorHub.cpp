#include "SensorHub.h"
#include "ADS131E.h"
#include "AcousticNode.h"
#include "mbed_bme680.h"
#include "KX122.h"

namespace CMC
{
    SPI spi0(PA_0, PA_1, PA_2, PA_3, mbed::use_gpio_ssel);
    I2C bme680_i2c(PA_6, PA_7);
    // ADS131E ads131e(&spi0, PB_0, PF_4, PF_5, 1000);
    AcousticNode acoustic_node(PB_6, 48000);
    BME680 bme680(0x76 << 1);
    KX122 kx122(&spi0, PA_10);
    EventFlags sensorEvent("sensorEvent");

    int32_t adc_data[6];
    float bme680_sensor_data[4];
    float kx122_data[3];

    /**
 * @brief Sensor list. 
 * The arrangement must be the same with enum _SensorType
 * 
 */
    Sensor *sensors[] =
        {
            // &ads131e,
            &acoustic_node,
            &bme680,
            &kx122,
    };

    /**
 *@brief set the ODR of the sensor
 *
 *@param sensor_id  sensor ID
 *@param odr        output data rate
 *@return uint32_t  ODR actually set
 */
    uint32_t SensorHub_SetODR(SensorType sensor_id, uint32_t odr)
    {
        uint32_t new_odr;
        sensors[sensor_id]->Control(SENSOR_CTRL_STOP, 0);
        sensors[sensor_id]->Control(SENSOR_CTRL_SET_ODR, odr);
        sensors[sensor_id]->Control(SENSOR_CTRL_GET_ODR, (uint32_t)&new_odr);
        sensors[sensor_id]->Control(SENSOR_CTRL_START, 0);
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
    int32_t SensorHub_Control(SensorType sensor_id, uint32_t control, uint32_t arg)
    {
        if (sensor_id < SENSOR_MAX)
        {
            switch (control)
            {
            case SENSOR_CTRL_START:
                DBG_MSG("SensorHub on\n");
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_STOP:
                DBG_MSG("SensorHub off\n");
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_SET_ODR:
                return SensorHub_SetODR(sensor_id, arg);
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
 *@brief initialize all sensors
 *
 */
    void SensorHub_Initial(void)
    {
        for (uint32_t i = 0; i < ARRAY_SIZE(sensors); i++)
        {
            sensors[i]->Initialize();
            sensors[i]->SetAsyncEvent(&sensorEvent, SENSOR_EVENT(i));
        }
    }

    /**
 * @brief 
 * 
 * @param pvParameters unused
 */
    void SensorHub_Task()
    {
        SensorHub_Initial();
        int count = 0;
        while (true)
        {
            uint32_t flags = sensorEvent.wait_any(0xFFFF, 1000);
            // if (flags & SENSOR_EVENT(SENSOR_ADS131E))
            // {
            //     sensors[SENSOR_ADS131E]->Read(&adc_data, sizeof(adc_data));
            // }
            if (flags & SENSOR_EVENT(SENSOR_ACOUSTIC_NODE))
            {
                sensors[SENSOR_ACOUSTIC_NODE]->Read(&adc_data, sizeof(adc_data));
            }
            if (flags & SENSOR_EVENT(SENSOR_BME680))
            {
                sensors[SENSOR_BME680]->Read(&bme680_sensor_data, sizeof(bme680_sensor_data));
            }
            if (flags & SENSOR_EVENT(SENSOR_KX122))
            {
                sensors[SENSOR_KX122]->Read(&kx122_data, sizeof(kx122_data));
            }
        }
    }

}
