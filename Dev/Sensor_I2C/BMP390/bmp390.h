/*
 * bmp390.h
 *
 *  Created on: Dec 16, 2024
 *      Author: SANG HUYNH
 */

#ifndef DEVICES_BMP390_BMP390_H_
#define DEVICES_BMP390_BMP390_H_

#include "main.h"
#include "i2c.h"
#include "stdbool.h"

// Calibration data structure for BMP390
typedef struct {
	uint16_t	u16_NVM_T1;
	uint16_t	u16_NVM_T2;
	int8_t		i8_NVM_T3;
	int16_t		i16_NVM_P1;
	int16_t		i16_NVM_P2;
	int8_t		i8_NVM_P3;
	int8_t		i8_NVM_P4;
	uint16_t	u16_NVM_P5;
	uint16_t	u16_NVM_P6;
	int8_t		i8_NVM_P7;
	int8_t		i8_NVM_P8;
	uint16_t	i16_NVM_P9;
	int8_t		i8_NVM_P10;
	int8_t		i8_NVM_P11;
} BMP390_Raw_Calib_Data_t;

typedef struct {
	float	f_PAR_T1;
	float	f_PAR_T2;
	float	f_PAR_T3;
	float	f_PAR_P1;
	float	f_PAR_P2;
	float	f_PAR_P3;
	float	f_PAR_P4;
	float	f_PAR_P5;
	float	f_PAR_P6;
	float	f_PAR_P7;
	float	f_PAR_P8;
	float	f_PAR_P9;
	float	f_PAR_P10;
	float	f_PAR_P11;
} BMP390_Calib_Data_t;

typedef enum {
	BMP390_MODE_SLEEP	= 0,
	BMP390_MODE_FORCED	= 1,
	BMP390_MODE_NORMAL	= 3
} BMP390_Mode;

typedef enum {
	BMP390_SUCCESS				= 0,   // No error
	BMP390_ERROR_UNKNOWN		= 1,   // Unknown error
	BMP390_ERROR_SET_MODE		= 2,   // BMP390 set mode error
	BMP390_ERROR_READ_CALIB		= 3,   // BMP390 read raw calib error
	BMP390_ERROR_READ_TEMPRESS	= 4    // BMP390 read temp, pressure error
} BMP390_ERROR;

// Data structure for the BMP390 sensor
typedef struct _BMP390_Data_
{
	int32_t 					temperature_raw;		// Temperature reading
	float 						temperature;			// Temperature reading
	int32_t 					pressure_raw;			// Pressure reading
	float 						pressure;				// Temperature reading
	uint8_t 					chipID;
	BMP390_Raw_Calib_Data_t		NVM;					// RAW Calibration data
	BMP390_Calib_Data_t			PAR;
	BMP390_ERROR				BMP390_ERR;				// Error status
}BMP390_Data;

extern BMP390_Data TempPress_data;

bool BMP390_read_raw_calibration(BMP390_Data *data);
void BMP390_convert_calibration(BMP390_Data *data);
void BMP390_set_mode(BMP390_Mode mode);
bool BMP390_init(void);
void BMP390_read_raw_temp_press(BMP390_Data *data);
void BMP390_compensate_temperature(BMP390_Data *data);
void BMP390_compensate_pressure(BMP390_Data *data);
void bmp390_temp_press_update(void);
int16_t bmp390_get_temperature(void);
int16_t bmp390_get_press(void);

#endif /* DEVICES_BMP390_BMP390_H_ */
