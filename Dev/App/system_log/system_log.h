/*
 * system_log.h
 *
 *  Created on: Jul 14, 2025
 *      Author: Admin
 */

#ifndef APP_SYSTEM_LOG_SYSTEM_LOG_H_
#define APP_SYSTEM_LOG_SYSTEM_LOG_H_

#include "sst.h"
#include "fsm.h"

typedef struct system_log_task_t system_log_task_t;
typedef struct system_log_evt_t  system_log_evt_t;
typedef struct system_log_task_init_t  system_log_task_init_t;

typedef state_t (* system_log_task_handler_t )(system_log_task_t * const me, system_log_evt_t * const e);

struct system_log_evt_t{
	SST_Evt super;
//	uint32_t data;
};
struct system_log_task_t{
	SST_Task super;
	SST_TimeEvt system_log_timer;
	system_log_task_handler_t state; /* the "state variable" */
	uint32_t	interval;
	uint8_t ntc_log_mask;
};

struct system_log_task_init_t {
	system_log_task_handler_t init_state;
	system_log_evt_t * current_evt;
	circular_buffer_t * event_buffer;
};


void system_log_task_ctor_singleton(void);
void system_log_task_start(uint8_t priority);

void system_log_task_set_ntc_log_mask(system_log_task_t *const me, uint8_t mask);
#endif /* APP_SYSTEM_LOG_SYSTEM_LOG_H_ */
