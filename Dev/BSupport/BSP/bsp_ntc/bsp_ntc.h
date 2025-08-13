/*
 * bsp_ntc.h
 *
 *  Created on: Jun 22, 2025
 *      Author: Admin
 */

#ifndef BSUPPORT_BSP_NTC_BSP_NTC_H_
#define BSUPPORT_BSP_NTC_BSP_NTC_H_

#include "board.h"
void bsp_ntc_trigger_adc(void);
void bsp_ntc_adc_init(void);
int16_t bsp_ntc_get_temperature(uint32_t ntc_channel);
uint16_t temperature_monitor_get_ntc_error(uint32_t channel1, uint32_t channel2, int16_t max_temp, int16_t min_temp);

void bsp_ntc_dma_adc_irq(void);

#endif /* BSUPPORT_BSP_NTC_BSP_NTC_H_ */
