/*
 * tec_control_task.c
 *
 *  Created on: Jun 16, 2025
 *      Author: Admin
 */
#include "bsp_tec.h"
#include "dbc_assert.h"
#include "app_signals.h"
#include "error_codes.h"
#include "uart_dbg.h"
#include "stddef.h"
#include "photodiode_cool.h"
#include "temperature_control.h"


DBC_MODULE_NAME("tec_ovr_control")

tec_ovr_control_task_t tec_ovr_control_task_inst;

#define TEC_OVR_DEFAULT_INTERVAL 			5000 //loop to control TEC every 10s
#define TEC_OVR_CONTROL_TASK_NUM_EVENTS 	2

static tec_ovr_control_evt_t tec_ovr_control_current_event = {0};
static tec_ovr_control_evt_t tec_ovr_control_task_event_buffer[TEC_OVR_CONTROL_TASK_NUM_EVENTS] = {0};
static tec_ovr_control_evt_t const entry_evt = {.super = {.sig = SIG_ENTRY} };
static tec_ovr_control_evt_t const exit_evt = {.super = {.sig = SIG_EXIT} };

circular_buffer_t tec_ovr_control_task_event_queue = {0};

extern temperature_control_task_t temperature_control_task_inst ;
static temperature_control_task_t *p_temperature_control_task = &temperature_control_task_inst;

static uint16_t override_interval = TEC_OVR_DEFAULT_INTERVAL;

static state_t tec_ovr_control_handler(tec_ovr_control_task_t * const me, tec_ovr_control_evt_t const * const e);


static void tec_ovr_control_task_init(tec_ovr_control_task_t * const me,tec_ovr_control_evt_t const * const e)
{
	SST_TimeEvt_disarm(&me->tec_ovr_control_task_timeout_timer);
}

static void tec_ovr_control_task_dispatch(tec_ovr_control_task_t * const me, tec_ovr_control_evt_t const * const e)
{
    tec_ovr_control_state_handler_t prev_state = me->state; /* save for later */
    state_t status = (me->state)(me, e);

    if (status == TRAN_STATUS) { /* transition taken? */
        (prev_state)(me, &exit_evt);
        (me->state)(me, &entry_evt);
    }
}

void tec_ovr_control_task_ctor(tec_ovr_control_task_t * const me, tec_ovr_control_task_init_t const * const init) {
    DBC_ASSERT(0u, me != NULL);
    SST_Task_ctor(&me->super, (SST_Handler) tec_ovr_control_task_init, (SST_Handler)tec_ovr_control_task_dispatch, \
                                (SST_Evt *)init->current_evt, init->tec_ovr_control_task_event_buffer);
    SST_TimeEvt_ctor(&me->tec_ovr_control_task_timeout_timer, EVT_TEC_OVR_POLL_TIME, &(me->super));
    me->state = init->init_state; // Set the initial state to process handler
    me->tec_ovr_state = TEC_OVR_OFF;
    SST_TimeEvt_disarm(&me->tec_ovr_control_task_timeout_timer); // Disarm the timeout timer
}

void tec_ovr_control_task_singleton_ctor(void)
{
	circular_buffer_init(&tec_ovr_control_task_event_queue, (uint8_t * )&tec_ovr_control_task_event_buffer, sizeof(tec_ovr_control_task_event_buffer), TEC_OVR_CONTROL_TASK_NUM_EVENTS, sizeof(tec_ovr_control_evt_t));
	tec_ovr_control_task_init_t init =
	{
		.init_state = tec_ovr_control_handler,
		.current_evt = &tec_ovr_control_current_event,
		.tec_ovr_control_task_event_buffer = &tec_ovr_control_task_event_queue
	};
	tec_ovr_control_task_ctor(&tec_ovr_control_task_inst,&init);
}

void tec_ovr_control_task_start(uint8_t priority)
{
	SST_Task_start(&tec_ovr_control_task_inst.super,priority);
}

static state_t tec_ovr_control_handler(tec_ovr_control_task_t * const me, tec_ovr_control_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			return HANDLED_STATUS;
		}

		case EVT_TEC_OVR_POLL_TIME:
		{
			if(me->tec_ovr_state == TEC_OVR_OFF)
			{
				photo_cool_debug_print("tec_ovr on\r\n");
				temperature_profile_tec_ovr_enable(p_temperature_control_task);
				me->tec_ovr_state = TEC_OVR_COOL;
			}
			else
			{
				photo_cool_debug_print("tec_ovr off\r\n");
				temperature_profile_tec_ovr_disable(p_temperature_control_task);
				me->tec_ovr_state = TEC_OVR_OFF;
			}
			return HANDLED_STATUS;
		}

		default: return IGNORED_STATUS;
	}
}


uint32_t tec_ovr_start(void)
{
	if (temperature_profile_tec_ovr_enable(p_temperature_control_task))
		return ERROR_FAIL;
	SST_TimeEvt_arm(&tec_ovr_control_task_inst.tec_ovr_control_task_timeout_timer, override_interval, override_interval);
	tec_ovr_control_task_inst.tec_ovr_state = TEC_OVR_COOL;
	return ERROR_OK;
}

uint32_t tec_ovr_stop(void)
{
	SST_TimeEvt_disarm(&tec_ovr_control_task_inst.tec_ovr_control_task_timeout_timer);
	temperature_profile_tec_ovr_disable(p_temperature_control_task);
	tec_ovr_control_task_inst.tec_ovr_state = TEC_OVR_OFF;
	return ERROR_OK;
}

void tec_over_set_interval(uint16_t time_mS)
{
	override_interval = time_mS;
}



