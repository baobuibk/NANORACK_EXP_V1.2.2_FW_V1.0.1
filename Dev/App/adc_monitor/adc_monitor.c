/*
 * temperature_monitor.c
 *
 *  Created on: Jun 21, 2025
 *      Author: Admin
 */

#include "adc_monitor.h"
#include "bsp_ntc.h"
#include "bsp_laser.h"
#include "dbc_assert.h"
#include "app_signals.h"
#include "error_codes.h"
#include "uart_dbg.h"
#include "stddef.h"
#include "stdlib.h"
#include "ntc.h"
#include "wdg.h"
#include "configs.h"

DBC_MODULE_NAME("adc_monitor")

#define MONITOR_TASK_INTERVAL			10		//mS
#define MONITOR_TASK_NUM_EVENTS			4

monitor_task_t monitor_task_inst;
circular_buffer_t monitor_e_queue = {0}; // Circular buffer to hold shell events
static monitor_evt_t current_monitor_e = {0}; // Current event being processed
static monitor_evt_t monitor_e_buffer[MONITOR_TASK_NUM_EVENTS] = {0}; // Array to hold shell events

static state_t monitor_state_process_handler(monitor_task_t * const me, monitor_evt_t const * const e);


static void monitor_task_init(monitor_task_t * const me, monitor_evt_t const * const e)
{
	bsp_ntc_adc_init();
	bsp_laser_adc_init();
	SST_TimeEvt_arm(&me->monitor_task_timer, MONITOR_TASK_INTERVAL, MONITOR_TASK_INTERVAL);
}


void monitor_task_ctor(monitor_task_t * const me, monitor_init_t const * const init) {
    DBC_ASSERT(0u, me != NULL);
    SST_Task_ctor(&me->super, (SST_Handler) monitor_task_init, (SST_Handler)monitor_state_process_handler, \
                                (SST_Evt *)init->current_evt, init->event_buffer);
    SST_TimeEvt_ctor(&me->monitor_task_timer, EVT_MONITOR_NTC_TRIGGER_TIME, &(me->super));

    me->state = init->init_state;
    me->adc_data.laser_current[0] = 0;		//set current laser int = 0
    SST_TimeEvt_disarm(&me->monitor_task_timer);
}

void adc_monitor_task_ctor_singleton()
{
	circular_buffer_init(&monitor_e_queue,(uint8_t *)&monitor_e_buffer,sizeof(monitor_e_buffer),MONITOR_TASK_NUM_EVENTS,sizeof(monitor_evt_t));
	monitor_init_t init = {
		 .init_state = monitor_state_process_handler,
		 .event_buffer = &monitor_e_queue,
		 .current_evt = &current_monitor_e
	};
	monitor_task_ctor(&monitor_task_inst, &init);
}

void adc_monitor_task_start(uint8_t priority)
{
	SST_Task_start(&monitor_task_inst.super,priority);
}

static state_t monitor_state_process_handler(monitor_task_t * const me, monitor_evt_t const * const e)
{
	wdg_feed(WDG_ADC_MONITOR_ID);
	switch (e->super.sig)
	{
		case EVT_MONITOR_NTC_TRIGGER_TIME:
		{
			bsp_ntc_trigger_adc();
//			bsp_laser_ext_trigger_adc();
//			bsp_laser_int_trigger_adc();
			bsp_laser_trigger_adc();
			return HANDLED_STATUS;
		}

		case EVT_MONITOR_NTC_ADC_COMPLETED:
		{
			for (uint8_t i = 0; i < 8; i++ )
			{
				me->adc_data.ntc_temperature[i] = bsp_ntc_get_temperature(i);
			}
			return HANDLED_STATUS;
		}

		case EVT_MONITOR_LD_ADC_COMPLETED:
		{
			me->adc_data.laser_current[0] = bsp_laser_get_int_current();
			me->adc_data.laser_current[1] = bsp_laser_get_ext_current();

			DBG(DBG_LEVEL_INFO, "			%d		%d\r\n", me->adc_data.laser_current[0], me->adc_data.laser_current[1]);
		}
		default:
			return IGNORED_STATUS;
	}
}

int16_t laser_monitor_get_laser_current(uint32_t channel)
{
	return monitor_task_inst.adc_data.laser_current[channel];
}

uint16_t temperature_monitor_get_ntc_error(uint32_t channel1, uint32_t channel2, int16_t max_temp, int16_t min_temp)
{
	int16_t ntc[2] = {monitor_task_inst.adc_data.ntc_temperature[channel1], monitor_task_inst.adc_data.ntc_temperature[channel2]};
	if((ntc[0] < min_temp) || (ntc[0] > max_temp) || (abs(ntc[0] - ntc[1]) > TEMPERATURE_MAX_ERROR))
	{
		return ERROR_FAIL;
	}
	return ERROR_OK;
}

int16_t temperature_monitor_get_ntc_temperature(uint32_t channel)
{
	return monitor_task_inst.adc_data.ntc_temperature[channel];
}
