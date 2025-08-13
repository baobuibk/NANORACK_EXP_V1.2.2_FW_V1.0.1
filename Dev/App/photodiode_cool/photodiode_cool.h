/*
 * tec_control.h
 *
 *  Created on: Jun 16, 2025
 *      Author: Admin
 */

#ifndef APP_tec_ovr_CONTROL_tec_ovr_CONTROL_TASK_H_
#define APP_tec_ovr_CONTROL_tec_ovr_CONTROL_TASK_H_

#include "sst.h"
#include "fsm.h"
#include "main.h"

//#define PHOTO_COOL_DEBUG_PRINTING
#ifdef PHOTO_COOL_DEBUG_PRINTING
    #define photo_cool_debug_print(...) DBG(0,__VA_ARGS__)
#else
	#define photo_cool_debug_print(...)
#endif


#define TEC_OVR_CONTROL_COMMAND_PAYLOAD_LENGTH	15




typedef struct tec_ovr_control_task_t tec_ovr_control_task_t;
typedef struct tec_ovr_control_evt_t tec_ovr_control_evt_t;
typedef struct tec_ovr_control_task_init_t tec_ovr_control_task_init_t;
typedef enum{TEC_OVR_OFF, TEC_OVR_COOL} tec_ovr_state_t;

typedef state_t (*tec_ovr_control_state_handler_t)(tec_ovr_control_task_t * const me, tec_ovr_control_evt_t  const * const e);

struct tec_ovr_control_evt_t
{
	SST_Evt super;
	uint8_t cmd;
	uint8_t payload[TEC_OVR_CONTROL_COMMAND_PAYLOAD_LENGTH];
};


struct tec_ovr_control_task_t
{
    SST_Task super;
    tec_ovr_control_state_handler_t state; /* the "state variable" */
    SST_TimeEvt tec_ovr_control_task_timeout_timer;
    tec_ovr_state_t tec_ovr_state;
};

struct tec_ovr_control_task_init_t {
	tec_ovr_control_state_handler_t init_state;
	tec_ovr_control_evt_t * current_evt;
	circular_buffer_t * tec_ovr_control_task_event_buffer;
};

void tec_ovr_control_task_singleton_ctor(void);
void tec_ovr_control_task_start(uint8_t priority);

uint32_t tec_ovr_start(void);
uint32_t tec_ovr_stop(void);

void tec_over_set_interval(uint16_t time_mS);


#endif /* APP_tec_ovr_CONTROL_tec_ovr_CONTROL_TASK_H_ */
