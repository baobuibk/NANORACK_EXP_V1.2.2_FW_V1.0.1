///*
// * bmp390.c
// *
// *  Created on: Dec 16, 2024
// *      Author: SANG HUYNH
// */
//
//#include "bmp390.h"
//#include "board.h"
//
//// BMP390 address
//#define BMP390_I2C_ADDR					0x77
//#define BMP390_CALIB_DATA_ADDR			0x31 // Starting address for calibration data in BMP390
//#define BMP390_TEMP_PRESS_DATA_ADDR		0x04 // Starting address for temperature and pressure data
//#define BMP390_REG_PWR_CTRL				0x1B
//
//// BMP390 data
//BMP390_Data TempPress_data;
//
//bool BMP390_read_raw_calibration(BMP390_Data *data)
//{
//	uint8_t calib[21];
//	uint8_t err = 0;
//	// Receive calibration data
//	for (uint8_t i = 0; i < 21; i++)
//	{
//		err = I2C_Read(SENSOR_I2C_HANDLE, BMP390_I2C_ADDR, BMP390_CALIB_DATA_ADDR + i, &calib[i]);
//		if (err)
//		{
//			TempPress_data.BMP390_ERR = BMP390_ERROR_READ_CALIB;
//			return false;
//		}
//	}
//
//	// Store the raw calibration data into the _BMP390_Raw_Calib_Data_ structure
//	data->NVM.u16_NVM_T1 = ((uint16_t)calib[1] << 8) | (uint16_t)calib[0];
//	data->NVM.u16_NVM_T2 = ((uint16_t)calib[3] << 8) | (uint16_t)calib[2];
//	data->NVM.i8_NVM_T3 = calib[4];
//	data->NVM.i16_NVM_P1 = ((int16_t)calib[6] << 8) | (int16_t)calib[5];
//	data->NVM.i16_NVM_P2 = ((int16_t)calib[8] << 8) | (int16_t)calib[7];
//	data->NVM.i8_NVM_P3 = calib[9];
//	data->NVM.i8_NVM_P4 = calib[10];
//	data->NVM.u16_NVM_P5 = ((uint16_t)calib[12] << 8) | (uint16_t)calib[11];
//	data->NVM.u16_NVM_P6 = ((uint16_t)calib[14] << 8) | (uint16_t)calib[13];
//	data->NVM.i8_NVM_P7 = calib[15];
//	data->NVM.i8_NVM_P8 = calib[16];
//	data->NVM.i16_NVM_P9 = ((int16_t)calib[18] << 8) | (int16_t)calib[17];
//	data->NVM.i8_NVM_P10 = calib[19];
//	data->NVM.i8_NVM_P11 = calib[20];
//	return true;
//}
//void BMP390_convert_calibration(BMP390_Data *data)
//{
//	data->PAR.f_PAR_T1 = data->NVM.u16_NVM_T1 * 256.0;
//	data->PAR.f_PAR_T2 = data->NVM.u16_NVM_T2 / 1073741824.0;
//	data->PAR.f_PAR_T3 = data->NVM.i8_NVM_T3 / 281474976710656.0;
//	data->PAR.f_PAR_P1 = (data->NVM.i16_NVM_P1 - 16384) / 1048576.0;
//	data->PAR.f_PAR_P2 = (data->NVM.i16_NVM_P2 - 16384) / 536870912.0;
//	data->PAR.f_PAR_P3 = data->NVM.i8_NVM_P3 / 4294967296.0;
//	data->PAR.f_PAR_P4 = data->NVM.i8_NVM_P4 / 137438953472.0;
//	data->PAR.f_PAR_P5 = data->NVM.u16_NVM_P5 * 8.0;
//	data->PAR.f_PAR_P6 = data->NVM.u16_NVM_P6 / 64.0;
//	data->PAR.f_PAR_P7 = data->NVM.i8_NVM_P7 / 256.0;
//	data->PAR.f_PAR_P8 = data->NVM.i8_NVM_P8 / 32768.0;
//	data->PAR.f_PAR_P9 = data->NVM.i16_NVM_P9 / 281474976710656.0;
//	data->PAR.f_PAR_P10 = data->NVM.i8_NVM_P10 / 281474976710656.0;
//	data->PAR.f_PAR_P11 = data->NVM.i8_NVM_P11 / 36893488147419103232.0;
//
//	data->PAR.f_PAR_P11 = data->NVM.i8_NVM_P11 / 36893488147419103232.0;
//	data->PAR.f_PAR_P11 = data->NVM.i8_NVM_P11 / 36893488147419103232.0;
//}
//void BMP390_set_mode(BMP390_Mode mode)
//{
//	uint8_t reg_value;
//	uint8_t err = I2C_Read(SENSOR_I2C_HANDLE, BMP390_I2C_ADDR, BMP390_REG_PWR_CTRL, &reg_value);
//	if (err)
//	{
//		TempPress_data.BMP390_ERR = BMP390_ERROR_SET_MODE;
//		return;
//	}
//	reg_value &= ~(0x30);
//	reg_value |= (mode << 4);
//	reg_value |= (3 << 0);
//	// Write modified value back to PWR_CTRL register
//	err = I2C_Write(SENSOR_I2C_HANDLE, BMP390_I2C_ADDR, BMP390_REG_PWR_CTRL, reg_value);
//	if (err)
//	{
//		TempPress_data.BMP390_ERR = BMP390_ERROR_SET_MODE;
//		return;
//	}
//}
//bool BMP390_init(void)
//{
//	LL_GPIO_SetOutputPin(SENSOR1_EN_GPIO_Port, SENSOR1_EN_Pin);
//	LL_GPIO_SetOutputPin(SENSOR2_EN_GPIO_Port, SENSOR2_EN_Pin);
//	TempPress_data.BMP390_ERR = BMP390_SUCCESS;
//	BMP390_set_mode(BMP390_MODE_NORMAL);
//	bool err = BMP390_read_raw_calibration(&TempPress_data);
//	BMP390_convert_calibration(&TempPress_data);
//	return err;
//}
//void BMP390_read_raw_temp_press(BMP390_Data *data)
//{
//	uint8_t temp_press_data[6];
//	uint8_t err = 0;
//	// Receive temperature and pressure data
//	for (uint8_t i = 0; i < 6; i++)
//	{
//		err = I2C_Read(SENSOR_I2C_HANDLE, BMP390_I2C_ADDR, BMP390_TEMP_PRESS_DATA_ADDR + i, &temp_press_data[i]);
//		if (err)
//		{
//			TempPress_data.BMP390_ERR = BMP390_ERROR_READ_TEMPRESS;
//			return;
//		}
//	}
//	// Combine the bytes to form the temperature and pressure readings
//	int32_t temp_raw = ((uint32_t)temp_press_data[5] << 16) | ((uint32_t)temp_press_data[4] << 8) | (uint32_t)temp_press_data[3];
//	int32_t press_raw = ((uint32_t)temp_press_data[2] << 16) | ((uint32_t)temp_press_data[1] << 8) | (uint32_t)temp_press_data[0];
//	// Store the raw readings into the BMP390_Data structure
//	data->temperature_raw = temp_raw;
//	data->pressure_raw = press_raw;
////	data->temperature_raw = 8646656;
////	data->pressure_raw = 6391040;
//}
//void BMP390_compensate_temperature(BMP390_Data *data)
//{
//	float partial_data1 = (float)(data->temperature_raw-data->PAR.f_PAR_T1);
//	float partial_data2 = (float)(partial_data1*data->PAR.f_PAR_T2);
//	data->temperature = partial_data2+(partial_data1*partial_data1)*data->PAR.f_PAR_T3;
//}
//void BMP390_compensate_pressure(BMP390_Data *data)
//{
//	float partial_data1;
//	float partial_data2;
//	float partial_data3;
//	float partial_data4;
//	float partial_out1;
//	float partial_out2;
//	partial_data1=data->PAR.f_PAR_P6*(data->temperature);
//	partial_data2=data->PAR.f_PAR_P7*(data->temperature*data->temperature);
//	partial_data3=data->PAR.f_PAR_P8*(data->temperature*data->temperature*data->temperature);
//	partial_out1=data->PAR.f_PAR_P5 + partial_data1 + partial_data2 + partial_data3;
//
//	partial_data1=data->PAR.f_PAR_P2*data->temperature;
//	partial_data2=data->PAR.f_PAR_P3*(data->temperature*data->temperature);
//	partial_data3=data->PAR.f_PAR_P4*(data->temperature*data->temperature*data->temperature);
//	partial_out2=(float)data->pressure_raw*(data->PAR.f_PAR_P1 + partial_data1 + partial_data2 + partial_data3);
//
//	partial_data1=(float)data->pressure_raw*(float)data->pressure_raw;
//	partial_data2=data->PAR.f_PAR_P9 + data->PAR.f_PAR_P10*data->temperature;
//	partial_data3=partial_data1*partial_data2;
//	partial_data4=((float)data->pressure_raw*(float)data->pressure_raw*(float)data->pressure_raw)*data->PAR.f_PAR_P11;
//
//	data->pressure = partial_out1 + partial_out2 + partial_data3 + partial_data4;
//}
//void bmp390_temp_press_update(void)
//{
//	BMP390_init();
//	BMP390_read_raw_temp_press(&TempPress_data);
//	BMP390_compensate_temperature(&TempPress_data);
//	BMP390_compensate_pressure(&TempPress_data);
//}
//int16_t bmp390_get_temperature(void)
//{
//	if (TempPress_data.BMP390_ERR) return 0x7FFF;
//	return (int16_t)(TempPress_data.temperature*10);
//}
//int16_t bmp390_get_press(void)
//{
//	if (TempPress_data.BMP390_ERR) return 0x7FFF;
//	return (int16_t)(TempPress_data.pressure/10.0);
//}
