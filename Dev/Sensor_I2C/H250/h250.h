/*
 * h250.h
 *
 *  Created on: Apr 23, 2025
 *      Author: HTSANG
 */

#ifndef SENSOR_I2C_H250_H250_H_
#define SENSOR_I2C_H250_H250_H_

#include "board.h"
#include "stdint.h"
#include "i2c.h"


#define H250_I2C
//#define H250_UART

#ifdef H250_I2C
	#define H250_I2C_ADDR				0x31
	#define H250_REG					0x52

	typedef struct H250_I2C_RawData {
		uint8_t RxData[7];
		I2C_Status_t status;
	} H250_I2C_RawData_t;

	typedef uint16_t H250_I2C_Data_t;

	I2C_Status_t H250_I2C_Init(void);
	I2C_Status_t H250_I2C_Read_Data(H250_I2C_Data_t *H250_Data);
#endif

#ifdef H250_UART
	typedef struct H250_UART_RawData {
		uint8_t RxData[12];
		uint8_t unit;		// 0: is percent, 1: is ppm
	} H250_UART_RawData_t;

	typedef struct H250_UART_Data {
		char buffer[12];
		uint16_t Data;
	} H250_UART_Data_t;

	void H250_UART_Init(USART_TypeDef *handle_uart);
	void H250_UART_Read_Data(H250_UART_Data_t *H250_UART_Data);
	void USART2_IRQ(void)
#endif

#endif /* SENSOR_I2C_H250_H250_H_ */
