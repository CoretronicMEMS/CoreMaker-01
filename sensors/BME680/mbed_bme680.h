#ifndef BME680_H
#define BME680_H

#include "bme680.h"
#include "mbed.h"
#include "SensorHub.h"

#define BME680_CTRL_GET_SAMPLE_COUNT 0x102

#define BME680_DEFAULT_ADDRESS (0x77 << 1) // The default I2C address (shifted for MBed 8 bit address)
// #define BME680_DEBUG_MODE  // Use this for enhance debug logs for I2C and more.

namespace CMC
{
    extern I2C bme680_i2c;

    /**
 * BME680 Class for I2C usage.
 * Wraps the Bosch library for MBed usage.
 */

    class BME680 final : public Sensor
    {
    public:
        BME680(uint8_t adr, uint8_t temperature_os = BME680_OS_8X,
               uint8_t pressure_os = BME680_OS_4X, uint8_t humidity_os = BME680_OS_8X,
               uint8_t filter_size = BME680_FILTER_SIZE_3, uint16_t heater_temp = 320,
               uint16_t heater_time = 150);
        ~BME680() override;
        int32_t Initialize() override final;
        int32_t Uninitialize() override final;
        int32_t Write(const void *data, uint32_t num) override final;
        int32_t Read(void *data, uint32_t num) override final;
        int32_t Control(uint32_t control, uint32_t arg) override final;

    private:
        bool _filterEnabled, _tempEnabled, _humEnabled, _presEnabled, _gasEnabled;
        int32_t _sensorID;
        struct bme680_dev gas_sensor;
        struct bme680_field_data bme680_data;
        uint8_t _adr;
        uint32_t m_intCount;
        uint8_t _temperature_os;
        uint8_t _pressure_os;
        uint8_t _humidity_os;
        uint8_t _filter_size;
        uint16_t _heater_temp;
        uint16_t _heater_time;

        int32_t ReadData(float *data, uint32_t num);
        static void log(const char *format, ...);
        bool setTemperatureOversampling(uint8_t os);
        bool setPressureOversampling(uint8_t os);
        bool setHumidityOversampling(uint8_t os);
        bool setIIRFilterSize(uint8_t fs);
        bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime);
        bool performReading();
        bool isGasHeatingSetupStable();
        int32_t SelftTest();
        // BME680 - hardware interface
        static int8_t i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
        static int8_t i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
        static void delay_msec(uint32_t ms);
    };

}

#endif
