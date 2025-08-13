/*
 * ads8327.c
 *
 *  Created on: Mar 2, 2025
 *      Author: DELL
 */
#include "ads8327.h"
#include "main.h"




static uint16_t CFR_user_default = 0x0EFD;		//0000 1110 1111 1101

void ADS8327_Wake_Up(ADS8327_Device_t *dev)
{
	dev->CMD = ADS8327_CMD_WAKE_UP;

    LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

    while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
    LL_SPI_TransmitData16(dev->spi, dev->CMD);
    while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
    (void)LL_SPI_ReceiveData16(dev->spi);

    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}

void ADS8327_Write_CFR(ADS8327_Device_t *dev, uint16_t CFR)
{
	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

	dev->CMD = ADS8327_CMD_WRITE_CONFIG;
	dev->CFR_value = CFR;
	uint16_t temp = (dev->CMD & 0xF000) | (dev->CFR_value & 0x0FFF);

	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
	LL_SPI_TransmitData16(dev->spi, temp);
	while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
	(void)LL_SPI_ReceiveData16(dev->spi);

	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}


void ADS8327_Default_CFR(ADS8327_Device_t *dev, CFR_default_t CFR_default)
{
	if (CFR_default == USER_DEFAULT)
	{
		ADS8327_Write_CFR(dev, CFR_user_default);
	}

	else if (CFR_default == FACTORY_DEFAULT)
	{
		LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
		dev->CMD = ADS8327_CMD_DEFAULT_MODE;
		dev->CFR_value = ADS8327_FACTORY_CFR_DEFAULT;

		while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
		LL_SPI_TransmitData16(dev->spi, dev->CMD);
		while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
		(void)LL_SPI_ReceiveData16(dev->spi);

		LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
	}
}

uint16_t ADS8327_Read_Data_Polling(ADS8327_Device_t *dev, int timeout)		// time out in tick
{
	LL_GPIO_ResetOutputPin(dev->convst_port, dev->convst_pin);

	int time = timeout;
	while(LL_GPIO_IsInputPinSet(dev->EOC_port, dev->EOC_pin) && (time--));

	time = timeout;
	while(!LL_GPIO_IsInputPinSet(dev->EOC_port, dev->EOC_pin) && (time--));

	LL_GPIO_SetOutputPin(dev->convst_port, dev->convst_pin);

	// start transfer
	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
	LL_SPI_TransmitData16(dev->spi, 0xAAAA);
	while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
	dev->ADC_val = LL_SPI_ReceiveData16(dev->spi);
	// end transfer
	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
	return dev->ADC_val;
}

//void ISR_TIMTrigger_IRQHandler(ADS8327_Device_t *dev)
//{
//	WRITE_REG(PHOTO_ADC_CS_GPIO_Port->BSRR, PHOTO_ADC_CS_Pin);
//	WRITE_REG(PHOTO_ADC_CONV_GPIO_Port->BSRR, PHOTO_ADC_CONV_Pin << 16);
//	WRITE_REG(PHOTO_ADC_CONV_GPIO_Port->BSRR, PHOTO_ADC_CONV_Pin);
//	SPI2->DR = 0xAAAA;
//	WRITE_REG(PHOTO_ADC_CS_GPIO_Port->BSRR, PHOTO_ADC_CS_Pin << 16);
//	WRITE_REG(TIM1->SR, ~(TIM_SR_UIF));
//
//}
//
//void ISR_SPI_DMA_Rx_IRQHandler(ADS8327_Device_t *dev)
//{
//	WRITE_REG(DMA1->LIFCR , DMA_LIFCR_CTCIF3);		// Clear TC flag DMA Transfer 	***Note: Modify num of flag to clear
//	CLEAR_BIT(TIM1->CR1, TIM_CR1_CEN);				// Timer disable counter
//	CLEAR_BIT(TIM1->DIER, TIM_DIER_UIE);			// Timer disable IT Update
//}


void ADS8327_Device_Init(ADS8327_Device_t *dev)
{
	dev->ADC_val = 0xFFFF;
	dev->tran_ind = 0;

	while (!LL_SPI_IsEnabled(dev->spi))
	{
		LL_SPI_Enable(dev->spi);
		__NOP();
	}

	ADS8327_Default_CFR(dev, USER_DEFAULT);
	ADS8327_Wake_Up(dev);
	// Hàm khởi tạo DMA

}






