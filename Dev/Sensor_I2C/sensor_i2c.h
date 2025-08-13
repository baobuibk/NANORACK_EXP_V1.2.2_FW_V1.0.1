/*
 * sensor_i2c.h
 *
 *  Created on: Dec 23, 2024
 *      Author: SANG HUYNH
 */

#ifndef SENSOR_I2C_SENSOR_I2C_H_
#define SENSOR_I2C_SENSOR_I2C_H_

#include "i2c.h"
#include "sensor_i2c.h"
#include "lsm6dsox.h"
#include "bmp390.h"
#include "bme280.h"
#include "h3lis331dl.h"
#include "h250.h"
#include "k33.h"

typedef struct Sens_List_Status {
	uint8_t ntc;
	uint8_t lsm;
	uint8_t bmp;
	uint8_t bme;
	uint8_t h3l;
	uint8_t h250;
	uint8_t k33;
	uint8_t sfc;
} Sens_List_Status_t;


extern LSM6DSOX_Data_t LSM6DSOX_Data;
extern BME280_Data_t BME280_Data;
extern H3LIS331DL_Data_t H3LIS331DL_Data;
extern H250_I2C_Data_t H250_I2C_Data;
extern K33_Data_t K33_Data;
extern Sens_List_Status_t Sensor_list;

void Sensor_I2C_Init(void);

#endif /* SENSOR_I2C_SENSOR_I2C_H_ */
