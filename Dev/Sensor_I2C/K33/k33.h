/*
 * k33.h
 *
 *  Created on: May 7, 2025
 *      Author: HTSANG
 */

#ifndef SENSOR_I2C_K33_K33_H_
#define SENSOR_I2C_K33_K33_H_

#include "board.h"
#include "i2c.h"
#include "uart_driver.h"

#define K33_I2C_HANDLE		EXP_I2C_SENSOR_HANDLE

#define K33_I2C_ADDR		0x68
#define K33_WRITE_RAM_CMD	0x10
#define K33_READ_RAM_CMD	0x20
#define K33_WRITE_EE_CMD	0x30
#define K33_READ_EE_CMD		0x40

#define K33_RAM_CO2_ADDR	0x08
#define K33_RAM_TEMP_ADDR	0x12
#define K33_RAM_RH_ADDR		0x14

#define K33_RAM_CALIB_ADDR		0x67

typedef struct K33_RawData {
	uint8_t RxData[6];
	uint8_t Calib_status;
	I2C_Status_t status;
} K33_RawData_t;

typedef struct K33_Data {
	int16_t	CO2;
	int16_t Temp;			// two decimals (value xxxx = xx.xx *C)
	int16_t RH;				// two decimals (value xxxx = xx.xx %)
} K33_Data_t;

I2C_Status_t K33_BGrCalib(void);
I2C_Status_t K33_ZeroCalib(void);

I2C_Status_t K33_Read_Data(K33_Data_t *K33_Data);

#endif /* SENSOR_I2C_K33_K33_H_ */
