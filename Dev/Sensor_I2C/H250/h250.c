/*
 * h250.c
 *
 *  Created on: Apr 23, 2025
 *      Author: HTSANG
 */


#include "h250.h"
#include <math.h>
#include <stdio.h>

#ifdef H250_I2C

static H250_I2C_RawData_t H250_I2C_RawData = {
		.status = I2C_Success,
};

void H250_I2C_Read(uint8_t reg, uint8_t *pData, uint8_t length) {
	H250_I2C_RawData.status = I2C_ReadMulti(EXP_I2C_SENSOR_HANDLE, H250_I2C_ADDR, reg, pData, length);
}

I2C_Status_t H250_I2C_Init(void) {
	H250_I2C_Read(H250_REG, &H250_I2C_RawData.RxData[0], 7);
	if (H250_I2C_RawData.RxData[0] == 0x08)
		return H250_I2C_RawData.status;
	else
		return I2C_Error;
}

I2C_Status_t H250_I2C_Read_Data(H250_I2C_Data_t *H250_I2C_Data)
{
	H250_I2C_Read(H250_REG, &H250_I2C_RawData.RxData[0], 7);
	*H250_I2C_Data = (H250_I2C_RawData.RxData[1] << 8) | H250_I2C_RawData.RxData[2];
	return H250_I2C_RawData.status;
}
#endif


#ifdef H250_UART

USART_TypeDef *UART_H250;

H250_UART_RawData_t H250_UART_RawData;

void H250_UART_Init(USART_TypeDef *handle_uart) {
	UART_H250 = handle_uart;
}

void H250_UART_Read_Data(H250_UART_Data_t *H250_UART_Data) {
	H250_UART_Data->Data = 0;
	if (H250_UART_RawData.unit == 0) {
		for (uint8_t i = 0; i < 9; i++) {
			H250_UART_Data->buffer[i] = H250_UART_RawData.RxData[i];
		}
		if (H250_UART_RawData.RxData[0] != 0x20)
			H250_UART_Data->Data = (H250_UART_RawData.RxData[0] - 0x30)*1000;
		H250_UART_Data->Data += (H250_UART_RawData.RxData[1] - 0x30)*100 + (H250_UART_RawData.RxData[3] - 0x30)*10 + (H250_UART_RawData.RxData[4] - 0x30);
	}
	if (H250_UART_RawData.unit == 1) {
		for (uint8_t i = 0; i < 12; i++) {
			H250_UART_Data->buffer[i] = H250_UART_RawData.RxData[i];
		}
		for (uint8_t i = 0; i < 6; ++i) {
			if (H250_UART_RawData.RxData[i] != 0x20)
				H250_UART_Data->Data += pow(H250_UART_RawData.RxData[i]-0x30, 5-i);
		}
	}
}

volatile uint8_t index = 0;

void USART2_IRQ(void)
{
    if (LL_USART_IsActiveFlag_RXNE(UART_H250)) {
        uint8_t received = LL_USART_ReceiveData8(UART_H250);
		H250_UART_RawData.RxData[index++] = received;
		if (received == 0x0A && H250_UART_RawData.RxData[index - 2] == 0x0D) {
			if (H250_UART_RawData.RxData[index - 4] == ' ' && H250_UART_RawData.RxData[index - 3] == 0x25) {
				H250_UART_RawData.unit = 0;
			}
			if (H250_UART_RawData.RxData[index - 5] == 'p' && H250_UART_RawData.RxData[index - 4] == 'p' && H250_UART_RawData.RxData[index - 3] == 'm') {
				H250_UART_RawData.unit = 1;
			}
			index = 0;
		}
    }
}

#endif
