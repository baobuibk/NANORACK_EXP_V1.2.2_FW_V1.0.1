/*
 * k33.c
 *
 *  Created on: May 7, 2025
 *      Author: HTSANG
 */


#include "k33.h"
#include "stdio.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_i2c.h"


static K33_RawData_t K33_RawData;

uint8_t CheckSum(uint8_t * buf, uint8_t count) {
	uint8_t sum=0;
	while (count>0) {
		sum += *buf;
		buf++;
		count--;
	}
	return sum;
}

I2C_Status_t K33_I2C_Read_RAM(uint8_t RAM_Addr, uint8_t *pData, uint8_t NumOfBytes) {
	uint8_t NumOfByteWrite = 3;
	uint8_t buf[NumOfByteWrite];
	buf[0] = (uint8_t)(K33_READ_RAM_CMD | (NumOfBytes & 0x0F));
	buf[1] = 0x00;
	buf[2] = RAM_Addr;

	uint32_t timeout = I2C_TIMEOUT;
	while (LL_I2C_IsActiveFlag_BUSY(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_GenerateStartCondition(K33_I2C_HANDLE);
	timeout = I2C_TIMEOUT;
//	while (!LL_I2C_IsActiveFlag_SB(K33_I2C_HANDLE) && timeout--);
//	if (timeout == 0) return I2C_Error;

	LL_I2C_TransmitData8(K33_I2C_HANDLE, K33_I2C_ADDR << 1);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_ADDR(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;
	LL_I2C_ClearFlag_ADDR(K33_I2C_HANDLE);

	for (uint8_t i = 0; i < NumOfByteWrite; i++) {
		LL_I2C_TransmitData8(K33_I2C_HANDLE, buf[i]);
		timeout = I2C_TIMEOUT;
		while (!LL_I2C_IsActiveFlag_TXE(K33_I2C_HANDLE) && timeout--);
		if (timeout == 0) return I2C_Error;
	}

	uint8_t CSum_Byte = CheckSum(buf, NumOfByteWrite);
	LL_I2C_TransmitData8(K33_I2C_HANDLE, CSum_Byte);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_TXE(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_GenerateStopCondition(K33_I2C_HANDLE);

	LL_I2C_GenerateStartCondition(K33_I2C_HANDLE);
	timeout = I2C_TIMEOUT;
//	while (!LL_I2C_IsActiveFlag_SB(K33_I2C_HANDLE) && timeout--);
//	if (timeout == 0) return I2C_Error;

	LL_I2C_TransmitData8(K33_I2C_HANDLE, (K33_I2C_ADDR << 1) | 1);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_ADDR(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;
	LL_I2C_ClearFlag_ADDR(K33_I2C_HANDLE);

	LL_I2C_AcknowledgeNextData(K33_I2C_HANDLE, LL_I2C_ACK);

	for (uint16_t i = 0; i < (NumOfBytes + 2); i++) {
		timeout = I2C_TIMEOUT;
		while (!LL_I2C_IsActiveFlag_RXNE(K33_I2C_HANDLE) && timeout--);
		if (timeout == 0) return I2C_Error;

		if (i == (NumOfBytes)) {
			LL_I2C_AcknowledgeNextData(K33_I2C_HANDLE, LL_I2C_NACK);
		}
		if (i == (NumOfBytes + 1)) {
			LL_I2C_GenerateStopCondition(K33_I2C_HANDLE);
		}
		pData[i] = LL_I2C_ReceiveData8(K33_I2C_HANDLE);
	}

	char buffer[80];
	snprintf(buffer, sizeof(buffer), "Start  0x%X   0x%X   0x%X   0x%X   0x%X  Stop Start  0x%X \r\n", K33_I2C_ADDR << 1, buf[0], buf[1], buf[2], CSum_Byte, (K33_I2C_ADDR << 1) | 1);
	UART_Driver_SendString(EXP_UART_CONSOLE_HANDLE, buffer);

	return I2C_Success;
}

I2C_Status_t K33_BGrCalib(void) {
	uint8_t NumOfByteWrite = 5;
	uint8_t buf[NumOfByteWrite];
	buf[0] = (uint8_t)(K33_WRITE_RAM_CMD | (0x02 & 0x0F));
	buf[1] = 0x00;
	buf[2] = K33_RAM_CALIB_ADDR;
	buf[3] = 0x7C;
	buf[4] = 0x06;

	uint32_t timeout = I2C_TIMEOUT;
	while (LL_I2C_IsActiveFlag_BUSY(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_GenerateStartCondition(K33_I2C_HANDLE);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_SB(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_TransmitData8(K33_I2C_HANDLE, K33_I2C_ADDR << 1);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_ADDR(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;
	LL_I2C_ClearFlag_ADDR(K33_I2C_HANDLE);

	for (uint8_t i = 0; i < NumOfByteWrite; i++) {
		LL_I2C_TransmitData8(K33_I2C_HANDLE, buf[i]);
		timeout = I2C_TIMEOUT;
		while (!LL_I2C_IsActiveFlag_TXE(K33_I2C_HANDLE) && timeout--);
		if (timeout == 0) return I2C_Error;
	}

	uint8_t CSum_Byte = CheckSum(buf, NumOfByteWrite);
	LL_I2C_TransmitData8(K33_I2C_HANDLE, CSum_Byte);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_TXE(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_GenerateStopCondition(K33_I2C_HANDLE);

	LL_I2C_GenerateStartCondition(K33_I2C_HANDLE);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_SB(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_TransmitData8(K33_I2C_HANDLE, (K33_I2C_ADDR << 1) | 1);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_ADDR(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;
	LL_I2C_ClearFlag_ADDR(K33_I2C_HANDLE);

	LL_I2C_AcknowledgeNextData(K33_I2C_HANDLE, LL_I2C_NACK);
	timeout = I2C_TIMEOUT;
	while(!LL_I2C_IsActiveFlag_RXNE(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	uint8_t RxData = LL_I2C_ReceiveData8(K33_I2C_HANDLE);
	LL_I2C_GenerateStopCondition(K33_I2C_HANDLE);

	if ((RxData & 0xF0) != 0x10) return I2C_Error;
	K33_RawData.Calib_status = RxData & 0x01;
	return I2C_Success;
}

I2C_Status_t K33_ZeroCalib(void) {
	uint8_t NumOfByteWrite = 5;
	uint8_t buf[NumOfByteWrite];
	buf[0] = (uint8_t)(K33_WRITE_RAM_CMD | (0x02 & 0x0F));
	buf[1] = 0x00;
	buf[2] = K33_RAM_CALIB_ADDR;
	buf[3] = 0x7C;
	buf[4] = 0x07;

	uint32_t timeout = I2C_TIMEOUT;
	while (LL_I2C_IsActiveFlag_BUSY(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_GenerateStartCondition(K33_I2C_HANDLE);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_SB(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_TransmitData8(K33_I2C_HANDLE, K33_I2C_ADDR << 1);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_ADDR(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;
	LL_I2C_ClearFlag_ADDR(K33_I2C_HANDLE);

	for (uint8_t i = 0; i < NumOfByteWrite; i++) {
		LL_I2C_TransmitData8(K33_I2C_HANDLE, buf[i]);
		timeout = I2C_TIMEOUT;
		while (!LL_I2C_IsActiveFlag_TXE(K33_I2C_HANDLE) && timeout--);
		if (timeout == 0) return I2C_Error;
	}

	uint8_t CSum_Byte = CheckSum(buf, NumOfByteWrite);
	LL_I2C_TransmitData8(K33_I2C_HANDLE, CSum_Byte);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_TXE(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_GenerateStopCondition(K33_I2C_HANDLE);

	LL_I2C_GenerateStartCondition(K33_I2C_HANDLE);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_SB(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	LL_I2C_TransmitData8(K33_I2C_HANDLE, (K33_I2C_ADDR << 1) | 1);
	timeout = I2C_TIMEOUT;
	while (!LL_I2C_IsActiveFlag_ADDR(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;
	LL_I2C_ClearFlag_ADDR(K33_I2C_HANDLE);

	LL_I2C_AcknowledgeNextData(K33_I2C_HANDLE, LL_I2C_NACK);
	timeout = I2C_TIMEOUT;
	while(!LL_I2C_IsActiveFlag_RXNE(K33_I2C_HANDLE) && timeout--);
	if (timeout == 0) return I2C_Error;

	uint8_t RxData = LL_I2C_ReceiveData8(K33_I2C_HANDLE);
	LL_I2C_GenerateStopCondition(K33_I2C_HANDLE);

	if ((RxData & 0xF0) != 0x10) return I2C_Error;
	K33_RawData.Calib_status = RxData & 0x01;
	return I2C_Success;
}

I2C_Status_t K33_Read_Data(K33_Data_t *K33_Data) {
	K33_RawData.status = K33_I2C_Read_RAM(K33_RAM_CO2_ADDR, K33_RawData.RxData, 2);
	if (K33_RawData.status == I2C_Success)
		K33_Data->CO2 = (K33_RawData.RxData[1] << 8) | K33_RawData.RxData[2];
	else return K33_RawData.status;

	char buffer[80];
	snprintf(buffer, sizeof(buffer), "  0x%X   0x%X   0x%X   0x%X \r\n", K33_RawData.RxData[0], K33_RawData.RxData[1], K33_RawData.RxData[2], K33_RawData.RxData[3]);

	UART_Driver_SendString(EXP_UART_CONSOLE_HANDLE, buffer);
	LL_mDelay(10);

	K33_RawData.status = K33_I2C_Read_RAM(K33_RAM_TEMP_ADDR, K33_RawData.RxData, 2);
	if (K33_RawData.status == I2C_Success)
		K33_Data->Temp = (K33_RawData.RxData[1] << 8) | K33_RawData.RxData[2];
	else return K33_RawData.status;

	snprintf(buffer, sizeof(buffer), "  0x%X   0x%X   0x%X   0x%X \r\n", K33_RawData.RxData[0], K33_RawData.RxData[1], K33_RawData.RxData[2], K33_RawData.RxData[3]);
	UART_Driver_SendString(EXP_UART_CONSOLE_HANDLE, buffer);

	LL_mDelay(10);

	K33_RawData.status = K33_I2C_Read_RAM(K33_RAM_RH_ADDR, K33_RawData.RxData, 2);
	if (K33_RawData.status == I2C_Success)
		K33_Data->RH = (K33_RawData.RxData[1] << 8) | K33_RawData.RxData[2];

	snprintf(buffer, sizeof(buffer), "  0x%X   0x%X   0x%X   0x%X \r\n", K33_RawData.RxData[0], K33_RawData.RxData[1], K33_RawData.RxData[2], K33_RawData.RxData[3]);
	UART_Driver_SendString(EXP_UART_CONSOLE_HANDLE, buffer);

	return K33_RawData.status;
}
