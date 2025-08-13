/*
 * system_log.c
 *
 *  Created on: Jul 14, 2025
 *      Author: Admin
 */
#include "lwl.h"
#include "app_signals.h"
#include "system_log.h"
#include "error_codes.h"
#include "dbc_assert.h"
#include "date_time.h"
#include "configs.h"
#include "wdg.h"

//#include "adc_monitor.h"
#include "bsp_ntc.h"

//DBC_MODULE_NAME("system_log")

#define SYSTEM_LOG_NUM_EVENT 		1
#define DEFAULT_POLL_TIME 			1000

system_log_task_t system_log_task_inst;
circular_buffer_t system_log_event_queue = {0};
static system_log_evt_t system_log_current_event = {0};
static system_log_evt_t system_log_event_buffer[SYSTEM_LOG_NUM_EVENT];

static void system_log_task_init(system_log_task_t * const me, system_log_evt_t * const e);
//static void system_log_task_dispatch(system_log_task_t * const me, system_log_evt_t * const e);
static state_t system_log_normal_state_handler(system_log_task_t * const me, system_log_evt_t * const e);

void system_log_house_keeping(system_log_task_t * const me);

void system_log_task_ctor(system_log_task_t * const me, system_log_task_init_t * const init)
{
	SST_Task_ctor(&me->super, (SST_Handler)system_log_task_init, (SST_Handler)system_log_normal_state_handler, (SST_Evt*)init->current_evt, init->event_buffer);
	SST_TimeEvt_ctor(&me->system_log_timer, EVT_SYSTEM_LOG_POLL, &me->super);
	me->state = init->init_state;
	me->interval = DEFAULT_POLL_TIME;
	me->ntc_log_mask = 0xFF;
	SST_TimeEvt_disarm(&me->system_log_timer);
}

void system_log_task_ctor_singleton()
{
	circular_buffer_init(&system_log_event_queue, (uint8_t *)system_log_event_buffer, sizeof(system_log_event_buffer), SYSTEM_LOG_NUM_EVENT, sizeof(system_log_evt_t));
	system_log_task_init_t init = {
			.current_evt = &system_log_current_event,
			.event_buffer = &system_log_event_queue,
			.init_state = system_log_normal_state_handler
	};
	system_log_task_ctor(&system_log_task_inst,&init);
}

static void system_log_task_init(system_log_task_t * const me, system_log_evt_t * const e)
{
	lwl_start();
	SST_TimeEvt_arm(&me->system_log_timer, me->interval, me->interval);
}

void system_log_task_start(uint8_t priority)
{
	SST_Task_start(&system_log_task_inst.super, priority);
}
static state_t system_log_normal_state_handler(system_log_task_t * const me, system_log_evt_t * const e)
{
	switch (e->super.sig)
	{
	case EVT_SYSTEM_LOG_POLL:
		wdg_feed(WDG_SYSTEM_LOG_ID);
		system_log_house_keeping(me);
	}
	return HANDLED_STATUS;
}


void system_log_house_keeping(system_log_task_t * const me)
{
	uint8_t days, hours, minutes, seconds;
	date_time_get(&days, &hours, &minutes, &seconds);
	LWL(LWL_EXP_TIMESTAMP, LWL_1(days), LWL_1(hours), LWL_1(minutes), LWL_1(seconds));
	for (uint32_t i=0;i<8;i++)
		if (me->ntc_log_mask & (0x01 << i))
			LWL(LWL_EXP_TEMP_SINGLE_NTC, LWL_1(i), LWL_2(bsp_ntc_get_temperature(i)));
}

void system_log_set_interval(uint32_t interval)
{
	system_log_task_inst.interval = interval;
}

void system_log_enable()
{
	SST_TimeEvt_arm(&system_log_task_inst.system_log_timer, system_log_task_inst.interval, system_log_task_inst.interval);
}

void system_log_disable()
{
	SST_TimeEvt_disarm(&system_log_task_inst.system_log_timer);
}

void system_log_task_set_ntc_log_mask(system_log_task_t *const me, uint8_t mask)
{
	me->ntc_log_mask = mask;
}
