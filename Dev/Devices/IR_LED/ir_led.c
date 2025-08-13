/*
 * ir_led.c
 *
 *  Created on: Dec 26, 2024
 *      Author: SANG HUYNH
 */

#include "ir_led.h"

static uint16_t ir_led_duty_current = 0;

void ir_led_set_duty(uint8_t duty)
{
	ir_led_duty_current = duty*10;
	LL_TIM_OC_SetCompareCH1(TIM1, duty*10);
}

uint8_t ir_led_get_duty(void)
{
	return ir_led_duty_current/10;
}
