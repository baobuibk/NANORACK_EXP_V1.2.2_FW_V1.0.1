/*
 * adg1414_chain8.c
 *
 *  Created on: Feb 28, 2025
 *      Author: Admin
 */

#include "adg1414.h"
#include "stm32f7xx.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_spi.h"
static void ADG1414_Chain_Write(ADG1414_Device_t *dev)
{
	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);

	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

    for (int i = dev->num_of_sw - 1; i >= 0; i--)
    {
        while (!LL_SPI_IsActiveFlag_TXE(dev->spi));  // Đợi TXE
        LL_SPI_TransmitData8(dev->spi, dev->switch_state[i]);

        while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));   // Đợi RXNE
        (void)LL_SPI_ReceiveData8(dev->spi);
    }

    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}

/* Hàm khởi tạo module ADG1414 */
void ADG1414_Chain_Init(ADG1414_Device_t *dev, SPI_TypeDef *spi, GPIO_TypeDef *cs_port, uint32_t cs_pin, uint8_t num_of_sw)
{
	dev->spi = spi;
	dev->num_of_sw = num_of_sw;
	dev->cs_port = cs_port;
	dev->cs_pin = cs_pin;

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = dev->cs_pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(dev->cs_port, &GPIO_InitStruct);

    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);

    for (int i = 0; i < dev->num_of_sw; i++)
    {
        dev->switch_state[i] = 0x00;
    }

    while (!LL_SPI_IsEnabled(dev->spi))
	{
		LL_SPI_Enable(dev->spi);
		__NOP();
	}

    ADG1414_Chain_Write(dev);
}

/* Hàm bật một switch */
void ADG1414_Chain_SwitchOn(ADG1414_Device_t *dev, uint8_t channel_num)
{
    if ((channel_num > INTERNAL_CHAIN_CHANNEL_NUM)&&
    	(dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM))
    	return;  // Kiểm tra giới hạn

//    if ((channel_num > EXTERNAL_CHAIN_CHANNEL_NUM)&&
//		(dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM))
//		return;  // Kiểm tra giới hạn

    if (dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM)
	{
    	for (int i = 0; i < dev->num_of_sw; i++)
		{
			dev->switch_state[i] = 0x00;
		}
    	if (channel_num)
    	{
    		uint8_t chip_idx = (channel_num-1) / 6;
			uint8_t bit_idx = (channel_num-1) % 6;
			dev->switch_state[(uint8_t)chip_idx] = (1 << bit_idx)&0x3F;
    	}
	}

    else if (dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM)
	{
    	if(channel_num)
    	{
//    		dev->switch_state[0] = (1 << (channel_num - 1));
    		dev->switch_state[0] = channel_num;
    	}

    	else
    	{
    		dev->switch_state[0] = 0;
		}
	}

    ADG1414_Chain_Write(dev);
}

/* Hàm tắt một switch */
void ADG1414_Chain_SwitchOff(ADG1414_Device_t *dev, uint8_t channel_num)
{
	if (!channel_num) return;

	if ((channel_num >= INTERNAL_CHAIN_CHANNEL_NUM) &&
		(dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM))
		return;  // Kiểm tra giới hạn

	if ((channel_num >= EXTERNAL_CHAIN_CHANNEL_NUM) &&
		(dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM))
		return;  // Kiểm tra giới hạn

	if (dev->num_of_sw == INTERNAL_CHAIN_SWITCH_NUM)
	{
		uint8_t chip_idx = (channel_num - 1) / 6;
		uint8_t bit_idx = (channel_num - 1) % 6;
		dev->switch_state[(uint8_t)chip_idx] &= ~(1 << bit_idx);
	}

	else if (dev->num_of_sw == EXTERNAL_CHAIN_SWITCH_NUM)
	{
		dev->switch_state[0] &= ~(1 << channel_num);
	}

	ADG1414_Chain_Write(dev);
}


/* Hàm tắt tất cả các switch */
void ADG1414_Chain_SwitchAllOn(ADG1414_Device_t *dev)
{
    for (int i = 0; i < dev->num_of_sw; i++)
    {
        dev->switch_state[i] = 0xFF;
    }
    ADG1414_Chain_Write(dev);
}

/* Hàm tắt tất cả các switch */
void ADG1414_Chain_SwitchAllOff(ADG1414_Device_t *dev)
{
    for (int i = 0; i < dev->num_of_sw; i++)
    {
        dev->switch_state[i] = 0x00;
    }
    ADG1414_Chain_Write(dev);
}
