/*
 * sensor_i2c.c
 *
 *  Created on: Dec 23, 2024
 *      Author: SANG HUYNH
 */

#include "sensor_i2c.h"
#include "board.h"

LSM6DSOX_Data_t LSM6DSOX_Data = {
    .Accel = {0, 0, 0},
    .Gyro =  {0, 0, 0}
};

BME280_Data_t BME280_Data = {
		.pressure = 0,
		.humidity = 0,
		.temperature = 0
};

H3LIS331DL_Data_t H3LIS331DL_Data = {
		.x = 0,
		.y = 0,
		.z = 0
};

H250_I2C_Data_t H250_I2C_Data = 0;
//H250_UART_Data_t H250_UART_Data;

K33_Data_t K33_Data = {
		.CO2 = 0,
		.Temp = 0,
		.RH = 0
};

Sens_List_Status_t Sensor_list = {
		.ntc = 0,
		.lsm = 0,
		.bmp = 0,
		.bme = 0,
		.h3l = 0,
		.h250 = 0,
		.k33 = 0,
		.sfc = 0,
};

void  Sensor_I2C_Init(void) {
	LL_GPIO_SetOutputPin(SENSOR1_EN_GPIO_Port, SENSOR1_EN_Pin);
	LL_GPIO_SetOutputPin(SENSOR2_EN_GPIO_Port, SENSOR2_EN_Pin);
	I2C_Status_t I2C_status = I2C_Error;

	LL_mDelay(100);
	I2C_status = LSM6DSOX_Init();
	if (I2C_status == I2C_Success) Sensor_list.lsm = 1;
	else Sensor_list.lsm = 0;

	LL_mDelay(100);
	I2C_status = BME280_Init();
	if (I2C_status == I2C_Success) Sensor_list.bme = 1;
	else Sensor_list.bme = 0;

	LL_mDelay(100);
	I2C_status = H3LIS331DL_Init();
	if (I2C_status == I2C_Success) Sensor_list.h3l = 1;
	else Sensor_list.h3l = 0;

	LL_mDelay(100);
	I2C_status = H250_I2C_Init();
	if (I2C_status == I2C_Success) Sensor_list.h250 = 1;
	else Sensor_list.h250 = 0;


#ifdef H250_UART
	H250_UART_Init(EXP_UART_CO2_HANDLE);
#endif

}
