/*
 * wdg_task.c
 *
 *  Created on: Jul 24, 2025
 *      Author: Admin
 */
#include "app_signals.h"
#include "wdg_task.h"
#include "wdg.h"
#include "bsp_watchdog.h"

wdg_task_t wdg_task_inst;


#define WDG_NUM_EVENT 		1
#define WDG_POLL_TIME 		100 	// MS


wdg_task_evt_t wdg_task_current_event = {0};
wdg_task_evt_t wdg_task_event_buffer[WDG_NUM_EVENT];
circular_buffer_t wdg_task_event_queue = {0};

static void wdg_task_init(wdg_task_t * const me, wdg_task_evt_t * const e);
static state_t wdg_update_state_handler(wdg_task_t * const me, wdg_task_evt_t * const e);

void wdg_task_ctor(wdg_task_t * const me, wdg_task_init_t * const init)
{
	SST_Task_ctor(&me->super, (SST_Handler)wdg_task_init, (SST_Handler)wdg_update_state_handler, (SST_Evt*)init->current_evt, init->event_buffer);
	SST_TimeEvt_ctor(&me->wdg_timer, EVT_WDG_POLL, &me->super);
	me->state = init->init_state;
	me->interval = WDG_POLL_TIME;
	SST_TimeEvt_disarm(&me->wdg_timer);
}

void wdg_task_ctor_singleton()
{
	circular_buffer_init(&wdg_task_event_queue, (uint8_t *)wdg_task_event_buffer, sizeof(wdg_task_event_buffer), WDG_NUM_EVENT, sizeof(wdg_task_evt_t));

	wdg_task_init_t init = {
			.current_evt = &wdg_task_current_event,
			.event_buffer = &wdg_task_event_queue,
			.init_state = wdg_update_state_handler
	};
	wdg_task_ctor(&wdg_task_inst,&init);
}


static void wdg_task_init(wdg_task_t * const me, wdg_task_evt_t * const e)
{
	wdg_init();
	wdg_start_hdw_wdg(WDG_INIT_TIMEOUT_MS);
	SST_TimeEvt_arm(&me->wdg_timer, WDG_POLL_TIME, WDG_POLL_TIME);
}

void wdg_task_start(uint8_t priority)
{
	SST_Task_start(&wdg_task_inst.super, priority);
}
static state_t wdg_update_state_handler(wdg_task_t * const me, wdg_task_evt_t * const e)
{
	// assume the only event is SYSTEM_LOG_POLL
	switch (e->super.sig)
	{
	case EVT_WDG_POLL:
		wdg_update();
	}
	return HANDLED_STATUS;
}




