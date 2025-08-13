/*
 * wdg_task.h
 *
 *  Created on: Jul 24, 2025
 *      Author: Admin
 */

#ifndef APP_WDG_WDG_TASK_H_
#define APP_WDG_WDG_TASK_H_

#include "sst.h"
#include "fsm.h"


typedef struct wdg_task_t wdg_task_t;
typedef struct wdg_task_evt_t  wdg_task_evt_t;
typedef struct wdg_task_init_t  wdg_task_init_t;

typedef state_t (* wdg_task_handler_t )(wdg_task_t * const me, wdg_task_evt_t * const e);

struct wdg_task_evt_t{
	SST_Evt super;
//	uint32_t data;
};
struct wdg_task_t{
	SST_Task super;
	SST_TimeEvt wdg_timer;
	wdg_task_handler_t state; /* the "state variable" */
	uint32_t interval;
};

struct wdg_task_init_t {
	wdg_task_handler_t init_state;
	wdg_task_evt_t * current_evt;
	circular_buffer_t * event_buffer;
};


void wdg_task_ctor_singleton(void);
void wdg_task_start(uint8_t priority);

#endif /* APP_WDG_WDG_TASK_H_ */
