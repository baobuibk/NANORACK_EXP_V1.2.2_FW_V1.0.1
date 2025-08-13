/*
 * bme280.h
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG
 */

#ifndef SENSOR_I2C_BME280_BME280_H_
#define SENSOR_I2C_BME280_BME280_H_

#include "main.h"
#include "i2c.h"
#include "stdbool.h"
#include "board.h"

#define BME280_ID					0x60
//#define BME280_I2C_ADDR			0x76	//with SDO connect to GND
#define BME280_I2C_ADDR				0x77	//with SDO connect to VCC

#define BME280_CALIB_REG_1			0x88
#define BME280_CALIB_REG_2			0xE1

#define BME280_ID_REG				0xD0
#define BME280_RESET_REG			0xE0
#define BME280_CTRL_HUM_REG			0xF2
#define BME280_STATUS_REG			0xF3
#define BME280_CTRL_MEAS_REG		0xF4
#define BME280_CONFIG_REG			0xF5
#define BME280_PRESS_REG			0xF7	// 0xF7  0xF8  0xF9 : msb  lsb  xlsb
#define BME280_TEMP_REG				0xFA	// 0xFA  0xFB  0xFC : msb  lsb  xlsb
#define BME280_HUM_REG				0xFD	// 0xFD  0xFE		: msb  lsb

typedef struct BME280_calib {
	uint16_t dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;

	uint16_t dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;

	uint8_t  dig_H1;
	int16_t  dig_H2;
	uint8_t  dig_H3;
	int16_t  dig_H4;
	int16_t  dig_H5;
	int8_t   dig_H6;
} BME280_calib_t;

typedef struct BME280_RawData
{
	int32_t temp;
	int32_t press;
	int16_t humi;
	I2C_Status_t status;
} BME280_RawData_t;

typedef struct BME280_Data {
	float temperature;  // độ C
	float pressure;     // hPa
	float humidity;     // %RH
} BME280_Data_t;

I2C_Status_t BME280_Init(void);
I2C_Status_t BME280_Read_ID(uint8_t *ID);
I2C_Status_t BME280_Read_Data(BME280_Data_t *BME280_Data);

#endif /* SENSOR_I2C_BME280_BME280_H_ */
