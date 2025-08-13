/*
 * mcp4902.h
 *
 *  Created on: Mar 2, 2025
 *      Author: DELL
 */

#ifndef MCP4902_H_
#define MCP4902_H_

#include "stm32f7xx.h"
#include "stm32f7xx_ll_gpio.h"


#define _VREF_DAC		3300	// milivolt

#define MCP4902_NUM_CHANNEL		2
#define MCP4902_BUF_BIT			14
#define MCP4902_GA_BIT			13
#define MCP4902_SHDN_BIT		12
#define MCP4902_AB_BIT			15
#define MCP4902_CHA				0
#define MCP4902_CHB				1


typedef struct {
    SPI_TypeDef *spi;              // Peripheral SPI (SPI1, SPI2, v.v.)
    GPIO_TypeDef *cs_port;         // Port của chân CS
    uint32_t cs_pin;               // Chân CS
    GPIO_TypeDef *latch_port;         // Port của chân latch
	uint32_t latch_pin;               // Chân latch
    uint8_t dac_channel[MCP4902_NUM_CHANNEL];  // DAC các kênh
} MCP4902_Device_t;


uint8_t v2dac(uint16_t voltage);
uint16_t dac2v(uint8_t dac);
void MCP4902_Shutdown(MCP4902_Device_t *dev, uint8_t channel);
void MCP4902_Set_DAC(MCP4902_Device_t *dev, uint8_t channel, uint8_t DAC_val);
void MCP4902_Set_Voltage(MCP4902_Device_t *dev, uint8_t channel, uint16_t voltage);
void MCP4902_Device_Init(MCP4902_Device_t *dev, SPI_TypeDef *spi, GPIO_TypeDef *cs_port, uint32_t cs_pin, GPIO_TypeDef *latch_port, uint32_t latch_pin);

#endif /* MCP4902_H_ */
