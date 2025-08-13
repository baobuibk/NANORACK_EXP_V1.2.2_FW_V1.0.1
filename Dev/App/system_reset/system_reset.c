/*
 * system_reset.c
 *
 *  Created on: Jul 19, 2025
 *      Author: HTSANG
 */

#include "system_reset.h"
#include "adc_monitor.h"
#include "app_signals.h"
#include "system_reset.h"
#include "error_codes.h"
#include "dbc_assert.h"
#include "main.h"
#include "wdg.h"

//DBC_MODULE_NAME("system_reset")

#define SYSTEM_RESET_NUM_EVENT					10
#define DEFAULT_RESET_POLL_TIME					5000

system_reset_task_t system_reset_task_inst;
circular_buffer_t system_reset_event_queue = {0};
static system_reset_evt_t system_reset_current_event = {0};
static system_reset_evt_t system_reset_event_buffer[SYSTEM_RESET_NUM_EVENT];


static void system_reset_task_init(system_reset_task_t * const me, system_reset_evt_t * const e);
static void system_reset_task_dispatch(system_reset_task_t * const me, system_reset_evt_t * const e);
static state_t system_reset_normal_state_handler(system_reset_task_t * const me, system_reset_evt_t * const e);

void system_reset_house_keeping(void);
void system_reset_request(void);

static void system_reset_task_dispatch(system_reset_task_t * const me, system_reset_evt_t * const e)
{
    (me->state)(me, e);
}

void system_reset_task_ctor(system_reset_task_t * const me, system_reset_task_init_t * const init)
{
	SST_Task_ctor(&me->super, (SST_Handler)system_reset_task_init, (SST_Handler)system_reset_task_dispatch, (SST_Evt*)init->current_evt, init->event_buffer);
	SST_TimeEvt_ctor(&me->system_reset_timer, EVT_SYSTEM_RESET_POLL, &me->super);
	SST_TimeEvt_ctor(&me->system_reset_delay_time, EVT_SYSTEM_RESET_REQUEST, &me->super);
	me->state = init->init_state;
	me->interval = DEFAULT_RESET_POLL_TIME;
	SST_TimeEvt_disarm(&me->system_reset_timer);
	SST_TimeEvt_disarm(&me->system_reset_delay_time);
}

void system_reset_task_ctor_singleton()
{
	system_reset_task_init_t init = {
			.current_evt  = &system_reset_current_event,
			.event_buffer = &system_reset_event_queue,
			.init_state   = system_reset_normal_state_handler
	};
	circular_buffer_init(&system_reset_event_queue, (uint8_t *)&system_reset_event_buffer, sizeof(system_reset_event_buffer), SYSTEM_RESET_NUM_EVENT, sizeof(system_reset_evt_t));
	system_reset_task_ctor(&system_reset_task_inst, &init);
}

static void system_reset_task_init(system_reset_task_t * const me, system_reset_evt_t * const e)
{
	SST_TimeEvt_arm(&me->system_reset_timer, me->interval, me->interval);
}

void system_reset_task_start(uint8_t priority)
{
	SST_Task_start(&system_reset_task_inst.super, priority);
}
static state_t system_reset_normal_state_handler(system_reset_task_t * const me, system_reset_evt_t * const e)
{
	switch (e->super.sig)
	{
		case EVT_SYSTEM_RESET_POLL:
			system_reset_house_keeping();
			return HANDLED_STATUS;
		case EVT_SYSTEM_RESET_REQUEST:
			system_reset_request();
			return HANDLED_STATUS;
		default:
			return IGNORED_STATUS;
	}
}

void system_reset_house_keeping(void)
{
	LL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
}

void system_reset_request(void)
{
	System_On_Bootloader_Reset();
}

void system_reset(system_reset_task_t * const me)
{
	SST_TimeEvt_arm(&me->system_reset_delay_time, 500, 0);
}
