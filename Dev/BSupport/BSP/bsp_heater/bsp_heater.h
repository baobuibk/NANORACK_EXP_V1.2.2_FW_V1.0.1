/*
 * bsp_heater.h
 *
 *  Created on: Jun 23, 2025
 *      Author: Admin
 */

#ifndef BSUPPORT_BSP_BSP_HEATER_BSP_HEATER_H_
#define BSUPPORT_BSP_BSP_HEATER_BSP_HEATER_H_

#include "board.h"

void bsp_heater_init(void);	//init timer
void bsp_heater_set_duty_channel(uint32_t channel, uint16_t duty_pwm);
void bsp_heater_turn_off_channel(uint32_t channel);
#endif /* BSUPPORT_BSP_BSP_HEATER_BSP_HEATER_H_ */
