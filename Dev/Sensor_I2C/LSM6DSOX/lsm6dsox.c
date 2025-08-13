/*
 * lsm6dsox.c
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG
 */


#include "lsm6dsox.h"
#include "board.h"
#include "i2c.h"
#include "math.h"

static LSM6DSOX_RawData_t LSM6DSOX_RawData = {
	.status = 0,
};

void LSM6DSOX_Write(uint8_t reg, uint8_t data) {
	LSM6DSOX_RawData.status = I2C_Write(EXP_I2C_SENSOR_HANDLE, LSM6DSOX_ADDRESS, reg, data);
}

void LSM6DSOX_Read(uint8_t reg, uint8_t *pData, uint8_t length) {
	LSM6DSOX_RawData.status = I2C_ReadMulti(EXP_I2C_SENSOR_HANDLE, LSM6DSOX_ADDRESS, reg, pData, length);
}

I2C_Status_t LSM6DSOX_Init(void) {
	// Accelerometer: 104 Hz, 8g ~ 78.48 m/s^2
	LSM6DSOX_Write(LSM6DSOX_CTRL1_XL, 0x58);
	// Gyroscope: 104 Hz, 500 dps
	LSM6DSOX_Write(LSM6DSOX_CTRL2_G, 0x54);
	uint8_t ID = 0;
	LSM6DSOX_Read_ID(&ID);
	if (ID != LSM6DSOX_ID) LSM6DSOX_RawData.status = I2C_Error;
	return LSM6DSOX_RawData.status;
}

I2C_Status_t LSM6DSOX_Read_ID(uint8_t *ID) {
	LSM6DSOX_Read(LSM6DSOX_ID_ADDR, ID, 1);
	return LSM6DSOX_RawData.status;
}

I2C_Status_t LSM6DSOX_Read_Data(LSM6DSOX_Data_t* LSM6DSOX_Data)
{
	if (LSM6DSOX_RawData.status == I2C_Error) LSM6DSOX_Init();
    LSM6DSOX_Read(LSM6DSOX_OUTX_L_G, LSM6DSOX_RawData.RxData, 12);

    int16_t gx = (int16_t)(LSM6DSOX_RawData.RxData[1] << 8 | LSM6DSOX_RawData.RxData[0]);
    int16_t gy = (int16_t)(LSM6DSOX_RawData.RxData[3] << 8 | LSM6DSOX_RawData.RxData[2]);
    int16_t gz = (int16_t)(LSM6DSOX_RawData.RxData[5] << 8 | LSM6DSOX_RawData.RxData[4]);
    int16_t ax = (int16_t)(LSM6DSOX_RawData.RxData[7] << 8 | LSM6DSOX_RawData.RxData[6]);
    int16_t ay = (int16_t)(LSM6DSOX_RawData.RxData[9] << 8 | LSM6DSOX_RawData.RxData[8]);
    int16_t az = (int16_t)(LSM6DSOX_RawData.RxData[11] << 8 | LSM6DSOX_RawData.RxData[10]);

    LSM6DSOX_Data->Gyro.x = gx * GYRO_SENSITIVITY_500DPS;
    LSM6DSOX_Data->Gyro.y = gy * GYRO_SENSITIVITY_500DPS;
    LSM6DSOX_Data->Gyro.z = gz * GYRO_SENSITIVITY_500DPS;

    LSM6DSOX_Data->Accel.x = ax * ACCEL_SENSITIVITY_8G;
    LSM6DSOX_Data->Accel.y = ay * ACCEL_SENSITIVITY_8G;
    LSM6DSOX_Data->Accel.z = az * ACCEL_SENSITIVITY_8G;

    return LSM6DSOX_RawData.status;
}
