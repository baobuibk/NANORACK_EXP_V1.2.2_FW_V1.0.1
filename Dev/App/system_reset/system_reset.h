/*
 * system_reset.h
 *
 *  Created on: Jul 19, 2025
 *      Author: HTSANG
 */

#ifndef APP_SYSTEM_RESET_SYSTEM_RESET_H_
#define APP_SYSTEM_RESET_SYSTEM_RESET_H_

#include "sst.h"
#include "fsm.h"

typedef struct system_reset_task_t system_reset_task_t;
typedef struct system_reset_evt_t system_reset_evt_t;
typedef struct system_reset_task_init_t system_reset_task_init_t;

typedef state_t (*system_reset_task_handler_t)(system_reset_task_t *const me,
		system_reset_evt_t *const e);

struct system_reset_evt_t {
	SST_Evt super;
//	uint32_t data;
};
struct system_reset_task_t {
	SST_Task super;
	SST_TimeEvt system_reset_timer;
	SST_TimeEvt system_reset_delay_time;
	system_reset_task_handler_t state; /* the "state variable" */
	uint32_t interval;
};

struct system_reset_task_init_t {
	system_reset_task_handler_t init_state;
	system_reset_evt_t *current_evt;
	circular_buffer_t *event_buffer;
};

void system_reset_task_ctor_singleton(void);
void system_reset_task_start(uint8_t priority);

void system_reset(system_reset_task_t * const me);

#endif /* APP_SYSTEM_RESET_SYSTEM_RESET_H_ */
