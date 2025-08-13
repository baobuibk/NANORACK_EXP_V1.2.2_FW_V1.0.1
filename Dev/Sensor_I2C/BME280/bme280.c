/*
 * bme280.c
 *
 *  Created on: Apr 18, 2025
 *      Author: HTSANG
 */


#include "bme280.h"

static BME280_calib_t BME280_calib;
static BME280_RawData_t BME280_RawData = {
		.status = I2C_Success,
};

void BME280_Write(uint8_t reg, uint8_t data) {
	BME280_RawData.status = I2C_Write(EXP_I2C_SENSOR_HANDLE, BME280_I2C_ADDR, reg, data);
}

void BME280_Read(uint8_t reg, uint8_t *pData, uint8_t length) {
	BME280_RawData.status = I2C_ReadMulti(EXP_I2C_SENSOR_HANDLE, BME280_I2C_ADDR, reg, pData, length);
}

I2C_Status_t BME280_Init(void)
{
	// reset device
	BME280_Write(BME280_RESET_REG, 0xB6);

	// write the humidity oversampling
	// osrs_h[2..0]= 000   001   010   011  100  101&others : oversampling is skipped 1  2  4  8  16
	BME280_Write(BME280_CTRL_HUM_REG, 0x01);

	// write the standby time and IIR filter coeff
	BME280_Write(BME280_CONFIG_REG, 0b00010000);

	// write the pressure and temp oversampling along with mode to 0xF4
	// osrs_t[2..0]= 000   001   010   011  100  101&others : oversampling is skipped 1  2  4  8  16
	// osrs_p[2..0]= 000   001   010   011  100  101&others : oversampling is skipped 1  2  4  8  16
	// mode[1..0]=	 00   01and10  11 : SleepMode  ForcedMode   NormalMode
	BME280_Write(BME280_CTRL_MEAS_REG, 0b01010111);
	uint8_t ID = 0;
	BME280_Read_ID(&ID);
	if (ID != BME280_ID) BME280_RawData.status = I2C_Error;
	return BME280_RawData.status;
}

I2C_Status_t BME280_Read_ID(uint8_t *ID) {
	BME280_Read(BME280_ID_REG, ID, 1);
	return BME280_RawData.status;
}

void BME280_Read_CalibrationData(void) {
	uint8_t calib[26];
	BME280_Read(BME280_CALIB_REG_1, calib, 26);

	BME280_calib.dig_T1 = (uint16_t)(calib[1] << 8 | calib[0]);
	BME280_calib.dig_T2 = (int16_t)(calib[3] << 8 | calib[2]);
	BME280_calib.dig_T3 = (int16_t)(calib[5] << 8 | calib[4]);

	BME280_calib.dig_P1 = (uint16_t)(calib[7] << 8 | calib[6]);
	BME280_calib.dig_P2 = (int16_t)(calib[9] << 8 | calib[8]);
	BME280_calib.dig_P3 = (int16_t)(calib[11] << 8 | calib[10]);
	BME280_calib.dig_P4 = (int16_t)(calib[13] << 8 | calib[12]);
	BME280_calib.dig_P5 = (int16_t)(calib[15] << 8 | calib[14]);
	BME280_calib.dig_P6 = (int16_t)(calib[17] << 8 | calib[16]);
	BME280_calib.dig_P7 = (int16_t)(calib[19] << 8 | calib[18]);
	BME280_calib.dig_P8 = (int16_t)(calib[21] << 8 | calib[20]);
	BME280_calib.dig_P9 = (int16_t)(calib[23] << 8 | calib[22]);

	BME280_calib.dig_H1 = calib[25];

	BME280_Read(BME280_CALIB_REG_2, calib, 7);

	BME280_calib.dig_H2 = (int16_t)(calib[1] << 8 | calib[0]);
	BME280_calib.dig_H3 = calib[2];
	BME280_calib.dig_H4 = (int16_t)((calib[3] << 4) | (calib[4] & 0x0F));
	BME280_calib.dig_H5 = (int16_t)((calib[5] << 4) | (calib[4] >> 4));
	BME280_calib.dig_H6 = (int8_t)calib[6];
}

void BME280_Read_RawData(BME280_RawData_t *BME280_RawData) {
	uint8_t RawData[8];
	BME280_Read(BME280_PRESS_REG, RawData, 8);
	BME280_RawData->press = ((uint32_t)RawData[0] << 12) | ((uint32_t)RawData[1] << 4) | (RawData[2] >> 4);
	BME280_RawData->temp  = ((uint32_t)RawData[3] << 12) | ((uint32_t)RawData[4] << 4) | (RawData[5] >> 4);
	BME280_RawData->humi  = ((uint16_t)RawData[6] << 8) | RawData[7];
}

int32_t t_fine = 0;

float BME280_Compensate_Temperature(uint32_t adc_T) {
	int32_t var1, var2;
	var1 = ((((adc_T >> 3) - ((int32_t)BME280_calib.dig_T1 << 1))) *
		   ((int32_t)BME280_calib.dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)BME280_calib.dig_T1)) *
			  ((adc_T >> 4) - ((int32_t)BME280_calib.dig_T1))) >> 12) *
			((int32_t)BME280_calib.dig_T3)) >> 14;

	t_fine = var1 + var2;
	float T = (t_fine * 5 + 128) >> 8;
	return T / 100.0f;
}

float BME280_Compensate_Pressure(uint32_t adc_P) {
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)BME280_calib.dig_P6;
	var2 = var2 + ((var1 * (int64_t)BME280_calib.dig_P5) << 17);
	var2 = var2 + (((int64_t)BME280_calib.dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)BME280_calib.dig_P3) >> 8) +
		   ((var1 * (int64_t)BME280_calib.dig_P2) << 12);
	var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)BME280_calib.dig_P1) >> 33;

	if (var1 == 0) return 0; // avoid division by zero

	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)BME280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)BME280_calib.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)BME280_calib.dig_P7) << 4);
	return (float)p / 25600.0f;  // hPa
}

float BME280_Compensate_Humidity(uint16_t adc_H) {
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)BME280_calib.dig_H4) << 20) -
					(((int32_t)BME280_calib.dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
				 (((((((v_x1_u32r * ((int32_t)BME280_calib.dig_H6)) >> 10) *
					  (((v_x1_u32r * ((int32_t)BME280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
					((int32_t)2097152)) * ((int32_t)BME280_calib.dig_H2) + 8192) >> 14));
	v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
							((int32_t)BME280_calib.dig_H1)) >> 4);
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (float)(v_x1_u32r >> 12) / 1024.0f;
}

void BME280_Convert_Raw_To_Physical(BME280_RawData_t *RawData, BME280_Data_t *PhyData) {
	PhyData->temperature = BME280_Compensate_Temperature(RawData->temp);
	PhyData->pressure = BME280_Compensate_Pressure(RawData->press);
	PhyData->humidity = BME280_Compensate_Humidity(RawData->humi);
}

I2C_Status_t BME280_Read_Data(BME280_Data_t *BME280_Data) {
	if (BME280_RawData.status == I2C_Error) BME280_Init();
	BME280_Read_CalibrationData();
	BME280_Read_RawData(&BME280_RawData);
	BME280_Convert_Raw_To_Physical(&BME280_RawData, BME280_Data);
	return BME280_RawData.status;
}

