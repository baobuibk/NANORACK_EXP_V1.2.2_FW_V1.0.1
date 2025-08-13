/*
 * bsp_tec.h
 *
 *  Created on: Jun 22, 2025
 *      Author: Admin
 */

#ifndef BSUPPORT_BSP_BSP_TEC_BSP_TEC_H_
#define BSUPPORT_BSP_BSP_TEC_BSP_TEC_H_

#include "board.h"

extern struct lt8722_dev tec_0;
extern struct lt8722_dev tec_1;
extern struct lt8722_dev tec_2;
extern struct lt8722_dev tec_3;

uint32_t bsp_temperature_power_on(void);

uint32_t bsp_temperature_power_off(void);
//
//uint32_t bsp_tec_enable(uint32_t tec_idx);
//uint32_t bsp_tec_disable(uint32_t tec_idx);
//uint32_t bsp_temperature_power_on(void);
//uint32_t bsp_temperature_power_off(void);
//
//uint32_t bsp_tec_init(uint32_t tec_idx);
//
//uint32_t bsp_tec_output_voltage(uint32_t tec_idx, uint32_t dir, uint16_t voltage);
#endif /* BSUPPORT_BSP_BSP_TEC_BSP_TEC_H_ */
