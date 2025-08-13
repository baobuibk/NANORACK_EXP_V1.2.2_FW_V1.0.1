/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_adc.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_i2c.h"
#include "stm32f7xx_ll_iwdg.h"
#include "stm32f7xx_ll_spi.h"
#include "stm32f7xx_ll_tim.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_exti.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_pwr.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SPI1_ReInit(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TEC_4_SWEN_Pin LL_GPIO_PIN_2
#define TEC_4_SWEN_GPIO_Port GPIOE
#define TEC_3_SWEN_Pin LL_GPIO_PIN_3
#define TEC_3_SWEN_GPIO_Port GPIOE
#define TEC_2_SWEN_Pin LL_GPIO_PIN_4
#define TEC_2_SWEN_GPIO_Port GPIOE
#define TEC_1_SWEN_Pin LL_GPIO_PIN_5
#define TEC_1_SWEN_GPIO_Port GPIOE
#define TEC_1_EN_Pin LL_GPIO_PIN_6
#define TEC_1_EN_GPIO_Port GPIOE
#define TEC_2_EN_Pin LL_GPIO_PIN_13
#define TEC_2_EN_GPIO_Port GPIOC
#define TEC_3_EN_Pin LL_GPIO_PIN_14
#define TEC_3_EN_GPIO_Port GPIOC
#define TEC_4_EN_Pin LL_GPIO_PIN_15
#define TEC_4_EN_GPIO_Port GPIOC
#define ADC_TEMP1_Pin LL_GPIO_PIN_0
#define ADC_TEMP1_GPIO_Port GPIOC
#define ADC_TEMP2_Pin LL_GPIO_PIN_1
#define ADC_TEMP2_GPIO_Port GPIOC
#define ADC_TEMP3_Pin LL_GPIO_PIN_2
#define ADC_TEMP3_GPIO_Port GPIOC
#define ADC_TEMP4_Pin LL_GPIO_PIN_3
#define ADC_TEMP4_GPIO_Port GPIOC
#define ADC_TEC_OUT_Pin LL_GPIO_PIN_1
#define ADC_TEC_OUT_GPIO_Port GPIOA
#define ADC_LASER_EXT_Pin LL_GPIO_PIN_2
#define ADC_LASER_EXT_GPIO_Port GPIOA
#define ADC_LASER_INT_Pin LL_GPIO_PIN_3
#define ADC_LASER_INT_GPIO_Port GPIOA
#define EXP_SPI_NSS_Pin LL_GPIO_PIN_4
#define EXP_SPI_NSS_GPIO_Port GPIOA
#define EXP_SPI_SCK_Pin LL_GPIO_PIN_5
#define EXP_SPI_SCK_GPIO_Port GPIOA
#define EXP_SPI_MISO_Pin LL_GPIO_PIN_6
#define EXP_SPI_MISO_GPIO_Port GPIOA
#define EXP_SPI_MOSI_Pin LL_GPIO_PIN_7
#define EXP_SPI_MOSI_GPIO_Port GPIOA
#define ADC_TEMP5_Pin LL_GPIO_PIN_4
#define ADC_TEMP5_GPIO_Port GPIOC
#define ADC_TEMP6_Pin LL_GPIO_PIN_5
#define ADC_TEMP6_GPIO_Port GPIOC
#define ADC_TEMP7_Pin LL_GPIO_PIN_0
#define ADC_TEMP7_GPIO_Port GPIOB
#define ADC_TEMP8_Pin LL_GPIO_PIN_1
#define ADC_TEMP8_GPIO_Port GPIOB
#define LASER_DAC_CS_Pin LL_GPIO_PIN_2
#define LASER_DAC_CS_GPIO_Port GPIOB
#define EXP_UART_RX_Pin LL_GPIO_PIN_7
#define EXP_UART_RX_GPIO_Port GPIOE
#define EXP_UART_TX_Pin LL_GPIO_PIN_8
#define EXP_UART_TX_GPIO_Port GPIOE
#define LASER_INT_SW_CS_Pin LL_GPIO_PIN_10
#define LASER_INT_SW_CS_GPIO_Port GPIOE
#define PHOTO_ADC_CS_Pin LL_GPIO_PIN_11
#define PHOTO_ADC_CS_GPIO_Port GPIOE
#define PHOTO_SCK_Pin LL_GPIO_PIN_12
#define PHOTO_SCK_GPIO_Port GPIOE
#define PHOTO_MISO_Pin LL_GPIO_PIN_13
#define PHOTO_MISO_GPIO_Port GPIOE
#define PHOTO_MOSI_Pin LL_GPIO_PIN_14
#define PHOTO_MOSI_GPIO_Port GPIOE
#define LASER_EXT_SW_CS_Pin LL_GPIO_PIN_15
#define LASER_EXT_SW_CS_GPIO_Port GPIOE
#define Log_OverFlow_Pin LL_GPIO_PIN_10
#define Log_OverFlow_GPIO_Port GPIOB
#define MIN_HandShake_Pin LL_GPIO_PIN_11
#define MIN_HandShake_GPIO_Port GPIOB
#define LASER_DAC_LATCH_Pin LL_GPIO_PIN_12
#define LASER_DAC_LATCH_GPIO_Port GPIOB
#define LASER_SCK_Pin LL_GPIO_PIN_13
#define LASER_SCK_GPIO_Port GPIOB
#define LASER_MISO_Pin LL_GPIO_PIN_14
#define LASER_MISO_GPIO_Port GPIOB
#define LASER_MOSI_Pin LL_GPIO_PIN_15
#define LASER_MOSI_GPIO_Port GPIOB
#define PHOTO_SW_CS_Pin LL_GPIO_PIN_8
#define PHOTO_SW_CS_GPIO_Port GPIOD
#define WD_DONE_Pin LL_GPIO_PIN_9
#define WD_DONE_GPIO_Port GPIOD
#define EF_12_AUX_Pin LL_GPIO_PIN_10
#define EF_12_AUX_GPIO_Port GPIOD
#define PHOTO_ADC_EOC_Pin LL_GPIO_PIN_11
#define PHOTO_ADC_EOC_GPIO_Port GPIOD
#define CONS_TX_Pin LL_GPIO_PIN_6
#define CONS_TX_GPIO_Port GPIOC
#define CONS_RX_Pin LL_GPIO_PIN_7
#define CONS_RX_GPIO_Port GPIOC
#define IRQ_0_Pin LL_GPIO_PIN_8
#define IRQ_0_GPIO_Port GPIOC
#define IRQ_1_Pin LL_GPIO_PIN_9
#define IRQ_1_GPIO_Port GPIOC
#define PHOTO_ADC_CONV_Pin LL_GPIO_PIN_8
#define PHOTO_ADC_CONV_GPIO_Port GPIOA
#define RS485_TX_Pin LL_GPIO_PIN_9
#define RS485_TX_GPIO_Port GPIOA
#define RS485_RX_Pin LL_GPIO_PIN_10
#define RS485_RX_GPIO_Port GPIOA
#define LED_G_Pin LL_GPIO_PIN_11
#define LED_G_GPIO_Port GPIOA
#define RS485_DE_Pin LL_GPIO_PIN_12
#define RS485_DE_GPIO_Port GPIOA
#define LED_B_Pin LL_GPIO_PIN_15
#define LED_B_GPIO_Port GPIOA
#define TEC_SCK_Pin LL_GPIO_PIN_10
#define TEC_SCK_GPIO_Port GPIOC
#define TEC_MISO_Pin LL_GPIO_PIN_11
#define TEC_MISO_GPIO_Port GPIOC
#define TEC_MOSI_Pin LL_GPIO_PIN_12
#define TEC_MOSI_GPIO_Port GPIOC
#define ADG_MOSI_Pin LL_GPIO_PIN_0
#define ADG_MOSI_GPIO_Port GPIOD
#define ADG_SCK_Pin LL_GPIO_PIN_1
#define ADG_SCK_GPIO_Port GPIOD
#define ADG_CS_Pin LL_GPIO_PIN_2
#define ADG_CS_GPIO_Port GPIOD
#define EF_5_AUX_Pin LL_GPIO_PIN_3
#define EF_5_AUX_GPIO_Port GPIOD
#define EF_5_EN_Pin LL_GPIO_PIN_4
#define EF_5_EN_GPIO_Port GPIOD
#define SENSOR1_EN_Pin LL_GPIO_PIN_5
#define SENSOR1_EN_GPIO_Port GPIOD
#define SENSOR2_EN_Pin LL_GPIO_PIN_6
#define SENSOR2_EN_GPIO_Port GPIOD
#define FRAM_CS_Pin LL_GPIO_PIN_7
#define FRAM_CS_GPIO_Port GPIOD
#define FRAM_SCK_Pin LL_GPIO_PIN_3
#define FRAM_SCK_GPIO_Port GPIOB
#define FRAM_MISO_Pin LL_GPIO_PIN_4
#define FRAM_MISO_GPIO_Port GPIOB
#define FRAM_MOSI_Pin LL_GPIO_PIN_5
#define FRAM_MOSI_GPIO_Port GPIOB
#define SENSOR_SCL_Pin LL_GPIO_PIN_6
#define SENSOR_SCL_GPIO_Port GPIOB
#define SENSOR_SDA_Pin LL_GPIO_PIN_7
#define SENSOR_SDA_GPIO_Port GPIOB
#define TEC_4_CS_Pin LL_GPIO_PIN_8
#define TEC_4_CS_GPIO_Port GPIOB
#define TEC_3_CS_Pin LL_GPIO_PIN_9
#define TEC_3_CS_GPIO_Port GPIOB
#define TEC_2_CS_Pin LL_GPIO_PIN_0
#define TEC_2_CS_GPIO_Port GPIOE
#define TEC_1_CS_Pin LL_GPIO_PIN_1
#define TEC_1_CS_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

//#define EXP_DEBUG_PRINTING
//#define MIN_SHELL_DEBUG_PRINTING
//#define PHOTO_COOL_DEBUG_PRINTING
//#define SPI_TRANS_DEBUG_PRINTING
//#define TEMPERATURE_CONTROL_DEBUG_PRINTING

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
