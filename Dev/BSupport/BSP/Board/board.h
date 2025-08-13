/*
 * board.h
 *
 *  Created on: Nov 21, 2024
 *      Author: SANG HUYNH
 */

#ifndef BOARD_BOARD_H_
#define BOARD_BOARD_H_
#include "stm32f7xx.h"
#include "main.h"
#include "adg1414.h"
#include "ads8327.h"
#include "mcp4902.h"
#include "lt8722.h"
//#define STM32F7_DISCO

#define EXP_BOARD

#if defined(EXP_BOARD)
#define CLI_UART						USART6
#define LWL_UART						USART3
#elif defined(STM32F7_DISCO)
#define CLI_UART						USART1
#define LWL_UART						USART1
#endif

#define BUFFER_HALF_SIZE				(8 * 1024)
#define BUFFER_HALF_SIZE_BYTE 			(2 * BUFFER_HALF_SIZE)
//this is the block size of data aquisition in 16 bit
#define BUFFER_FULL_SIZE				(2 * BUFFER_HALF_SIZE)

typedef enum{SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3} spi_mode_t;



#define MIN_SHELL_UART	UART7
//*****************************************************************************
// EXP_UART_RS485
//*****************************************************************************
#define EXP_UART_RS485_HANDLE			USART1
#define EXP_UART_RS495_IRQ				USART1_IRQn


//*****************************************************************************
// EXP_UART_CONSOLE
//*****************************************************************************
#define EXP_UART_CONSOLE_HANDLE			USART6
#define EXP_UART_CONSOLE_IRQ			USART6_IRQn

//*****************************************************************************
// OBC_EXP_UART
//*****************************************************************************
//#define EXP_UART_OBC_HANDLE				UART7
//#define EXP_UART_OBC_IRQ				UART7_IRQn


//*****************************************************************************
// I2C_SENSOR
//*****************************************************************************
#define EXP_I2C_SENSOR_HANDLE			I2C1
#define EXP_I2C_SENSOR_IRQ				I2C1_EV_IRQn


//*****************************************************************************
// I2C_COPC
//*****************************************************************************
#define EXP_I2C_OBC_HANDLE				I2C2
#define EXP_I2C_OBC_IRQ					I2C2_EV_IRQn


//*****************************************************************************
// SPI_TEC
//*****************************************************************************
#define SPI_TEC				SPI3

extern struct lt8722_dev tec_0;
extern struct lt8722_dev tec_1;
extern struct lt8722_dev tec_2;
extern struct lt8722_dev tec_3;
extern struct lt8722_dev * tec_table[];
extern struct mb85rs2mt_dev fram;



#endif /* BOARD_BOARD_H_ */
