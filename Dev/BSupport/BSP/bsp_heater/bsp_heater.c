/*
 * bsp_heater.c
 *
 *  Created on: Jun 23, 2025
 *      Author: Admin
 */
#include "bsp_heater.h"

#define HEATER_TIMER TIM4
#define TIMER_PERIOD 50000

void bsp_heater_set_duty_channel(uint32_t channel, uint16_t duty_pwm)
{
    if (duty_pwm > 100) duty_pwm = 100;
    uint16_t compare_value = duty_pwm * TIMER_PERIOD / 100;
    switch (channel)
    {
        case 0:
            LL_TIM_OC_SetCompareCH1(HEATER_TIMER, compare_value);
            break;
        case 1:
            LL_TIM_OC_SetCompareCH2(HEATER_TIMER, compare_value);
            break;
        case 2:
            LL_TIM_OC_SetCompareCH3(HEATER_TIMER, compare_value);
            break;
        case 3:
            LL_TIM_OC_SetCompareCH4(HEATER_TIMER, compare_value);
            break;
        default:
            break;
    }
}

void bsp_heater_turn_off_channel(uint32_t channel){
    switch (channel)
    {
        case 0:
            LL_TIM_OC_SetCompareCH1(HEATER_TIMER, 0);
            break;
        case 1:
            LL_TIM_OC_SetCompareCH2(HEATER_TIMER, 0);
            break;
        case 2:
            LL_TIM_OC_SetCompareCH3(HEATER_TIMER, 0);
            break;
        case 3:
            LL_TIM_OC_SetCompareCH4(HEATER_TIMER, 0);
            break;
        default:
            break;
    }
}
