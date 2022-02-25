
#ifndef GMP102_DEFS_H_
#define GMP102_DEFS_H_

/********************************************************/
/* header includes */
#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

/******************************************************************************/
/*! @name		Common macros					      */
/******************************************************************************/

#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x)       S8_C(x)
#define UINT8_C(x)      U8_C(x)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x)      S16_C(x)
#define UINT16_C(x)     U16_C(x)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x)      S32_C(x)
#define UINT32_C(x)     U32_C(x)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x)      S64_C(x)
#define UINT64_C(x)     U64_C(x)
#endif

/**@}*/

/**\name C standard macros */
//#ifndef NULL
#ifdef __cplusplus
#define NULL   0
#else
#define NULL   ((void *) 0)
#endif
//#endif


/** GMP102 General config */
#define GMP102_POLL_PERIOD_MS		10

/** GMP102 I2C addresses */
#define GMP102_I2C_ADDR_PRIMARY		0x6C
#define GMP102_I2C_ADDR_SECONDARY	0x6D

/** GMP102 unique chip identifier */
#define GMP102_CHIP_ID  0x02

/** Over-sampling settings */
#define GMP102_OSR_1024		0
#define GMP102_OSR_2048		1
#define GMP102_OSR_4096		2
#define GMP102_OSR_8192		3
#define GMP102_OSR_256		4
#define GMP102_OSR_512		5
#define GMP102_OSR_16384	6
#define GMP102_OSR_32768	7

/** GMP102 coefficients related defines */
#define GMP102_COEFF_SIZE		18


/** Error code definitions */
#define GMP102_OK		0
/* Errors */
#define GMP102_E_NULL_PTR		    -1
#define GMP102_E_COM_FAIL		    -2
#define GMP102_E_DEV_NOT_FOUND		-3
#define GMP102_E_INVALID_LENGTH		-4

#define GMP102_TEMPERATURE_SENSITIVITY 256  //1 Celsius = 256 code
#define GMP102_T_CODE_TO_CELSIUS(tCode) (((float)(tCode)) / GMP102_TEMPERATURE_SENSITIVITY)

/*Registers Address*/
#define GMP102_REG_RESET	    0x00
#define GMP102_REG_PID 	        0x01     
#define GMP102_REG_STATUS 	    0x02
#define GMP102_REG_PRESSH	    0x06
#define GMP102_REG_PRESSM	    0x07
#define GMP102_REG_PRESSL	    0x08
#define GMP102_REG_TEMPH	    0x09
#define GMP102_REG_TEMPL	    0x0A
#define GMP102_REG_CMD	 	    0x30
#define GMP102_REG_CONFIG1 	    0xA5
#define GMP102_REG_CONFIG2 	    0xA6
#define GMP102_REG_CONFIG3 	    0xA7
#define GMP102_REG_CALIB00      0xAA

/*Calibration Table*/


//Soft reset command
#define GMP102_SW_RST_SET_VALUE		    0x24

//P raw data out command
#define GMP102_P_raw_out_SET_VALUE		0x02
//T raw data out command
#define GMP102_T_raw_out_SET_VALUE		0x00

//P-forced command
#define GMP102_P_forced_SET_VALUE		0x09
//T-forced command
#define GMP102_T_forced_SET_VALUE		0x08

/** Delay related macro declaration */
#define GMP102_RESET_PERIOD	    100

/** Chip identifier */
#define GMP102_CHIP_ID_ADDR	        GMP102_REG_PID
/** Calibration paramter identifier */
#define GMP102_Calibration_ADDR	    GMP102_REG_CALIB00
/** Status identifier */
#define GMP102_Status_ADDR	        GMP102_REG_STATUS
/** Pressure identifier */
#define GMP102_Pressure_ADDR	    GMP102_REG_PRESSH
/** Pressure identifier */
#define GMP102_Temperature_ADDR	    GMP102_REG_TEMPH

/* Soft Rest bit */
#define GMP102_RST__REG		    GMP102_REG_RESET
#define GMP102_RST__MSK		    0x24
#define GMP102_RST__POS		    0
/* DRDY bit */
#define GMP102_DRDY__REG	    GMP102_REG_STATUS
#define GMP102_DRDY__MSK	    0x01
#define GMP102_DRDY__POS	    0
/* P OSR bits */
#define GMP102_P_OSR__REG       GMP102_REG_CONFIG2
#define GMP102_P_OSR__MSK       0x07
#define GMP102_P_OSR__POS       0
/* T OSR bits */
#define GMP102_T_OSR__REG       GMP102_REG_CONFIG3
#define GMP102_T_OSR__MSK       0x07
#define GMP102_T_OSR__POS       0


/** Buffer length macro declaration */
#define GMP102_TMP_BUFFER_LENGTH	40


/** Macro to combine two 8 bit data's to form a 16 bit data */
#define GMP102_CONCAT_BYTES(msb, lsb)	(((uint16_t)msb << 8) | (uint16_t)lsb)

#define GMP102_GET_BITSLICE(regvar, bitname)	\
  ((regvar & bitname##__MSK) >> bitname##__POS)

#define GMP102_SET_BITSLICE(regvar, bitname, val)			\
  ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))



/** Type definitions */
/*!
 * Generic communication function pointer
 * @param[in] dev_id: Place holder to store the id of the device structure
 *                    Can be used to store the index of the Chip select or
 *                    I2C address of the device.
 * @param[in] reg_addr:	Used to select the register the where data needs to
 *                      be read from or written to.
 * @param[in/out] reg_data: Data array to read/write
 * @param[in] len: Length of the data array
 */
typedef int8_t (*gmp102_com_fptr_t)(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);

/*!
 * Delay function pointer
 * @param[in] period: Time period in milliseconds
 */
typedef void (*gmp102_delay_fptr_t)(uint32_t period);


/*!
 * @brief Interface selection Enumerations
 */
enum gmp102_intf {
	/*! SPI interface */
	GMP102_SPI_INTF,
	/*! I2C interface */
	GMP102_I2C_INTF
};

/* structure definitions */
/*!
 * @brief Sensor field data structure
 */
struct	gmp102_field_data {
	
#if 1	
	/*! Temperature raw data */
	int16_t temperature;
    /*! Temperature in degree celsius */
	float temperature_Celsius;//float temperature_Celsius;
	/*! Pressure raw data */
	int32_t pressure;
	/*! calculate Pressure in Pascal */
	float pressure_Cal;//float pressure_Cal;
#else
	/*! Temperature raw data */
	float temperature;
    /*! Temperature in degree celsius */
	int16_t temperature_Celsius;//float temperature_Celsius;
	/*! Pressure raw data */
	float pressure;
	/*! calculate Pressure in Pascal */
	int16_t pressure_Cal;//float pressure_Cal;

#endif

};

struct	gmp102_calib_data 
{
	float par_[18];
};

/*!
 * @brief GMP102 sensor settings structure which comprises of ODR,
 * over-sampling and filter settings.
 */
struct	gmp102_tp_sett {
	
	/*! Temperature oversampling */
	uint8_t os_temp;
	/*! Pressure oversampling */
	uint8_t os_pres;
	/*! Filter coefficient */
	uint8_t filter;
};

/*!
 * @brief GMP102 device structure
 */
struct	gmp102_dev {
	/*! Chip Id */
	uint8_t chip_id;
	/*! Device Id */
	uint8_t dev_id;
	/*! Device Status */
	uint8_t dev_status;
	uint8_t dev_osr;
	uint8_t cali_test;
	
	/*! SPI/I2C interface */
	enum gmp102_intf intf;
	/*! Memory page used */
	uint8_t mem_page;
	/*! Ambient temperature in Degree C */
	int8_t amb_temp;
	/*! Sensor calibration data */
	float par_[18];//struct gmp102_calib_data calib;
	/*! Sensor settings */
	struct gmp102_tp_sett tp_sett;
	
	/*! Sensor power modes */
	uint8_t power_mode;
	/*! New sensor fields */
	uint8_t new_fields;
	/*! Store the info messages */
	uint8_t info_msg;
	/*! Bus read function pointer */
	gmp102_com_fptr_t read;
	/*! Bus write function pointer */
	gmp102_com_fptr_t write;
	/*! delay function pointer */
	gmp102_delay_fptr_t delay_ms;
	/*! Communication function result */
	int8_t com_rslt;
};

#endif /* BME680_DEFS_H_ */