/*
 *
 ****************************************************************************
 * Copyright (C) 2016 GlobalMEMS, Inc. <www.globalmems.com>
 * All rights reserved.
 *
 * File : gmp102.h
 *
 * Date : 2016/09/21
 *
 * Usage: Sensor Driver file for GMP102 sensor
 *
 ****************************************************************************
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 **************************************************************************/

/*! @file gmp102.h
 *  @brief  GMP102 Sensor Driver Header File 
 */
 
#ifndef __GMP102_H__
#define __GMP102_H__

#include "type_support.h"

#define GMP102_7BIT_I2C_ADDR		0x6C
#define GMP102_TEMPERATURE_SENSITIVITY 256  //1 Celsius = 256 code
#define GMP102_T_CODE_TO_CELSIUS(tCode) (((float)(tCode)) / GMP102_TEMPERATURE_SENSITIVITY)

//Registers Address
#define GMP102_REG_RESET	  0x00
#define GMP102_REG_PID 	          0x01
#define GMP102_REG_STATUS 	  0x02
#define GMP102_REG_PRESSH	  0x06
#define GMP102_REG_PRESSM	  0x07
#define GMP102_REG_PRESSL	  0x08
#define GMP102_REG_TEMPH	  0x09
#define GMP102_REG_TEMPL	  0x0A
#define GMP102_REG_CMD	 	  0x30
#define GMP102_REG_CONFIG1 	  0xA5
#define GMP102_REG_CONFIG2 	  0xA6
#define GMP102_REG_CONFIG3 	  0xA7
#define GMP102_REG_CALIB00        0xAA
//Total calibration register count: AAh~BBh total 18
#define GMP102_CALIBRATION_REGISTER_COUNT 18
//Total calibration parameter count: total 9
#define GMP102_CALIBRATION_PARAMETER_COUNT (GMP102_CALIBRATION_REGISTER_COUNT/2)
//Soft reset 
#define GMP102_SW_RST_SET_VALUE		0x24

/* PID */
#define GMP102_PID__REG GMP102_REG_PID
/* Soft Rest bit */
#define GMP102_RST__REG		GMP102_REG_RESET
#define GMP102_RST__MSK		0x24
#define GMP102_RST__POS		0
/* DRDY bit */
#define GMP102_DRDY__REG	GMP102_REG_STATUS
#define GMP102_DRDY__MSK	0x01
#define GMP102_DRDY__POS	0
/* P OSR bits */
#define GMP102_P_OSR__REG       GMP102_REG_CONFIG2
#define GMP102_P_OSR__MSK       0x07
#define GMP102_P_OSR__POS       0
/* T OSR bits */
#define GMP102_T_OSR__REG       GMP102_REG_CONFIG3
#define GMP102_T_OSR__MSK       0x07
#define GMP102_T_OSR__POS       0

#define GMP102_GET_BITSLICE(regvar, bitname)	\
  ((regvar & bitname##__MSK) >> bitname##__POS)

#define GMP102_SET_BITSLICE(regvar, bitname, val)			\
  ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

//I2C bus read write function definition
#define BUS_RD_FUNC_PTR s8(*bus_read)(u8, u8, u8*, u8)
#define BUS_WR_FUNC_PTR s8(*bus_write)(u8, u8, u8*, u8)
#define BUS_READ_FUNC(u8DevAddr, u8RegAddr, pu8RegData, u8Len) bus_read(u8DevAddr, u8RegAddr, pu8RegData, u8Len)
#define BUS_WRITE_FUNC(u8DevAddr, u8RegAddr, pu8RegData, u8Len) bus_write(u8DevAddr, u8RegAddr, pu8RegData, u8Len)

typedef struct {
		u8 u8DevAddr;
		BUS_WR_FUNC_PTR;
		BUS_RD_FUNC_PTR;
} bus_support_t;

typedef enum {
  GMP102_P_OSR_256 = 0x04,
  GMP102_P_OSR_512 = 0x05,
  GMP102_P_OSR_1024 = 0x00,
  GMP102_P_OSR_2048 = 0x01,
  GMP102_P_OSR_4096 = 0x02,
  GMP102_P_OSR_8192 = 0x03,
  GMP102_P_OSR_16384 = 0x06,
  GMP102_P_OSR_32768 = 0x07,	
} GMP102_P_OSR_Type;

typedef enum {
  GMP102_T_OSR_256 = 0x04,
  GMP102_T_OSR_512 = 0x05,
  GMP102_T_OSR_1024 = 0x00,
  GMP102_T_OSR_2048 = 0x01,
  GMP102_T_OSR_4096 = 0x02,
  GMP102_T_OSR_8192 = 0x03,
  GMP102_T_OSR_16384 = 0x06,
  GMP102_T_OSR_32768 = 0x07,	
} GMP102_T_OSR_Type;

/*!
 * @brief Read multiple data from the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values read
 * @param u8Len Number of bytes to read
 * 
 * @return Result from the burst read function
 * @retval >= 0 Success, number of bytes read
 * @retval -127 Error null bus
 * @retval -1   Bus communication error
 *
 */
s8 gmp102_burst_read(u8 u8Addr, u8* pu8Data, u8 u8Len);

/*!
 * @brief Write multiple data to the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values to write
 * @param u8Len Number of bytes to write
 * 
 * @return Result from the burst write function
 * @retval >= 0 Success, number of bytes write
 * @retval -127 Error null bus
 * @retval -1   Communication error
 *
 */
s8 gmp102_burst_write(u8 u8Addr, u8* pu8Data, u8 u8Len);


/*!
 * @brief gmp102 initialize communication bus
 *
 * @param pbus Pointer to the I2C/SPI read/write bus support struct
 * 
 * @return Result from bus communication function
 * @retval 0 Success
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_bus_init(bus_support_t* pbus);

/*!
 * @brief gmp102 soft reset
 *
 * @param None
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_soft_reset(void);


/*!
 * @brief Get gmp102 calibration parameters
 *        - Read calibration register AAh~BBh total 18 bytes 
 *        - Compose 9 calibration parameters from the 18 bytes
 *
 * @param fCalibParam: the calibration parameter array returned to caller
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_get_calibration_param(float* fCalibParam);

/*!
 * @brief Get gmp102 calibration parameters for fixed-point compensation
 *        - Read calibration register AAh~BBh total 18 bytes
 *        - Return 9 calibration parameters with fixed-point value and power parts
 *
 * @param s16Value[]: array of the value part of the calibration parameter
 * @param u8Power[]: array of the power part of the calibration parameter
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_get_calibration_param_fixed_point(s16 s16Value[], u8 u8Power[]);

/*!
 * @brief gmp102 initialization
 *        Set AAh ~ ADh to 0x00
 *
 * @param None
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_initialization(void);


/*!
 * @brief gmp102 measure temperature
 *
 * @param *ps16T calibrated temperature code returned to caller
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_measure_T(s16* ps16T);

/*!
 * @brief gmp102 measure pressure
 *
 * @param *ps32P raw pressure in code returned to caller
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_measure_P(s32* ps32P);

/*!
 * @brief gmp102 measure pressure and temperature
 *        Read pressure first then commit pressure data conversion for the next call
 *        
 * @param *ps32P raw pressure in code returned to caller
 * @param *ps16T calibrated temperature code returned to caller
 * @param s8WaitPDrdy 1: P wait for DRDY bit set, 0: P no wait
 *
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_measure_P_T(s32* ps32P, s16* ps16T, s8 s8PWaitDrdy);

/*!
 * @brief gmp102 temperature and pressure compensation
 *
 * @param s16T calibrated temperature in code
 * @param s32P raw pressure in code
 * @param fParam[] pressure calibration parameters
 * @param *pfT_Celsius calibrated temperature in Celsius returned to caller
 * @param *pfP_Pa calibrated pressure in Pa returned to caller
 * 
 * @return None
 *
 */
void gmp102_compensation(s16 s16T, s32 s32P, float fParam[], float* pfT_Celsius, float* pfP_Pa);

/*!
 * @brief gmp102 temperature and pressure compensation, s64 fixed point operation
 *
 * @param s16T raw temperature in code
 * @param s32P raw pressure in code
 * @param s16Value[]: array of the value part of the calibration parameter
 * @param u8Power[]: array of the power part of the calibration parameter
 * @param *ps32T_Celsius calibrated temperature in 1/256*Celsius returned to caller
 * @param *ps32P_Pa calibrated pressure in Pa returned to caller
 * 
 * @return None
 *
 */
void gmp102_compensation_fixed_point_s64(s16 s16T, s32 s32P, s16 s16Value[], u8 u8Power[], s32* ps32T_Celsius, s32* ps32P_Pa);

/*!
 * @brief gmp102 temperature and pressure compensation, s32 fixed point operation
 *
 * @param s16T raw temperature in code
 * @param s32P raw pressure in code
 * @param s16Value[]: array of the value part of the calibration parameter
 * @param u8Power[]: array of the power part of the calibration parameter
 * @param *ps32T_Celsius calibrated temperature in 1/256*Celsius returned to caller
 * @param *ps32P_Pa calibrated pressure in Pa returned to caller
 *
 * @return None
 *
 */
void gmp102_compensation_fixed_point_s32(s16 s16T, s32 s32P, s16 s16Value[], u8 u8Power[], s32* ps32T_Celsius, s32* ps32P_Pa);

/*!
 * @brief gmp102 set pressure OSR
 *
 * @param osrP OSR to set
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_set_P_OSR(GMP102_P_OSR_Type osrP);

/*!
 * @brief gmp102 set temperature OSR
 *
 * @param osrT OSR to set
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_set_T_OSR(GMP102_T_OSR_Type osrT);

#endif // __GMP102_H__
