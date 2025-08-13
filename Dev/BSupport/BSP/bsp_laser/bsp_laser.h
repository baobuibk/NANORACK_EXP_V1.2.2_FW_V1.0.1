/*
 * bsp_laser.h
 *
 *  Created on: Jun 25, 2025
 *      Author: Admin
 */

#ifndef BSUPPORT_BSP_BSP_LASER_BSP_LASER_H_
#define BSUPPORT_BSP_BSP_LASER_BSP_LASER_H_

#include "board.h"
#include "adg1414.h"
#include "mcp4902.h"

#define ADC_MAX 	4095.0f
#define ADC_VREF 	3300.0f			//mV
#define ADC_RES_SHUNT	250			//Ohm

typedef enum{LD_CUR_INT = 0, LD_CUR_EXT}laser_channel_t;

void bsp_laser_set_spi_prescaler(uint32_t Prescaler);
void bsp_laser_set_spi_mode(spi_mode_t spi_mode);

void bsp_laser_int_set_current(uint32_t percent);
void bsp_laser_ext_set_current(uint32_t percent);

void bsp_laser_init(void);
void bsp_laser_set_current(uint32_t id, uint32_t percent);
void bsp_laser_int_switch_on(uint32_t channel_idx);
void bsp_laser_int_switch_off_all(void);
void bsp_laser_ext_switch_on(uint32_t channel_idx);
void bsp_laser_ext_switch_on_mask(uint32_t mask);
void bsp_laser_ext_switch_off_all(void);

uint16_t bsp_laser_get_ext_current(void);
uint16_t bsp_laser_get_int_current(void);

void bsp_laser_adc_init(void);
void bsp_laser_trigger_adc(void);

void bsp_laser_collect_current_data_to_buffer(void);
uint32_t bsp_laser_send_current_to_spi(void);

void bsp_laser_dma_adc_current_irq(void);

void haha(void);

#endif /* BSUPPORT_BSP_BSP_LASER_BSP_LASER_H_ */
