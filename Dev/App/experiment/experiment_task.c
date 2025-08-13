/*
 * experiment_task.c
 *
 *  Created on: Jun 25, 2025
 *      Author: Admin
 */
#include "experiment_task.h"
#include "bsp_photodiode.h"
#include "bsp_laser.h"
#include "bsp_spi_ram.h"
#include "dbc_assert.h"
#include "app_signals.h"
#include "error_codes.h"
#include "uart_dbg.h"
#include "stddef.h"
#include "string.h"
#include "shell.h"
#include "min_shell_command.h"
#include "bsp_spi_slave.h"
#include "bsp_handshake.h"
#include "../spi_transmit/spi_transmit.h"
#include "adc_monitor.h"
#include "bsp_bkram.h"
#include "bsp_ntc.h"
#include "bsp_laser.h"
#include "lwl.h"

DBC_MODULE_NAME("experiment_task")

//////////////////////// BK RAM ////////////////////////
struct experiment_profile_backup_RAM_t
{
	uint32_t magic;
	uint32_t sampling_rate;										// Hz		(Sample/second)
	uint16_t pre_time; //time before switching					// mS
	uint16_t experiment_time;//time when switch the laser on	// mS
	uint16_t post_time; //time after switching off the laser	// mS

	uint32_t crc;
};

struct experiment_profile_backup_RAM_t experiment_profile_bkram  __attribute__((section (".bkpram")));

#define EXPERIMENT_VARS_MAGIC			0xbcbcbcbc

static void experiment_update_profile(void);

static bool experiment_validate_profile(void);

//////////////////////// BK RAM ////////////////////////

///////////////////// CLI CONSOLE //////////////////////
#include "embedded_cli.h"
extern EmbeddedCli * shell_uart_cli;
///////////////////// CLI CONSOLE //////////////////////


#define EXPERIMENT_TASK_NUM_EVENTS  		10
#define EXPERIMENT_TASK_AQUI_TIMEOUT		20000

#define EXPERIMENT_LASER_CURRENT_POLL_TIME	10

extern shell_task_t shell_task_inst;
static shell_task_t *p_shell_task = &shell_task_inst;

extern min_shell_task_t min_shell_task_inst;
static min_shell_task_t *p_min_shell_task = &min_shell_task_inst;

extern spi_transmit_task_t spi_transmit_task_inst;
static spi_transmit_task_t *p_spi_transmit_task = &spi_transmit_task_inst;

experiment_task_t experiment_task_inst;
circular_buffer_t experiment_task_event_queue = {0}; // Circular buffer to hold shell events
static experiment_evt_t experiment_task_current_event = {0}; // Current event being processed
static experiment_evt_t experiment_task_event_buffer[EXPERIMENT_TASK_NUM_EVENTS];
static experiment_evt_t const entry_evt = {.super = {.sig = SIG_ENTRY} };
static experiment_evt_t const exit_evt = {.super = {.sig = SIG_EXIT} };
static experiment_evt_t const start_measuring_evt = {.super = {.sig = EVT_EXPERIMENT_START_MEASURE} };
static experiment_evt_t const start_send_to_shell_evt = {.super = {.sig = EVT_EXPERIMENT_START_SEND_TO_SHELL} };
static experiment_evt_t const start_send_to_spi_evt = {.super = {.sig = EVT_EXPERIMENT_START_SEND_TO_SPI} };
static experiment_evt_t const done_send_header_evt = {.super = {.sig = EVT_EXPERIMENT_DONE_SEND_HEADER},};
static experiment_evt_t const done_read_ram_evt = {.super = {.sig = EVT_EXPERIMENT_DONE_READ_RAM}};
static experiment_evt_t const done_send_chunk = {.super = {.sig = EVT_EXPERIMENT_DONE_SEND_CHUNK}};

static data_profile_t remain_data_profile;
__attribute__((aligned(4))) static uint16_t experiment_data_buffer[EXPERIMENT_BUFFER_SAMPLE_SIZE];
static uint16_t batch_size;

//__attribute__((aligned(4))) static uint16_t laser_int_current_buffer[EXPERIMENT_LASER_CURRENT_SIZE];
//static uint16_t laser_int_current_idx;

static state_t experiment_task_state_manual_handler(experiment_task_t * const me, experiment_evt_t const * const e);
static state_t experiment_task_state_data_aqui_handler(experiment_task_t * const me, experiment_evt_t const * const e);
static state_t experiment_task_state_send_to_shell_handler(experiment_task_t * const me, experiment_evt_t const * const e);
static state_t experiment_task_state_send_to_spi_handler(experiment_task_t * const me, experiment_evt_t const * const e);

static void experiment_task_init(experiment_task_t * const me,experiment_evt_t const * const e)
{
	//exp_debug_print("experiment_task init\r\n");
	bsp_laser_init();
	bsp_spi_ram_init();
	bsp_photodiode_init();

	bsp_ntc_adc_init();
	bsp_laser_adc_init();

	me->laser_spi_mode = 1;
	me->photodiode_mode = ADC_MODE;
	if (experiment_validate_profile())
	{
		EnableBackupRAM();
		me->profile.sampling_rate = experiment_profile_bkram.sampling_rate;
		me->profile.pre_time = experiment_profile_bkram.pre_time;
		me->profile.experiment_time = experiment_profile_bkram.experiment_time;
		me->profile.post_time = experiment_profile_bkram.post_time;

		DisableBackupRAM();
		me->profile.num_sample_x1024 = (me->profile.sampling_rate * ((me->profile.pre_time + me->profile.experiment_time + me->profile.post_time) /1000)) /1024;
	}
	else
	{
		me->profile.sampling_rate = 500000;
		me->profile.pre_time = 100;
		me->profile.experiment_time = 3800;
		me->profile.post_time = 100;
		me->profile.num_sample_x1024 = 1954;
	}

}

static void experiment_task_dispatch(experiment_task_t * const me,experiment_evt_t const * const e)
{
    experiment_task_handler_t prev_state = me->state; /* save for later */
    state_t status = (me->state)(me, e);

    if (status == TRAN_STATUS) { /* transition taken? */
        (prev_state)(me, &exit_evt);
        (me->state)(me, &entry_evt);
    }
}

void experiment_task_ctor(experiment_task_t * const me, experiment_task_init_t const * const init) {
    DBC_ASSERT(0u, me != NULL);
    SST_Task_ctor(&me->super, (SST_Handler) experiment_task_init, (SST_Handler)experiment_task_dispatch, \
                                (SST_Evt *)init->current_evt, init->event_buffer);
    me->state = init->init_state;
    SST_TimeEvt_ctor(&me->timeout_timer, EVT_EXPERIMENT_DATA_AQUISITION_TIMEOUT, &(me->super));
    SST_TimeEvt_disarm(&me->timeout_timer); // Disarm the timeout timer
    me->sub_state = init->sub_state;
}

void experiment_task_singleton_ctor(void)
{
	circular_buffer_init(&experiment_task_event_queue, (uint8_t * )&experiment_task_event_buffer, sizeof(experiment_task_event_buffer), EXPERIMENT_TASK_NUM_EVENTS, sizeof(experiment_evt_t));
	experiment_task_init_t init = {
			.init_state = experiment_task_state_manual_handler,
			.current_evt = &experiment_task_current_event,
			.event_buffer = &experiment_task_event_queue,
			.sub_state = NO_SUBSTATE
	};
	experiment_task_ctor(&experiment_task_inst,&init);
}

void experiment_task_start(uint8_t priority)
{
	SST_Task_start(&experiment_task_inst.super,priority);
}

//only handle command from shell
static state_t experiment_task_state_manual_handler(experiment_task_t * const me, experiment_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			SST_TimeEvt_disarm(&me->timeout_timer); //disable the timeout
			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_START_MEASURE:
		{
	    	// Add log
	    	LWL(LWL_EXP_START);

			me->state = experiment_task_state_data_aqui_handler;
			return TRAN_STATUS;
		}

		case EVT_EXPERIMENT_START_SEND_TO_SHELL:
		{
			me->state = experiment_task_state_send_to_shell_handler;
			return TRAN_STATUS;
		}

		case EVT_EXPERIMENT_START_SEND_TO_SPI:
		{
			me->state = experiment_task_state_send_to_spi_handler;
			return TRAN_STATUS;
		}

		default:
			return IGNORED_STATUS;
	}
}



static state_t experiment_task_state_data_aqui_handler(experiment_task_t * const me, experiment_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
//			cli_printf(shell_uart_cli, "Start Sampling...\r\n");
			SST_TimeEvt_arm(&me->timeout_timer, EXPERIMENT_TASK_AQUI_TIMEOUT, 0);

	      	// Switch the photodiode on
			experiment_task_photodiode_switchon(me, me->profile.pos);

			// Switch the SPI to ADC supported mode
			experiment_task_photo_ADC_prepare_SPI(me);

			// Prepare the timer for sampling
			bsp_photodiode_time_t init_photo_time;
			init_photo_time.pre_time = me->profile.pre_time ;
			init_photo_time.sampling_time = me->profile.experiment_time ;
			init_photo_time.post_time = me->profile.post_time ;
			init_photo_time.sampling_rate = me->profile.sampling_rate;
			init_photo_time.pos = me->profile.pos;

			// Set current for int laser
			bsp_laser_int_set_current(me->profile.laser_percent);

			// Set sample timer (photo adc timer)
			bsp_photo_set_time(&init_photo_time);

			// Get current data when sampling time
			bsp_laser_collect_current_data_to_buffer();

			// start sampling
			bsp_photodiode_sample_start();
			me->sub_state = S_PRE_SAMPLING;
			return HANDLED_STATUS;
		}

		case SIG_EXIT:
		{
			exp_debug_print("exit experiment_task_state_data_aqui_handler\r\n");
			SST_TimeEvt_disarm(&me->timeout_timer);
			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_FINISH_PRE_SAMPLING:
		{
//			cli_printf(shell_uart_cli, "EXPERIMENT_FINISH_PRE_SAMPLING\r\n");

			if (me->sub_state == S_PRE_SAMPLING)
			{
				me->sub_state = S_DATA_SAMPLING;
			}
			else me->sub_state = S_AQUI_ERROR;

			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_FINISH_SAMPLING:
		{
//			cli_printf(shell_uart_cli, "EXPERIMENT_FINISH_SAMPLING\r\n");

			if (me->sub_state == S_DATA_SAMPLING)
			{
				me->sub_state = S_POST_SAMPLING;
			}
			else me->sub_state = S_AQUI_ERROR;
			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_FINISH_POST_SAMPLING:
		{
	    	// Add log
	    	LWL(LWL_EXP_STOP);

//			cli_printf(shell_uart_cli, "EXPERIMENT_FINISH_POST_SAMPLING\r\n");

			if (me->sub_state == S_POST_SAMPLING)
			{
				me->sub_state = NO_SUBSTATE;
			}
			else me->sub_state = S_AQUI_ERROR;

//			cli_printf(shell_uart_cli, "Sampling Done!\r\n");

			// Cho phép giao tiếp Min với OBC
			min_shell_busy_clear(p_min_shell_task);

			me->state = experiment_task_state_manual_handler;
			return TRAN_STATUS;
		}
		case EVT_EXPERIMENT_DATA_AQUISITION_TIMEOUT:
		{
			me->sub_state = S_AQUI_TIMEOUT;
			me->state = experiment_task_state_manual_handler;
			return TRAN_STATUS;
		}

		default:
			return IGNORED_STATUS;
	}
}

static state_t experiment_task_state_send_to_shell_handler(experiment_task_t * const me, experiment_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			SST_TimeEvt_disarm(&me->timeout_timer); //disable the timeout
		    remain_data_profile.num_data = me->data_profile.num_data;
		    remain_data_profile.start_address = me->data_profile.start_address;
		    shell_uart_send_buffer_bin_evt(p_shell_task);
			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_DONE_SEND_HEADER:
		{
		    batch_size = (remain_data_profile.num_data > EXPERIMENT_BUFFER_SAMPLE_SIZE) ? EXPERIMENT_BUFFER_SAMPLE_SIZE : remain_data_profile.num_data;
			bsp_spi_ram_read_dma(remain_data_profile.start_address * 2, batch_size * 2, (uint8_t *)experiment_data_buffer);
			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_DONE_READ_RAM:
		{
			shell_send_buffer(p_shell_task, experiment_data_buffer, batch_size, me->data_profile.mode);
			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_DONE_SEND_CHUNK:
		{
			remain_data_profile.num_data -= batch_size;
			remain_data_profile.start_address += batch_size;
			if(remain_data_profile.num_data > 0)
			{
				batch_size = (remain_data_profile.num_data > EXPERIMENT_BUFFER_SAMPLE_SIZE) ? EXPERIMENT_BUFFER_SAMPLE_SIZE : remain_data_profile.num_data;
				bsp_spi_ram_read_dma(remain_data_profile.start_address * 2, batch_size * 2, (uint8_t *)experiment_data_buffer);
				return HANDLED_STATUS;
			}
			else
			{
				shell_uart_send_crc_evt(p_shell_task);
				me->state = experiment_task_state_manual_handler;
				return TRAN_STATUS;
			}
		}

		default:
			return IGNORED_STATUS;
	}
}

static state_t experiment_task_state_send_to_spi_handler(experiment_task_t * const me, experiment_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			exp_debug_print("entry SPI_TRANS\r\n");
			SST_TimeEvt_disarm(&me->timeout_timer); //disable the timeout
			bsp_spi_ram_read_dma(me->data_profile.start_address * 2, EXPERIMENT_BUFFER_SAMPLE_SIZE * 2, (uint8_t *)experiment_data_buffer);
			return HANDLED_STATUS;
		}

		case EVT_EXPERIMENT_DONE_READ_RAM:
		{
			exp_debug_print("entry DONE_READ_RAM\r\n");
			// Cấu hình địa chỉ buffer
			SPI_SlaveDevice_CollectData((uint16_t *)experiment_data_buffer);
			// Bật tín hiệu DataReady
			spi_transmit_task_data_ready(p_spi_transmit_task);
			// Quay về manual handler
			me->state = experiment_task_state_manual_handler;
			return TRAN_STATUS;
		}

		default:
			return IGNORED_STATUS;

	}
}

uint32_t experiment_task_get_ram_data(experiment_task_t * const me, uint32_t start_addr, uint32_t num_data, uint8_t mode)	// mode0: send ASCII, mode1: send binary
{
//	me->data_profile.total_data = num_data;
	me->data_profile.start_address = start_addr;
	me->data_profile.num_data = num_data;
	me->data_profile.mode = mode;
	p_shell_task->crc = 0xffff;
	SST_Task_post(&me->super, (SST_Evt *)&start_send_to_shell_evt);
	return ERROR_OK;
}

uint32_t experiment_task_data_transfer(experiment_task_t * const me)
{
//	me->data_profile.total_data = me->num_data_real;
	me->data_profile.start_address = 0;
//	me->data_profile.num_data = me->num_data_real;
	me->data_profile.num_data = 16*1024;
	me->data_profile.mode = 1;
	p_shell_task->crc = 0xffff;
	SST_Task_post(&me->super, (SST_Evt *)&start_send_to_shell_evt);
	return ERROR_OK;
}

uint32_t experiment_task_done_send_header_evt(experiment_task_t * const me)
{
	SST_Task_post(&me->super, (SST_Evt *)&done_send_header_evt);
	return ERROR_OK;
}

uint32_t experiment_task_done_read_ram_evt(experiment_task_t * const me)
{
	SST_Task_post(&me->super, (SST_Evt *)&done_read_ram_evt);
	return ERROR_OK;
}

uint32_t experiment_task_done_send_chunk(experiment_task_t * const me)
{
	SST_Task_post(&me->super, (SST_Evt *)&done_send_chunk);
	return ERROR_OK;
}


uint32_t experiment_task_laser_set_current(experiment_task_t * const me, uint32_t laser_id, uint32_t percent)
{
	if ((laser_id > 1) || (percent > 100)) return ERROR_NOT_SUPPORTED;
	if(me->laser_spi_mode != 0)
	{
		bsp_laser_set_spi_mode(SPI_MODE_0);
		me->laser_spi_mode = 0;
	}
	bsp_laser_set_current(laser_id, percent);
	me->laser_current[laser_id] = percent;
	return ERROR_OK;
}

uint32_t experiment_task_laser_get_current(experiment_task_t * const me, uint32_t laser_id)
{
	uint32_t index;
	if (laser_id > 0)
		index = 1;
	else
		index = 0;
	return me->laser_current[index];
}

uint32_t experiment_task_int_laser_switchon(experiment_task_t * const me, uint32_t laser_id)
{
	if (laser_id > INTERNAL_CHAIN_CHANNEL_NUM) return ERROR_NOT_SUPPORTED;
	if(me->laser_spi_mode != 1)
	{
		bsp_laser_set_spi_mode(SPI_MODE_1);
		me->laser_spi_mode = 1;
	}
	bsp_laser_int_switch_on(laser_id);
	me->int_laser_pos = laser_id;
	return ERROR_OK;
}

uint32_t experiment_task_int_laser_switchoff(experiment_task_t * const me)
{
	if(me->laser_spi_mode != 1)
	{
		bsp_laser_set_spi_mode(SPI_MODE_1);
		me->laser_spi_mode = 1;
	}
	bsp_laser_int_switch_off_all();
	me->int_laser_pos = 0xFF;
	return ERROR_OK;
}

uint32_t experiment_task_ext_laser_switchon(experiment_task_t * const me, uint32_t laser_id)
{
	if (laser_id > EXTERNAL_CHAIN_CHANNEL_NUM) return ERROR_NOT_SUPPORTED;
	if(me->laser_spi_mode != 1)
	{
		bsp_laser_set_spi_mode(SPI_MODE_1);
		me->laser_spi_mode = 1;
	}
	bsp_laser_ext_switch_on(laser_id);
	me->ext_laser_pos = laser_id;
	return ERROR_OK;
}

uint32_t experiment_task_ext_laser_switchoff(experiment_task_t * const me)
{
	if(me->laser_spi_mode != 1)
	{
		bsp_laser_set_spi_mode(SPI_MODE_1);
		me->laser_spi_mode = 1;
	}
	bsp_laser_ext_switch_off_all();
	me->ext_laser_pos = 0xFF;
	return ERROR_OK;
}

uint32_t experiment_task_photodiode_switchon(experiment_task_t * const me, uint32_t photo_id)
{
	if (photo_id > INTERNAL_CHAIN_CHANNEL_NUM) return ERROR_NOT_SUPPORTED;
	bsp_photodiode_sw_spi_change_mode();
	bsp_photo_switch_on(photo_id);
	me->photo_pos = photo_id;
	return ERROR_OK;
}

uint32_t experiment_task_photodiode_switchoff(experiment_task_t * const me)
{
	bsp_photodiode_sw_spi_change_mode();
	bsp_photo_switch_off_all();
	me->photo_pos = 0xFF; //photo is OFF
	return ERROR_OK;
}

uint32_t experiment_task_photo_ADC_prepare_SPI(experiment_task_t * const me)
{
	bsp_photodiode_adc_spi_change_mode();
	return ERROR_OK;
}

uint32_t experiment_task_set_pda(experiment_task_t * me,experiment_profile_t * profile)
{
	if ((profile->sampling_rate == 0 ) || (profile->sampling_rate > 850000)) return ERROR_NOT_SUPPORTED;
	if (profile->num_sample_x1024 > 2048) return ERROR_NOT_SUPPORTED;

	me->profile.sampling_rate = profile->sampling_rate;
	me->profile.pre_time = profile->pre_time;
	me->profile.experiment_time = profile->experiment_time;
	me->profile.post_time = profile->post_time;
	me->profile.num_sample_x1024 = profile->num_sample_x1024;
	me->profile.period = profile->period;
	experiment_update_profile();

	return ERROR_OK;
}

uint32_t experiment_task_set_intensity(experiment_task_t * me,experiment_profile_t * profile)
{
	if ((profile->laser_percent > 100 ) ) return ERROR_NOT_SUPPORTED;

	me->profile.laser_percent = profile->laser_percent;

	return ERROR_OK;
}

uint32_t experiment_task_set_position(experiment_task_t * me,experiment_profile_t * profile)
{
	if ((profile->pos == 0 ) || (profile->pos > 36)) return ERROR_NOT_SUPPORTED;
	me->profile.pos = profile->pos;
	return ERROR_OK;
}

uint32_t experiment_task_set_profile(experiment_task_t * me,experiment_profile_t * profile)
{
	if ((profile->sampling_rate == 0 ) || (profile->sampling_rate > 850000)) return ERROR_NOT_SUPPORTED;
	if ((profile->pos == 0 ) || (profile->pos > 36)) return ERROR_NOT_SUPPORTED;
	if ((profile->laser_percent > 100 ) ) return ERROR_NOT_SUPPORTED;
	if (profile->num_sample_x1024 > 2048) return ERROR_NOT_SUPPORTED;
	if (profile->period == 0) return ERROR_NOT_SUPPORTED;

	me->profile = *profile;
	experiment_update_profile();

	return ERROR_OK;
}

void experiment_task_get_profile(experiment_task_t * me, experiment_profile_t * profile)
{
	*profile = me->profile;
}

uint32_t experiment_start_measuring(experiment_task_t * const me)
{
	experiment_profile_t * profile = &me->profile;
	if ((profile->sampling_rate ==0 ) || (profile->sampling_rate > 850000)) return ERROR_NOT_SUPPORTED;
	if ((profile->pos ==0 ) || (profile->pos > 36)) return ERROR_NOT_SUPPORTED;
	if ((profile->laser_percent > 100 ) ) return ERROR_NOT_SUPPORTED;
	if (profile->num_sample_x1024 > 2048) return ERROR_NOT_SUPPORTED;
	SST_Task_post(&me->super, (SST_Evt *)&start_measuring_evt);
	return ERROR_OK;
}

uint32_t experiment_sample_send_to_spi(experiment_task_t * const me, uint16_t chunk_id)
{
	me->data_profile.start_address = chunk_id * EXPERIMENT_BUFFER_SAMPLE_SIZE;
	me->data_profile.num_data = EXPERIMENT_BUFFER_SAMPLE_SIZE;
	me->data_profile.destination = 1;
	SST_Task_post(&me->super, (SST_Evt *)&start_send_to_spi_evt);
	return ERROR_OK;
}

static void experiment_update_profile(void)
{
	EnableBackupRAM();
	// Magic key
	experiment_profile_bkram.magic = EXPERIMENT_VARS_MAGIC;

	// Profile
	experiment_profile_bkram.sampling_rate = experiment_task_inst.profile.sampling_rate;
	experiment_profile_bkram.pre_time = experiment_task_inst.profile.pre_time;
	experiment_profile_bkram.experiment_time = experiment_task_inst.profile.experiment_time;
	experiment_profile_bkram.post_time = experiment_task_inst.profile.post_time;

	// CRC
    uint32_t crc = 0x0000;
//    crc = CRC_HW_Calculation((uint8_t *), size);
    experiment_profile_bkram.crc = crc;

    DisableBackupRAM();
}

static bool experiment_validate_profile(void)
{
	EnableBackupRAM();
    if (experiment_profile_bkram.magic != EXPERIMENT_VARS_MAGIC)
    {
        memset(&experiment_profile_bkram, 0, sizeof(experiment_profile_bkram));
        return false;
    }
    DisableBackupRAM();
    return true;
}
