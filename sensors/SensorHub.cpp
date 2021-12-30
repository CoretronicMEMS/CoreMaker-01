

#include "SensorHub.h"
#include "ADS131E.h"
#include "AcousticNode.h"
#include "mbed_bme680.h"
#include "GMC306.h"

extern DigitalOut led_r;
extern DigitalOut led_g;
extern DigitalOut led_b;

namespace CMC
{
    SPI spi0(PA_0, PA_1, PA_2, PA_3, mbed::use_gpio_ssel);
    I2C i2c_sensor(I2C_SDA, I2C_SCL);

    AcousticNode acoustic_node(PB_6, 48000);
    BME680 bme680(0x76 << 1, &i2c_sensor);
    GMC306 gmc306(&i2c_sensor);
    EventFlags sensorEvent("sensorEvent");

    int32_t adc_data[6];
    float bme680_sensor_data[4];
    int magnet_data[GMC306_ADC_CHANNELS];

    /**
     * @brief Sensor list. 
     * The arrangement must be the same with enum _SensorType
     * 
     */
    Sensor *sensors[] =
        {
            &acoustic_node,
            &bme680,
            &gmc306
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
    int32_t SensorHub_Control(SensorType sensor_id, uint32_t control, uint32_t arg = 0)
    {
        if (sensor_id < SENSOR_MAX)
        {
            switch (control)
            {
            case SENSOR_CTRL_START:
                DBG_MSG("SensorHub: sensor%d %s on\n", sensor_id, sensors[sensor_id]->Name());
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_STOP:
                DBG_MSG("SensorHub: sensor %d %s off\n", sensor_id, sensors[sensor_id]->Name());
                return sensors[sensor_id]->Control(control, arg);
            case SENSOR_CTRL_SET_ODR:
                return SensorHub_SetODR(sensor_id, arg);
            case SENSOR_CTRL_SET_GAIN:
                break;
            case SENSOR_CTRL_SELFTEST:
                DBG_MSG("SensorHub_SensorTest\n");
                return sensors[sensor_id]->Control(control, 0);
            case SENSOR_CTRL_GET_ODR:
                return sensors[sensor_id]->Control(control, arg);
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
     * @brief Sensor hub thread
     * 
     */
    void SensorHub_Task()
    {
        SensorHub_Initial();
        SensorHub_Control(SENSOR_GMC306, SENSOR_CTRL_SET_ODR, 50);
        SensorHub_Control(SENSOR_GMC306, SENSOR_CTRL_START);

        int count = 0;
        while (true)
        {
            uint flags = sensorEvent.wait_any(0xFF, 1000);
            if (!(flags & osFlagsError))
            {
                if (flags & SENSOR_EVENT(SENSOR_ACOUSTIC_NODE))
                {
                    sensors[SENSOR_ACOUSTIC_NODE]->Read(&adc_data, sizeof(adc_data));
                    printf("%ld, %ld, %ld, %ld, %ld, %ld\n", adc_data[0], adc_data[1], adc_data[2], adc_data[3], adc_data[4], adc_data[5]);
                }
                if (flags & SENSOR_EVENT(SENSOR_BME680))
                {
                    sensors[SENSOR_BME680]->Read(&bme680_sensor_data, sizeof(bme680_sensor_data));
                    printf("%.2f, %.2f, %.2f, %.2f\n", bme680_sensor_data[0], bme680_sensor_data[1], bme680_sensor_data[2], bme680_sensor_data[3]);
                }
                if (flags & SENSOR_EVENT(SENSOR_GMC306))
                {
                    sensors[SENSOR_GMC306]->Read(&magnet_data, sizeof(magnet_data));
                    //printf("magnet: %d, %d, %d\n", magnet_data[0], magnet_data[1], magnet_data[2]);
                }
                led_g = !led_g;
            }
            else if (flags == osFlagsErrorTimeout)
            {
                //printf("No event\n");
                led_g = 0;
            }
        }
    }

}
