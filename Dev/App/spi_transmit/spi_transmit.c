/*
 * spi_transmit.c
 *
 *  Created on: Jul 23, 2025
 *      Author: HTSANG
 */


#include "../spi_transmit/spi_transmit.h"

#include "app_signals.h"
#include "bsp_handshake.h"
#include "wdg.h"
#include "configs.h"
#include "bsp_spi_slave.h"

//DBC_MODULE_NAME("spi_transmit")

#define SPI_TRANSMIT_NUM_EVENT 			5
#define FINISH_POLL_TIME 				5			// ms
#define TIME_OUT_SPI_TRANS				60000		// poll

spi_transmit_task_t spi_transmit_task_inst;
circular_buffer_t spi_transmit_event_queue = {0};
static spi_transmit_evt_t spi_transmit_current_event = {0};
static spi_transmit_evt_t spi_transmit_event_buffer[SPI_TRANSMIT_NUM_EVENT];

static void spi_transmit_task_init(spi_transmit_task_t * const me, spi_transmit_evt_t * const e);
static void spi_transmit_task_dispatch(spi_transmit_task_t * const me, spi_transmit_evt_t * const e);
static state_t spi_transmit_handler(spi_transmit_task_t * const me, spi_transmit_evt_t * const e);

//static void spi_transmit_task_finish(spi_transmit_task_t *const me);

void spi_transmit_task_ctor(spi_transmit_task_t * const me, spi_transmit_task_init_t * const init)
{
	SST_Task_ctor(&me->super, (SST_Handler)spi_transmit_task_init, (SST_Handler)spi_transmit_task_dispatch, (SST_Evt*)init->current_evt, init->event_buffer);
	SST_TimeEvt_ctor(&me->spi_transmit_poll_check_finish_timer, EVT_SPI_TRANSMIT_POLL_CHECK_FINISH, &me->super);
	me->state = init->init_state;
	me->count = 0;
	SST_TimeEvt_disarm(&me->spi_transmit_poll_check_finish_timer);
}

void spi_transmit_task_singleton_ctor()
{
	spi_transmit_task_init_t init = {
			.current_evt = &spi_transmit_current_event,
			.event_buffer = &spi_transmit_event_queue,
			.init_state = spi_transmit_handler
	};
	circular_buffer_init(&spi_transmit_event_queue, (uint8_t *)&spi_transmit_event_buffer, sizeof(spi_transmit_event_buffer), SPI_TRANSMIT_NUM_EVENT, sizeof(spi_transmit_evt_t));
	spi_transmit_task_ctor(&spi_transmit_task_inst,&init);
}

static void spi_transmit_task_init(spi_transmit_task_t * const me, spi_transmit_evt_t * const e)
{
	bsp_handshake_spi_busy();
}

static void spi_transmit_task_dispatch(spi_transmit_task_t * const me, spi_transmit_evt_t * const e)
{
    (me->state)(me, e);
}

void spi_transmit_task_start(uint8_t priority)
{
	SST_Task_start(&spi_transmit_task_inst.super, priority);
}

static state_t spi_transmit_handler(spi_transmit_task_t * const me, spi_transmit_evt_t * const e)
{
	switch (e->super.sig)
	{
		case EVT_SPI_TRANSMIT_POLL_CHECK_FINISH:
			wdg_feed(WDG_SPI_TRANSMIT_ID);
			me->count ++;
//			if (bsp_handshake_spi_check_finish() || (me->count > TIME_OUT_SPI_TRANS))
			if (bsp_handshake_spi_check_finish())
			{
				spi_trans_debug_print("........................spi trans finish______________________________\r\n");
				me->count = 0;
				bsp_handshake_spi_busy();
				wdg_unregister(WDG_SPI_TRANSMIT_ID);
				SST_TimeEvt_disarm(&me->spi_transmit_poll_check_finish_timer);
//				spi_transmit_task_finish(me);
			}
			else if ((me->count > TIME_OUT_SPI_TRANS))
			{
				spi_trans_debug_print("........................spi trans timeout______________________________\r\n");
				me->count = 0;
				bsp_handshake_spi_busy();
				wdg_unregister(WDG_SPI_TRANSMIT_ID);
				SST_TimeEvt_disarm(&me->spi_transmit_poll_check_finish_timer);
			}

			return HANDLED_STATUS;
//		case EVT_SPI_TRANSMIT_FINISH:
//		case EVT_SPI_TRANSMIT_TIMEOUT:

//			LL_DMA_ClearFlag_TC3(DMA2);
//			SPI_SlaveDevice_SetTransferState(SPI_TRANSFER_COMPLETE);
//			SPI_SlaveDevice_Disable();

//			spi_trans_debug_print("........................spi trans finish______________________________\r\n");
//			bsp_handshake_spi_busy();
//			return HANDLED_STATUS;

		default:
			return IGNORED_STATUS;
	}
}

void spi_transmit_task_data_ready(spi_transmit_task_t *const me)
{
	spi_trans_debug_print("........................spi data ready______________________________\r\n");
	bsp_handshake_spi_ready();
	SST_TimeEvt_arm(&me->spi_transmit_poll_check_finish_timer, FINISH_POLL_TIME, FINISH_POLL_TIME);
	wdg_register(WDG_SPI_TRANSMIT_ID, WDG_SPI_TRANSMIT_TIMEOUT);
}

//void spi_transmit_task_finish(spi_transmit_task_t *const me)
//{
//	spi_transmit_evt_t spi_finish_evt = {.super = {.sig = EVT_SPI_TRANSMIT_FINISH}, };
//	SST_Task_post(&me->super, (SST_Evt *)&spi_finish_evt);
//	wdg_unregister(WDG_SPI_TRANSMIT_ID);
//}
