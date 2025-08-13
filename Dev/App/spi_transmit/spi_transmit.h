/*
 * spi_transmit.h
 *
 *  Created on: Jul 23, 2025
 *      Author: HTSANG
 */

#ifndef APP_SPI_TRANSMIT_SPI_TRANSMIT_H_
#define APP_SPI_TRANSMIT_SPI_TRANSMIT_H_

#include "sst.h"
#include "fsm.h"
#include "main.h"

//#define SPI_TRANS_DEBUG_PRINTING
#ifdef SPI_TRANS_DEBUG_PRINTING
    #define spi_trans_debug_print(...) DBG(0,__VA_ARGS__)
#else
	#define spi_trans_debug_print(...)
#endif

typedef struct spi_transmit_task_t spi_transmit_task_t;
typedef struct spi_transmit_evt_t  spi_transmit_evt_t;
typedef struct spi_transmit_task_init_t  spi_transmit_task_init_t;

typedef state_t (* spi_transmit_task_handler_t )(spi_transmit_task_t * const me, spi_transmit_evt_t * const e);

struct spi_transmit_evt_t{
	SST_Evt super;
};
struct spi_transmit_task_t{
	SST_Task super;
	SST_TimeEvt spi_transmit_poll_check_finish_timer;
	spi_transmit_task_handler_t state; /* the "state variable" */
	uint32_t count;
};

struct spi_transmit_task_init_t {
	spi_transmit_task_handler_t init_state;
	spi_transmit_evt_t * current_evt;
	circular_buffer_t * event_buffer;
};


void spi_transmit_task_singleton_ctor(void);
void spi_transmit_task_start(uint8_t priority);


void spi_transmit_task_data_ready(spi_transmit_task_t *const me);


#endif /* APP_SPI_TRANSMIT_SPI_TRANSMIT_H_ */
