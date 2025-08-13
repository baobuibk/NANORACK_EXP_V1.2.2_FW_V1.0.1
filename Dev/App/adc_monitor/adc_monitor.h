///*
// * temperature_monitor.h
// *
// *  Created on: Jun 21, 2025
// *      Author: Admin
// */
//
//#ifndef APP_MONITOR_TEMPERATURE_MONITOR_H_
//#define APP_MONITOR_TEMPERATURE_MONITOR_H_
//
//#include "sst.h"
//#include "fsm.h"
//
//#define TEMPERATURE_MAX_ERROR 50		//5.0C
//
//typedef struct monitor_task_t monitor_task_t;
//typedef struct monitor_evt_t  monitor_evt_t;
//typedef struct monitor_init_t  monitor_init_t;
//typedef struct monitor_data_t {
//	int16_t	ntc_temperature[8]; //8 channels for ntc
//	int16_t laser_current[2]; // 2 channels for laser
//}monitor_data_t;
//typedef state_t (* monitor_state_handler_t )(struct monitor_task_t * const me, monitor_evt_t const * const e);
//
//struct monitor_evt_t{
//	SST_Evt super;
//};
//struct monitor_task_t{
//	SST_Task super;
//	SST_TimeEvt monitor_task_timer;
//	monitor_state_handler_t state; /* the "state variable" */
//	monitor_data_t adc_data;
//};
//
//struct monitor_init_t {
//	monitor_state_handler_t init_state;
//	monitor_evt_t * current_evt;
//	circular_buffer_t * event_buffer;
//};
//
//void adc_monitor_task_ctor_singleton(void);
//void adc_monitor_task_start(uint8_t priority);
//
//uint16_t temperature_monitor_get_ntc_error(uint32_t channel1, uint32_t channel2, int16_t max_temp, int16_t min_temp);
//int16_t laser_monitor_get_laser_current(uint32_t channel);
//int16_t temperature_monitor_get_ntc_temperature(uint32_t channel);
//#endif /* APP_TEMPERATURE_MONITOR_TEMPERATURE_MONITOR_H_ */
