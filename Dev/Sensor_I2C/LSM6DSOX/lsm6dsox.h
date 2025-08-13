/*
 * LSM6DSOX.h
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG
 */

#ifndef SENSOR_I2C_LSM6DSOX_LSM6DSOX_H_
#define SENSOR_I2C_LSM6DSOX_LSM6DSOX_H_

#include "stdint.h"
#include "i2c.h"

#define LSM6DSOX_ID		            0x6C

#define LSM6DSOX_ID_ADDR			0x0F
#define LSM6DSOX_ADDRESS            0x6A

#define LSM6DSOX_CTRL1_XL           0X10
#define LSM6DSOX_CTRL2_G            0X11

#define LSM6DSOX_STATUS_REG         0X1E

#define LSM6DSOX_CTRL3_C            0X12
#define LSM6DSOX_CTRL6_C            0X15
#define LSM6DSOX_CTRL7_G            0X16
#define LSM6DSOX_CTRL8_XL           0X17

#define LSM6DSOX_OUTX_L_G           0X22

#define GYRO_SENSITIVITY_500DPS 	0.01750f 		// dps/LSB
#define ACCEL_SENSITIVITY_8G		0.000244f		// g / LSB

typedef struct LSM6DSOX_RawData {
	uint8_t RxData[12];
	I2C_Status_t status;
} LSM6DSOX_RawData_t;

typedef struct Accel_Gyro {
	int16_t x;
	int16_t y;
	int16_t z;
} Accel_Gyro_t;

typedef struct LSM6DSOX_Data {
	Accel_Gyro_t Accel;
	Accel_Gyro_t Gyro;
} LSM6DSOX_Data_t;


I2C_Status_t LSM6DSOX_Init(void);
I2C_Status_t LSM6DSOX_Read_ID(uint8_t *ID);
I2C_Status_t LSM6DSOX_Read_Data(LSM6DSOX_Data_t* LSM6DSOX_Data);

#endif /* SENSOR_I2C_LSM6DSOX_LSM6DSOX_H_ */
