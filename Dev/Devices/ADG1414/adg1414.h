/*
 * adg1414_chain8.h
 *
 *  Created on: Feb 28, 2025
 *      Author: Admin
 */

#ifndef ADG1414_H
#define ADG1414_H

#include "stm32f7xx.h"


/* Định nghĩa số lượng chip và switch */
#define ADG1414_CHAIN_NUM_CHIPS_MAX    16   // max chip trong daisy chain
#define ADG1414_DATA_BYTES   8   			// 8 byte (mỗi chip 1 byte, chỉ dùng 6 bit thấp)

/* User define */
#define INTERNAL_CHAIN_SWITCH_NUM		6
#define EXTERNAL_CHAIN_SWITCH_NUM		1
#define INTERNAL_CHAIN_CHANNEL_NUM		36
#define EXTERNAL_CHAIN_CHANNEL_NUM		8

/* Cấu trúc instance cho ADG1414 chain8 */
typedef struct {
    SPI_TypeDef *spi;              // Peripheral SPI (SPI1, SPI2, v.v.)
    GPIO_TypeDef *cs_port;         // Port của chân CS
    uint32_t cs_pin;               // Chân CS (chung cho tất cả chip)
    uint8_t num_of_sw;			   // Số chip mode chain
    uint8_t switch_state[ADG1414_CHAIN_NUM_CHIPS_MAX];  // Trạng thái các switch
} ADG1414_Device_t;

/* Hàm khởi tạo module ADG1414 */
void ADG1414_Chain_Init(ADG1414_Device_t *dev, SPI_TypeDef *spi, GPIO_TypeDef *cs_port, uint32_t cs_pin, uint8_t num_of_sw);

/* Hàm bật một switch */
void ADG1414_Chain_SwitchOn(ADG1414_Device_t *dev, uint8_t channel_num);

/* Hàm tắt một switch */
void ADG1414_Chain_SwitchOff(ADG1414_Device_t *dev, uint8_t channel_num);

/* Hàm tắt tất cả các switch */
void ADG1414_Chain_SwitchAllOn(ADG1414_Device_t *dev);

/* Hàm tắt tất cả các switch */
void ADG1414_Chain_SwitchAllOff(ADG1414_Device_t *dev);

#endif /* __ADG1414_H */
