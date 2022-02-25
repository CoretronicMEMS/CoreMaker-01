/*
 *
 ****************************************************************************
 * Copyright (C) 2016 GlobalMEMS, Inc. <www.globalmems.com>
 * All rights reserved.
 *
 * File : gmp102.c
 *
 * Date : 2016/10/05
 *
 * Usage: GMP102 sensor driver header file
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
 
/*! @file gmp102.c
 *  @brief  GMP102 Sensor Driver File
 */
 
#include <stddef.h>
#include <math.h>
// #include <cmath>
// #include "nrf_error.h"
#include "gmp102.h"
 

#define WAIT_FOR_DRDY_LOOP_DELAY(count) {int i;for(i = 0; i < (count); ++i);}
 
bus_support_t* pGMP102Bus = 0;
 
static const float GMP102_CALIB_SCALE_FACTOR[] = {
  1.0E+00,
  1.0E-05,
  1.0E-10,
  1.0E-05,
  1.0E-10,
  1.0E-15,
  1.0E-12,
  1.0E-17,
  1.0E-21 };

static const s32 GMP102_POWER_SCALE[] = {1, 10, 100, 1000};

/*!
 * @brief Read multiple data from the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values read
 * @param u8Len Number of bytes to read
 * 
 * @return Result from the burst read function
 * @retval >= 0 Success
 * @retval -127 Error null bus
 * @retval < 0  Communication error
 *
 */
s8 gmp102_burst_read(u8 u8Addr, u8* pu8Data, u8 u8Len){
	
  s8 comRslt = -1;
  if(pGMP102Bus == NULL){
    return -127;
  }
  else{
    comRslt = pGMP102Bus->bus_read(pGMP102Bus->u8DevAddr, u8Addr, pu8Data, u8Len);
    // if(comRslt == NRF_SUCCESS) //success, return # of bytes read
    //   comRslt = u8Len;
    // else //return the nRF51 error code
    //   comRslt = -comRslt;
  }
	
  return comRslt;
}
 

/*!
 * @brief Write multiple data to the starting regsiter address
 *
 * @param u8Addr Starting register address
 * @param pu8Data The data array of values to write
 * @param u8Len Number of bytes to write
 * 
 * @return Result from the burst write function
 * @retval >= 0 Success
 * @retval -127 Error null bus
 * @retval < 0   Communication error
 *
 */
s8 gmp102_burst_write(u8 u8Addr, u8* pu8Data, u8 u8Len){
	
  s8 comRslt = -1;
  if(pGMP102Bus == NULL){
    return -127;
  }
  else{
    comRslt = pGMP102Bus->bus_write(pGMP102Bus->u8DevAddr, u8Addr, pu8Data, u8Len);
    // if(comRslt == NRF_SUCCESS) //success, return # of bytes write
    //   comRslt = u8Len;
    // else //return the nRF51 error code
    //   comRslt = -comRslt;
  }
	
  return comRslt;	
}

/*!
 * @brief GMP102 initialize communication bus
 *
 * @param pbus Pointer to the I2C/SPI read/write bus support struct
 * 
 * @return Result from bus communication function
 * @retval 0 Success
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_bus_init(bus_support_t* pbus){
	
  s8 comRslt = -1;
  u8 u8Data;

  //assign the I2C/SPI bus
  if(pbus == NULL)
    return -127;
  else
    pGMP102Bus = pbus;
	
  //Read chip ID
  comRslt = gmp102_burst_read(GMP102_REG_PID, &u8Data, 1);
  // 	u8Data = GMP102_SET_BITSLICE(u8Data, GMP102_P_OSR, GMP102_P_OSR_16384);
  // comRslt = gmp102_burst_write(GMP102_REG_CONFIG2, &u8Data, 1);
	// comRslt = gmp102_burst_read(GMP102_REG_CONFIG2, &u8Data, 1);
  return comRslt;
}
 
/*!
 * @brief GMP102 soft reset
 *
 * @param None
 * 
 * @return Result from bus communication function
 * @retval -1 Bus communication error
 * @retval -127 Error null bus
 *
 */
s8 gmp102_soft_reset(void){
	
  s8 comRslt = -1;
  u8 u8Data = GMP102_SW_RST_SET_VALUE;
	
  //Set 00h = 0x24
  comRslt = gmp102_burst_write(GMP102_RST__REG, &u8Data, 1);
	
  return comRslt;
}

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
s8 gmp102_get_calibration_param(float* fCalibParam){
  u8 u8DataBuf[GMP102_CALIBRATION_REGISTER_COUNT];
  s8 comRslt;
  s32 tmp, shift, i;
  //read the calibration registers
  comRslt = gmp102_burst_read(GMP102_REG_CALIB00, u8DataBuf, GMP102_CALIBRATION_REGISTER_COUNT);
	// printf("comRslt = %d\n",comRslt);
  // if(comRslt < GMP102_CALIBRATION_REGISTER_COUNT){
  //   comRslt = -1;
  //   goto EXIT;
  // }
	
  // Get the parameters
  shift = sizeof(s32)*8 - 16;
  for(i = 0; i < GMP102_CALIBRATION_PARAMETER_COUNT; ++i){
    tmp = (u8DataBuf[2 * i] << 8) + u8DataBuf[2 * i + 1];
    fCalibParam[i] = ((tmp << shift) >> (shift + 2)) * (pow(10.0, (u8DataBuf[2 * i + 1] & 0x03))) * GMP102_CALIB_SCALE_FACTOR[i];
    // printf("fCalibParam %d = %f\n",i,fCalibParam[i]);
  }
	
 EXIT:
  return comRslt;
}

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
s8 gmp102_get_calibration_param_fixed_point(s16 s16Value[], u8 u8Power[]){

  u8 u8DataBuf[GMP102_CALIBRATION_REGISTER_COUNT];
  s8 comRslt;
  s16 tmp, i;
	
  //read the calibration registers
  comRslt = gmp102_burst_read(GMP102_REG_CALIB00, u8DataBuf, GMP102_CALIBRATION_REGISTER_COUNT);
	
  if(comRslt < GMP102_CALIBRATION_REGISTER_COUNT){
    comRslt = -1;
    goto EXIT;
  }

  for(i = 0; i < GMP102_CALIBRATION_PARAMETER_COUNT; ++i){
    tmp = (u8DataBuf[2 * i] << 8) + u8DataBuf[2 * i + 1];
    s16Value[i] = (tmp>>2);
    u8Power[i] = (tmp & 0x03);
  }

 EXIT:
  return comRslt;
}

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
s8 gmp102_initialization(void){
	
  s8 comRslt = 0, s8Tmp;
  u8 u8Data[] = {0, 0, 0, 0};
	
  //Set AAh ~ AD to 0x00
  s8Tmp = gmp102_burst_write(GMP102_REG_CALIB00, u8Data, 4);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
 EXIT:
  return comRslt;
	
}

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
s8 gmp102_measure_T(s16* ps16T){
  s8 comRslt = 0, s8Tmp;
  u8 u8Data[2];
	
  // Set A5h = 0x00, Calibrated data out
  u8Data[0] = 0x00;
  s8Tmp = gmp102_burst_write(GMP102_REG_CONFIG1, u8Data, 1);
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
  // Set 30h = 0x08, T-Forced mode
  u8Data[0] = 0x08;
  s8Tmp = gmp102_burst_write(GMP102_REG_CMD, u8Data, 1);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
  // Wait for 02h[0] DRDY bit set
  do{
    //wait a while
    WAIT_FOR_DRDY_LOOP_DELAY(1000)

    s8Tmp = gmp102_burst_read(GMP102_REG_STATUS, u8Data, 1);

    if(s8Tmp < 0){ //communication error
      comRslt = s8Tmp;
      goto EXIT;
    }
    comRslt += s8Tmp;	

  } while( GMP102_GET_BITSLICE(u8Data[0], GMP102_DRDY) != 1);
	
  // Read 09h~0Ah
  s8Tmp = gmp102_burst_read(GMP102_REG_TEMPH, u8Data, 2);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;	
	
  // Get the calibrated temperature in code
  *ps16T = (u8Data[0] << 8) + u8Data[1];
	
 EXIT:
  return comRslt;
}

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
s8 gmp102_measure_P(s32* ps32P){

  s8 comRslt = 0, s8Tmp;
  u8 u8Data[3];
	
  // Set A5h = 0x02, raw data out
  u8Data[0] = 0x02;
  s8Tmp = gmp102_burst_write(GMP102_REG_CONFIG1, u8Data, 1);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
  // Set 30h = 0x09, P-Forced mode
  u8Data[0] = 0x09;
  s8Tmp = gmp102_burst_write(GMP102_REG_CMD, u8Data, 1);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
  // Wait for 02h[0] DRDY bit set
  do{

    //wait a while
    WAIT_FOR_DRDY_LOOP_DELAY(1000)
		
    s8Tmp = gmp102_burst_read(GMP102_REG_STATUS, u8Data, 1);

    if(s8Tmp < 0){ //communication error
      comRslt = s8Tmp;
      goto EXIT;
    }
    comRslt += s8Tmp;		
		
  } while( GMP102_GET_BITSLICE(u8Data[0], GMP102_DRDY) != 1);
	
  // Read 06h~08h
  s8Tmp = gmp102_burst_read(GMP102_REG_PRESSH, u8Data, 3);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;	
	
  s8Tmp = sizeof(*ps32P)*8 - 24;
  // Get the raw pressure in code
  *ps32P = (u8Data[0] << 16) + (u8Data[1] << 8) + u8Data[2];
  *ps32P = (*ps32P << s8Tmp) >> s8Tmp; //24 bit sign extension
	
 EXIT:
  return comRslt;
}

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
s8 gmp102_measure_P_T(s32* ps32P, s16* ps16T, s8 s8PWaitDrdy){

  s8 comRslt = 0, s8Tmp;
  u8 u8Data[3];
	
  /*
   *
   * Read raw P code
   *
   */
  if(s8PWaitDrdy){
    // Wait for 02h[0] DRDY bit set if s8PWaitDrdy is 1
    do{

      //wait a while
      WAIT_FOR_DRDY_LOOP_DELAY(1000)
		
      s8Tmp = gmp102_burst_read(GMP102_REG_STATUS, u8Data, 1);

      if(s8Tmp < 0){ //communication error
	comRslt = s8Tmp;
	goto EXIT;
      }
      comRslt += s8Tmp;		
		
    } while( GMP102_GET_BITSLICE(u8Data[0], GMP102_DRDY) != 1);
  }
	
  // Read 06h~08h
  s8Tmp = gmp102_burst_read(GMP102_REG_PRESSH, u8Data, 3);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;	
	
  s8Tmp = sizeof(*ps32P)*8 - 24;
  // Get the raw pressure in code
  *ps32P = (u8Data[0] << 16) + (u8Data[1] << 8) + u8Data[2];
  *ps32P = (*ps32P << s8Tmp) >> s8Tmp; //24 bit sign extension
	
  /*
   *
   * Measure calibrated T code
   *
   */
  // Set A5h = 0x00, Calibrated data out
  u8Data[0] = 0x00;
  s8Tmp = gmp102_burst_write(GMP102_REG_CONFIG1, u8Data, 1);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
  // Set 30h = 0x08, T-Forced mode
  u8Data[0] = 0x08;
  s8Tmp = gmp102_burst_write(GMP102_REG_CMD, u8Data, 1);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
  // Wait for 02h[0] DRDY bit set
  do{

    //wait a while
    WAIT_FOR_DRDY_LOOP_DELAY(1000)
		
      s8Tmp = gmp102_burst_read(GMP102_REG_STATUS, u8Data, 1);

    if(s8Tmp < 0){ //communication error
      comRslt = s8Tmp;
      goto EXIT;
    }
    comRslt += s8Tmp;		
		
  } while( GMP102_GET_BITSLICE(u8Data[0], GMP102_DRDY) != 1);
	
  // Read 09h~0Ah
  s8Tmp = gmp102_burst_read(GMP102_REG_TEMPH, u8Data, 2);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;	
	
  // Get the calibrated temperature in code
  *ps16T = (u8Data[0] << 8) + u8Data[1];
	
  /*
   *
   * Commit the next pressure conversion
   *
   */
  // Set A5h = 0x02, raw data out
  u8Data[0] = 0x02;
  s8Tmp = gmp102_burst_write(GMP102_REG_CONFIG1, u8Data, 1);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
  // Set 30h = 0x09, P-Forced mode
  u8Data[0] = 0x09;
  s8Tmp = gmp102_burst_write(GMP102_REG_CMD, u8Data, 1);

  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
 EXIT:
  return comRslt;
}

/*!
 * @brief gmp102 temperature and pressure compensation
 *
 * @param s16T calibrated temperature in code
 * @param s32P raw pressure in code
 * @param fParam[] pressure calibration parameters
 * @param *pfT_Celsius calibrated temperature in Celsius returned to caller
 * @param *pfP_Pa calibraated pressure in Pa returned to caller
 * 
 * @return None
 *
 */
void gmp102_compensation(s16 s16T, s32 s32P, float fParam[], float* pfT_Celsius, float* pfP_Pa){
	
  *pfT_Celsius = GMP102_T_CODE_TO_CELSIUS(s16T);

  *pfP_Pa = \
    fParam[0] + \
    fParam[1]*s16T + \
    fParam[2]*s16T*s16T + \
    fParam[3]*s32P + \
    fParam[4]*s16T*s32P + \
    fParam[5]*s16T*s16T*s32P + \
    fParam[6]*s32P*s32P + \
    fParam[7]*s16T*s32P*s32P + \
    fParam[8]*s16T*s16T*s32P*s32P;
}

#define ShiftRight(v, s) (((v)+(1<<((s)-1)))>>(s))
#define RoundDivide(v, d) (((v)+((d)/2))/(d))

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
void gmp102_compensation_fixed_point_s64(s16 s16T, s32 s32P, s16 s16Value[], u8 u8Power[], s32* ps32T_Celsius, s32* ps32P_Pa){

  s64 tmp, val, s64T, s64P;
  s64T = s16T;
  s64P = s32P;

  //Temperature
  *ps32T_Celsius = s16T;

  //Pressure
  val = 0;
  //beta0
  tmp = s16Value[0] * GMP102_POWER_SCALE[u8Power[0]] * 10;
  val += tmp;
  //beta1*T
  tmp = s64T * s16Value[1];
  tmp = tmp * GMP102_POWER_SCALE[u8Power[1]];
  tmp = RoundDivide(tmp, 10000);
  val += tmp;
  //beta2*T*T
  tmp = s64T * s16Value[2];
  tmp = tmp * s64T;
  tmp = tmp * GMP102_POWER_SCALE[u8Power[2]];
  tmp = RoundDivide(tmp, 1000000000);
  val += tmp;
  //beta3*P
  tmp = s64P * s16Value[3];
  tmp = tmp * GMP102_POWER_SCALE[u8Power[3]];
  tmp = RoundDivide(tmp, 10000);
  val += tmp;
  //beta4*P*T
  tmp = s64P * s16Value[4];
  tmp = tmp * s64T;
  tmp = tmp * GMP102_POWER_SCALE[u8Power[4]];
  tmp = RoundDivide(tmp, 1000000000);
  val += tmp;
  //beta5*P*T*T
  tmp = s64P * s16Value[5];
  tmp = tmp * s64T;
  tmp = ShiftRight(tmp, 10) * s64T;
  tmp = ShiftRight(tmp, 10) * GMP102_POWER_SCALE[u8Power[5]];
  tmp = RoundDivide(tmp, 95367432);
  val += tmp;
  //beta6*P*P
  tmp = s64P * s16Value[6];
  tmp = tmp * s64P;
  tmp = ShiftRight(tmp, 7) * GMP102_POWER_SCALE[u8Power[6]];
  tmp = RoundDivide(tmp, 781250000);
  val += tmp;
  //beta7*P*P*T
  tmp = s64P * s16Value[7];
  tmp = tmp * s64P;
  tmp = ShiftRight(tmp, 10) * s64T;
  tmp = ShiftRight(tmp, 10) * GMP102_POWER_SCALE[u8Power[7]];
  tmp = RoundDivide(tmp, 9536743164);
  val += tmp;
  //beta8*P*P*T*T
  tmp = s64P * s16Value[8];
  tmp = tmp * s64P;
  tmp = ShiftRight(tmp, 9) * ShiftRight(s64T, 1);
  tmp = ShiftRight(tmp, 12) * ShiftRight(s64T, 3);
  tmp = ShiftRight(tmp, 7) * GMP102_POWER_SCALE[u8Power[8]];
  tmp = RoundDivide(tmp, 23283064365);
  val += tmp;

  *ps32P_Pa = (s32)RoundDivide(val, 10);

  return;
}

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
void gmp102_compensation_fixed_point_s32(s16 s16T, s32 s32P, s16 s16Value[], u8 u8Power[], s32* ps32T_Celsius, s32* ps32P_Pa){

  s32 tmp, val, s32T, tmpT1, tmpT3, tmpP4, tmpP10, tmpP12, tmpP13;
  s32T = s16T;

  //some temporary variables
  tmpT1 = ShiftRight(s32T, 1);
  tmpT3 = ShiftRight(s32T, 3);
  tmpP4 = ShiftRight(s32P, 4);
  tmpP10 = ShiftRight(s32P, 10);
  tmpP12 = ShiftRight(s32P, 12);
  tmpP13 = ShiftRight(s32P, 13);

  //Temperature
  *ps32T_Celsius = s16T;

  //Pressure
  val = 0;
  //beta0
  tmp = s16Value[0] * GMP102_POWER_SCALE[u8Power[0]];
  val += tmp;
  //beta1*T
  tmp = s32T * s16Value[1];
  tmp = ShiftRight(tmp, 5) * GMP102_POWER_SCALE[u8Power[1]];
  tmp = RoundDivide(tmp, 3125);
  val += tmp;
  //beta2*T*T
  tmp = s32T * s16Value[2];
  tmp = ShiftRight(tmp, 8) * tmpT1;
  tmp = ShiftRight(tmp, 9) * GMP102_POWER_SCALE[u8Power[2]];
  tmp = RoundDivide(tmp, 38147);
  val += tmp;
  //beta3*P
  tmp = tmpP4 * s16Value[3];
  tmp = ShiftRight(tmp, 5) * GMP102_POWER_SCALE[u8Power[3]];
  tmp = RoundDivide(tmp, 195);
  val += tmp;
  //beta4*P*T
  tmp = tmpP10 * s16Value[4];
  tmp = ShiftRight(tmp, 13) * tmpT3;
  tmp = tmp * GMP102_POWER_SCALE[u8Power[4]];
  tmp = RoundDivide(tmp, 149);
  val += tmp;
  //beta5*P*T*T
  tmp = tmpP13 * s16Value[5];
  tmp = ShiftRight(tmp, 9) * s32T;
  tmp = ShiftRight(tmp, 10) * s32T;
  tmp = ShiftRight(tmp, 10) * GMP102_POWER_SCALE[u8Power[5]];
  tmp = RoundDivide(tmp, 227);
  val += tmp;
  //beta6*P*P
  tmp = tmpP10 * s16Value[6];
  tmp = ShiftRight(tmp, 2) * tmpP12;
  tmp = ShiftRight(tmp, 10) * GMP102_POWER_SCALE[u8Power[6]];
  tmp = RoundDivide(tmp, 58);
  val += tmp;
  //beta7*P*P*T
  tmp = tmpP10 * s16Value[7];
  tmp = ShiftRight(tmp, 1) * tmpP13;
  tmp = ShiftRight(tmp, 13) * s32T;
  tmp = ShiftRight(tmp, 10) * GMP102_POWER_SCALE[u8Power[7]];
  tmp = RoundDivide(tmp, 711);
  val += tmp;
  //beta8*P*P*T*T
  tmp = tmpP12 * s16Value[8];
  tmp = ShiftRight(tmp, 2) * tmpP12;
  tmp = ShiftRight(tmp, 12) * tmpT1;
  tmp = ShiftRight(tmp, 12) * tmpT1;
  tmp = ShiftRight(tmp, 8) * GMP102_POWER_SCALE[u8Power[8]];
  tmp = RoundDivide(tmp, 867);
  val += tmp;

  *ps32P_Pa = val;

  return;
}

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
s8 gmp102_set_P_OSR(GMP102_P_OSR_Type osrP){
	
  s8 comRslt = 0, s8Tmp;
  u8 u8Data;
	
  //Read A6h
  s8Tmp = gmp102_burst_read(GMP102_REG_CONFIG2, &u8Data, 1);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;	

  //Set the A6h[2:0] OSR bits
  u8Data = GMP102_SET_BITSLICE(u8Data, GMP102_P_OSR, osrP);
  s8Tmp = gmp102_burst_write(GMP102_REG_CONFIG2, &u8Data, 1);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
 EXIT:
  return comRslt;
}


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
s8 gmp102_set_T_OSR(GMP102_T_OSR_Type osrT){
	
  s8 comRslt = 0, s8Tmp;
  u8 u8Data;
	
  //Read A7h
  s8Tmp = gmp102_burst_read(GMP102_REG_CONFIG3, &u8Data, 1);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;	

  //Set the A7h[2:0] OSR bits
  u8Data = GMP102_SET_BITSLICE(u8Data, GMP102_T_OSR, osrT);
  s8Tmp = gmp102_burst_write(GMP102_REG_CONFIG3, &u8Data, 1);
	
  if(s8Tmp < 0){ //communication error
    comRslt = s8Tmp;
    goto EXIT;
  }
  comRslt += s8Tmp;
	
 EXIT:
  return comRslt;
}
