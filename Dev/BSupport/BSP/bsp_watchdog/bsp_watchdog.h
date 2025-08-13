/*
 * bsp_watchdog.h
 *
 *  Created on: Jul 23, 2025
 *      Author: Admin
 */

#ifndef BSUPPORT_BSP_BSP_WATCHDOG_BSP_WATCHDOG_H_
#define BSUPPORT_BSP_BSP_WATCHDOG_BSP_WATCHDOG_H_

#include "stm32f7xx.h"
#include "stm32f7xx_ll_gpio.h"
#include "configs.h"
#include "main.h"

//#define WGD_TPL5010_PORT GPIOA
//#define WGD_TPL5010_PIN  (1U << 0) // PA0

#define WGD_TPL5010_PORT		WD_DONE_GPIO_Port
#define WGD_TPL5010_PIN			WD_DONE_Pin

#define WGD_TPL5010_ON()		(WGD_TPL5010_PORT->BSRR = WGD_TPL5010_PIN)
#define WGD_TPL5010_OFF()		(WGD_TPL5010_PORT->BSRR = (WGD_TPL5010_PIN << 16))
#define WGD_TPL5010_PULSE_100NS() 	do { WGD_TPL5010_ON(); __NOP(); __NOP(); __NOP(); __NOP(); WGD_TPL5010_OFF(); } while(0)

void bsp_tpl5010_feed();
void bsp_enable_backup_sram(void);

#endif /* BSUPPORT_BSP_BSP_WATCHDOG_BSP_WATCHDOG_H_ */
