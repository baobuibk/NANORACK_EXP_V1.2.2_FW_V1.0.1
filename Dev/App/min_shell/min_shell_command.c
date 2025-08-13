/*
 * min_command.c
 *
 *  Created on: Apr 22, 2025
 *      Author: CAO HIEU
 */

#include "min_shell_command.h"
#include "lt8722.h"
#include "temperature_control.h"
#include "experiment_task.h"
#include "photodiode_cool.h"
#include "board.h"
#include "adc_monitor.h"
#include "bsp_spi_slave.h"
#include "system_reset.h"
#include "bsp_spi_ram.h"
#include "system_log.h"
#include "date_time.h"
#include "lwl.h"
#include "wdg.h"
#include "bsp_laser.h"

#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <stdio.h>

// =================================================================
// Header Define
// =================================================================


extern min_shell_task_t min_shell_task_inst;
static min_shell_task_t *p_min_shell_task = &min_shell_task_inst;

extern temperature_control_task_t temperature_control_task_inst;
static temperature_control_task_t *p_temperature_control_task = &temperature_control_task_inst;

extern experiment_task_t experiment_task_inst;
static experiment_task_t *p_experiment_task = &experiment_task_inst;

extern system_log_task_t system_log_task_inst;
static system_log_task_t *p_system_log_task = &system_log_task_inst;

// =================================================================
// Command Handlers
// =================================================================

static void MIN_Handler_PLEASE_RESET_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    MIN_Send(ctx, PLEASE_RESET_ACK, NULL, 0);
    min_shell_debug_print("RESET REQUEST:\r\n", len);
    min_shell_triger_reset(p_min_shell_task);
}

static void MIN_Handler_TEST_CONNECTION_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
#ifdef MIN_SHELL_DEBUG_PRINTING
	uint32_t var = (payload[0] << 24) | (payload[1] << 16) | (payload[2] << 8) | payload[3];
#endif
    MIN_Send(ctx, TEST_CONNECTION_ACK, payload, len);
    min_shell_debug_print("Payload TEST_CONNECTION_CMD (%d bytes): %d\r\n", len, var);
}

static void MIN_Handler_SET_WORKING_RTC_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t days  = payload[0];
	uint8_t hours = payload[1];
	uint8_t minutes  = payload[2];
	uint8_t seconds  = payload[3];

	MIN_Send(ctx, SET_WORKING_RTC_ACK, payload, len);
	// Add log
	LWL(LWL_EXP_SET_RTC, LWL_1(days), LWL_1(hours), LWL_1(minutes), LWL_1(seconds));

	date_time_set(days, hours, minutes, seconds);
	min_shell_debug_print("set time: day: %d %d:%d:%d\r\n", days, hours, minutes, seconds);
}

static void MIN_Handler_SET_NTC_CONTROL_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t buffer[1] = {MIN_ERROR_OK};
	uint8_t ntc_log_mask = payload[0];
	system_log_task_set_ntc_log_mask(p_system_log_task, ntc_log_mask);
	MIN_Send(ctx, SET_NTC_CONTROL_ACK, buffer, 1);
	min_shell_debug_print("NTC report mask: 0x%X\r\n", ntc_log_mask);
}

static void MIN_Handler_SET_TEMP_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};
    uint16_t ret = 0;
    int16_t target_temp = (payload[0] << 8) | payload[1];
    int16_t min_temp 	= (payload[2] << 8) | payload[3];
    int16_t max_temp 	= (payload[4] << 8) | payload[5];
    uint8_t pri_ntc_id  =  payload[6];
    uint8_t sec_ntc_id  =  payload[7];
    uint8_t auto_recover=  payload[8];
    uint8_t tec_pos_mask=  payload[9];			// Bit mark control
    uint8_t htr_pos_mask=  payload[10];			// Bit mark control
    uint16_t tec_mV	= (payload[11] << 8) | payload[12];
    uint8_t htr_duty	=  payload[13];

    if (pri_ntc_id > 7)
    {
    	ret++;
    	min_shell_debug_print("pri_ntc_id out off range (0-7)\r\n");
    }
    if (sec_ntc_id > 7)
    {
    	ret++;
    	min_shell_debug_print("sec_ntc_id out off range (0-7)\r\n");
    }
    if (tec_pos_mask > 0x0F)
    {
    	ret++;
    	min_shell_debug_print("tec_pos_mask is not recognized\r\n");
    }
    if (htr_pos_mask > 0x0F)
    {
    	ret++;
    	min_shell_debug_print("htr_pos_mask is not recognized\r\n");
    }

    if (!ret)
    {
    	temperature_control_set_profile(p_temperature_control_task, target_temp, min_temp, max_temp, pri_ntc_id, sec_ntc_id, auto_recover, tec_pos_mask, htr_pos_mask, tec_mV, htr_duty);
    	min_shell_debug_print("target_temp: %d\r\n", target_temp);
    	min_shell_debug_print("min_temp: %d\r\n", min_temp);
    	min_shell_debug_print("max_temp: %d\r\n", max_temp);
    	min_shell_debug_print("pri_ntc_id: %d\r\n", pri_ntc_id);
    	min_shell_debug_print("sec_ntc_id: %d\r\n", sec_ntc_id);
    	min_shell_debug_print("auto_recover: %s\r\n", auto_recover == 0? "OFF": "ON");
    	min_shell_debug_print("tec_pos_mask: 0x%02X\r\n", tec_pos_mask);
    	min_shell_debug_print("htr_pos_mask: 0x%02X\r\n", htr_pos_mask);
    	min_shell_debug_print("tec_mV: %d mV\r\n", tec_mV);
    	min_shell_debug_print("htr_duty: %d%%\r\n", htr_duty);

    	// Add log
    	LWL(LWL_EXP_TEMP_PROFILE_SET, LWL_2(target_temp), LWL_2(min_temp), LWL_2(max_temp),
    	    LWL_1(pri_ntc_id), LWL_1(sec_ntc_id), LWL_1(auto_recover),
    	    LWL_1(tec_pos_mask), LWL_1(htr_pos_mask), LWL_2(tec_mV), LWL_1(htr_duty));
    }
    else
    {
    	buffer[0] = MIN_RESP_FAIL;
    	buffer[1] = MIN_RESP_FAIL;
    }
    MIN_Send(ctx, SET_TEMP_PROFILE_ACK, buffer, 2);
    min_shell_debug_print("SET_TEMP_PROFILE_ACK\r\n");
}

static void MIN_Handler_START_TEMP_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t reserved = 0xFF;
    min_shell_debug_print("Start temperature control auto\r\n");

    // Add log
    LWL(LWL_EXP_TEMP_AUTO_MODE);

    temperature_control_auto_mode_set(p_temperature_control_task);
    MIN_Send(ctx, START_TEMP_PROFILE_ACK, &reserved, 1);
}

static void MIN_Handler_STOP_TEMP_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t reserved = 0xFF;
    min_shell_debug_print("Stop temperature control auto\r\n");

    // Add log
    LWL(LWL_EXP_TEMP_MANUAL_MODE);

    temperature_control_man_mode_set(p_temperature_control_task);
    MIN_Send(ctx, STOP_TEMP_PROFILE_ACK, &reserved, 1);
}

static void MIN_Handler_SET_OVERRIDE_TEC_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};
	uint8_t ret = 0;
	uint16_t interval = ((payload[0] << 8) | payload[1]) * 1000;
	uint8_t over_tec_id = payload[2];
	uint16_t over_TEC_mV = (payload[3] << 8) | payload[4];

	if (interval > 60000)
	{
		ret++;
		min_shell_debug_print("The time interval is greater than 1 minute\r\n");
	}

    if (over_tec_id > 4)
    {
        ret++;
        min_shell_debug_print("tec index out of range (0-4)\r\n");
    }
    else if (over_tec_id == 4)
    {
        MIN_Send(ctx, SET_OVERRIDE_TEC_PROFILE_ACK, buffer, 2);
        temperature_profile_tec_ovr_register(p_temperature_control_task, 4);
    	min_shell_debug_print("Unregister TEC for override mode\r\n");
        min_shell_debug_print("SET_OVERRIDE_TEC_PROFILE_ACK\r\n");
        return;
    }
    else
    {
    	uint8_t tec_profile = temperature_control_profile_tec_get(p_temperature_control_task);
        if (tec_profile & (1 << over_tec_id))
        {
        	ret++;
            min_shell_debug_print("tec[%d] is UNAVAILABLE !!\r\n", over_tec_id);
        }
    }

    if ((over_TEC_mV < 500) || (over_TEC_mV > 3000))
    {
        ret++;
        min_shell_debug_print("tec voltage out of range (500-3000)mV\r\n", over_TEC_mV);
    }

	if (!ret)
    {
        tec_over_set_interval(interval);
        temperature_profile_tec_ovr_register(p_temperature_control_task, over_tec_id);
        temperature_profile_tec_ovr_voltage_set(p_temperature_control_task, over_TEC_mV);

        // Add log
        LWL(LWL_EXP_TEMP_TEC_OVERRIDE_PROFILE, LWL_2(interval), LWL_1(over_tec_id), LWL_2(over_TEC_mV));
    }
    else
    {
        buffer[0] = MIN_RESP_FAIL;
        buffer[1] = MIN_RESP_FAIL;
    }

    MIN_Send(ctx, SET_OVERRIDE_TEC_PROFILE_ACK, buffer, 2);
    min_shell_debug_print("SET_OVERRIDE_TEC_PROFILE_ACK\r\n");
}

static void MIN_Handler_START_OVERRIDE_TEC_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t reserved = 0xFF;
    MIN_Send(ctx, START_OVERRIDE_TEC_PROFILE_ACK, &reserved, 1);

    // Add log
    LWL(LWL_EXP_TEMP_TEC_OVERRIDE_ON);

    min_shell_debug_print("Min start tec override\r\n");
    tec_ovr_start();
}

static void MIN_Handler_STOP_OVERRIDE_TEC_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t reserved = 0xFF;
    MIN_Send(ctx, STOP_OVERRIDE_TEC_PROFILE_ACK, &reserved, 1);

    // Add log
	LWL(LWL_EXP_TEMP_TEC_OVERRIDE_OFF);

    min_shell_debug_print("Min stop tec override\r\n");
    tec_ovr_stop();
}

static void MIN_Handler_SET_PDA_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    min_shell_debug_print("Min set PDA profile\r\n");
    uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};

    uint16_t ret = 0;
    uint32_t sample_rate = (uint32_t)((uint32_t)payload[0] << 24) | ((uint32_t)payload[1] << 16) | ((uint32_t)payload[2] << 8) | payload[3];
    uint16_t pre_time  = (uint16_t)((uint16_t)payload[4] << 8) | payload[5];
    uint16_t sample_time = (uint16_t)((uint16_t)payload[6] << 8) | payload[7];
    uint16_t post_time = (uint16_t)((uint16_t)payload[8] << 8) | payload[9];

    if ((sample_rate < 1000) || (sample_rate > 1000000))
    {
        ret++;
        min_shell_debug_print("sampling rate out of range (1K-1000K)\r\n");
    }

    if (pre_time == 0)
    {
        ret++;
        min_shell_debug_print("pre_time should be larger than 0\r\n");
    }

    if (sample_time == 0)
    {
        ret++;
        min_shell_debug_print("sample time should be larger than 0\r\n");
    }

    if (post_time == 0)
    {
        ret++;
        min_shell_debug_print("post_time should be larger than 0\r\n");
    }

    uint32_t num_sample_x1024 = ((pre_time + sample_time + post_time) * sample_rate) /1024000;
    if (num_sample_x1024 > 2048) // larrger than 4MB
    {
        ret++;
        min_shell_debug_print("total sample must be less than 2048K \r\n");
    }

    if (!ret)
    {
        experiment_profile_t profile;
        profile.sampling_rate = sample_rate;    	// Hz	// (Sample/second)
        profile.pre_time = pre_time;          		// mS
        profile.experiment_time = sample_time;  	// mS
        profile.post_time = post_time;        		// mS
        profile.num_sample_x1024 = num_sample_x1024;// kSample
        profile.period = 1000000 / sample_rate; 	// uS
        if (!experiment_task_set_pda(p_experiment_task, &profile))
        {
        	// Add log
        	LWL(LWL_EXP_SET_PHOTO_PROFILE, LWL_4(sample_rate), LWL_2(pre_time), LWL_2(sample_time), LWL_2(post_time));

            min_shell_debug_print("Min set PDA DONE\r\n");
        }
        else
        {
            buffer[0] = MIN_RESP_FAIL;
            buffer[1] = MIN_RESP_FAIL;
            min_shell_debug_print("Min set PDA ERROR\r\n");
        }
    }
    else
    {
        buffer[0] = MIN_RESP_FAIL;
        buffer[1] = MIN_RESP_FAIL;
    }
    MIN_Send(ctx, SET_PDA_PROFILE_ACK, buffer, 2);
}

static void MIN_Handler_SET_LASER_INTENSITY_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};
    uint8_t percent = payload[0];
    if (percent > 100)
    {
        buffer[0] = MIN_RESP_FAIL;
        buffer[1] = MIN_RESP_FAIL;

        min_shell_debug_print("argument 1 out of range (0-100)\r\n");
        return;
    }
    else
    {
        experiment_profile_t profile;
        experiment_task_get_profile(p_experiment_task, &profile);

        profile.laser_percent = percent;
        if (!experiment_task_set_intensity(p_experiment_task, &profile))
        {
        	// Add log
        	LWL(LWL_EXP_SET_LASER_INTENSITY, LWL_1(percent));

            min_shell_debug_print("Min SET_LASER_INTENSITY DONE\r\n");
        }
        else
        {
            buffer[0] = MIN_RESP_FAIL;
            buffer[1] = MIN_RESP_FAIL;
            min_shell_debug_print("Min SET_LASER_INTENSITY ERROR\r\n");
        }
    }
    MIN_Send(ctx, SET_LASER_INTENSITY_ACK, buffer, 2);
}

static void MIN_Handler_SET_POSITION_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};
    uint8_t laser_idx = payload[0];
    if ((laser_idx > INTERNAL_CHAIN_CHANNEL_NUM) || (laser_idx < 1))
    {
        buffer[0] = MIN_RESP_FAIL;
        buffer[1] = MIN_RESP_FAIL;
        min_shell_debug_print("argument 1 out of range,(1-36)\r\n");
    }
    else
    {
        experiment_profile_t profile;
        experiment_task_get_profile(p_experiment_task, &profile);

        profile.pos = laser_idx;
        if (!experiment_task_set_position(p_experiment_task, &profile))
        {
        	// Add log
        	LWL(LWL_EXP_SET_LASER_PHOTO_INDEX, LWL_1(laser_idx));

            min_shell_debug_print("Min SET_POSITION DONE\r\n");
        }
        else
        {
            buffer[0] = MIN_RESP_FAIL;
            buffer[1] = MIN_RESP_FAIL;
            min_shell_debug_print("Min SET_POSITION ERROR\r\n");
        }
    }
    MIN_Send(ctx, SET_POSITION_ACK, buffer, 2);
}

static void MIN_Handler_START_SAMPLE_CYCLE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[1] = {MIN_RESP_OK};
	if (experiment_start_measuring(p_experiment_task))
    {
        buffer[0] = MIN_RESP_FAIL;
        min_shell_debug_print("Wrong profile, please check\r\n");
    }
    else
    {
    	min_shell_busy_set(p_min_shell_task);
        min_shell_debug_print("Stop Min\r\nStart sampling...\r\n");
    }

    MIN_Send(ctx, START_SAMPLE_CYCLE_ACK, buffer, 1);
}

static void MIN_Handler_GET_INFO_SAMPLE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[2];
    experiment_profile_t profile;
    experiment_task_get_profile(p_experiment_task, &profile);
    if (profile.num_sample_x1024 == 0)
    {
        buffer[0] = 0;
        buffer[1] = 0;
        min_shell_debug_print("Sample store empty\r\n");
    }
    else
    {
        uint16_t total_chunks = (profile.num_sample_x1024 / EXPERIMENT_CHUNK_SAMPLE_SIZE);
        if (profile.num_sample_x1024 % EXPERIMENT_CHUNK_SAMPLE_SIZE)
        	total_chunks += 1;
        min_shell_debug_print("Total sample chunks: %d\r\n", total_chunks);
        buffer[0] = (uint8_t)(total_chunks >> 8) 	& 0xFF;
        buffer[1] = (uint8_t)(total_chunks			& 0xFF);
    }
    MIN_Send(ctx, GET_INFO_SAMPLE_ACK, buffer, 2);
}

static void MIN_Handler_GET_CHUNK_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};
    uint8_t chunk_id = payload[0];
    experiment_profile_t profile;
    experiment_task_get_profile(p_experiment_task, &profile);
    uint16_t total_chunks = (profile.num_sample_x1024 / EXPERIMENT_CHUNK_SAMPLE_SIZE);
    if (profile.num_sample_x1024 % EXPERIMENT_CHUNK_SAMPLE_SIZE)
    	total_chunks += 1;
    if (total_chunks == 0)
    {
        buffer[0] = MIN_RESP_FAIL;
        buffer[1] = MIN_RESP_FAIL;
        min_shell_debug_print("FRAM is empty\r\n");
    }
    else if (chunk_id >= total_chunks)
    {
        buffer[0] = MIN_RESP_FAIL;
        buffer[1] = MIN_RESP_FAIL;
        min_shell_debug_print("Chunk index out of range\r\n");
    }
    else
    {
    	// Add log
    	LWL(LWL_EXP_TRANS_PHOTO_DATA, LWL_1(chunk_id));

    	experiment_sample_send_to_spi(p_experiment_task, chunk_id);
        min_shell_debug_print("Already prepare sample chunk %d\r\n", chunk_id);
    }
    MIN_Send(ctx, GET_CHUNK_ACK, buffer, 2);
}

static void MIN_Handler_GET_CHUNK_CRC_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[4];
    uint32_t crc = SPI_SlaveDevide_GetDataCRC();
    buffer[0] = (crc >> 24) & 0xFF;
    buffer[1] = (crc >> 16) & 0xFF;
    buffer[2] = (crc >> 8) & 0xFF;
    buffer[3] = crc & 0xFF;
    MIN_Send(ctx, GET_CHUNK_CRC_ACK, buffer, 4);

    min_shell_debug_print("Chunk index: %d - CRC: 0x%08X\r\n", payload[0], crc);
}

static void MIN_Handler_GET_LASER_CURRENT_DATA_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};

    bsp_laser_send_current_to_spi();

    min_shell_debug_print("Already prepare current chunk\r\n");
    MIN_Send(ctx, GET_LASER_CURRENT_DATA_ACK, buffer, 2);
}

static void MIN_Handler_GET_LASER_CURRENT_CRC_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[4];
    uint32_t crc = SPI_SlaveDevide_GetDataCRC();
    buffer[0] = (crc >> 24) & 0xFF;
    buffer[1] = (crc >> 16) & 0xFF;
    buffer[2] = (crc >> 8) & 0xFF;
    buffer[3] = crc & 0xFF;
    MIN_Send(ctx, GET_LASER_CURRENT_CRC_ACK, buffer, 4);

    min_shell_debug_print("Current chunk CRC: 0x%08X\r\n", payload[0], crc);
}

static void MIN_Handler_MANUAL_SET_LASER_INTENSITY_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};
	int8_t obj = payload[0];		// int/ext
	int16_t percent = payload[1];
	if (percent > 100 || obj >= 2)
	{
		buffer[0] = MIN_RESP_FAIL;
		buffer[1] = MIN_RESP_FAIL;
		min_shell_debug_print("Wrong input\r\n");
	}
	else
	{
		experiment_task_laser_set_current(p_experiment_task, obj, percent);
		min_shell_debug_print("_ACK: %d\r\n", percent);
	}

	MIN_Send(ctx, MANUAL_SET_LASER_INTENSITY_ACK, buffer, 2);
}

static void MIN_Handler_MANUAL_TURN_ON_LASER_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t buffer[2] = {MIN_RESP_OK, MIN_ERROR_OK};
	uint8_t obj = payload[0];
	uint8_t laser_idx = payload[1];
	if (((obj == 0) && ((laser_idx > INTERNAL_CHAIN_CHANNEL_NUM) || (laser_idx < 1)))
			|| ((obj == 1) && (laser_idx < 1)))
	{
		buffer[0] = MIN_RESP_FAIL;
		buffer[1] = MIN_RESP_FAIL;
		min_shell_debug_print("Wrong input\r\n");
	}
	else
	{
		if (obj == 0)
			experiment_task_int_laser_switchon(p_experiment_task, laser_idx);
		else if (obj == 1)
//			experiment_task_ext_laser_switchon(p_experiment_task, laser_idx);
			bsp_laser_ext_switch_on_mask(laser_idx);
		min_shell_debug_print("_ACK\r\n");
	}

	MIN_Send(ctx, MANUAL_TURN_ON_LASER_ACK, buffer, 2);
}

static void MIN_Handler_MANUAL_TURN_OFF_LASER_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t buffer[1] = {MIN_ERROR_OK};
	uint8_t obj = payload[0];
	MIN_Send(ctx, MANUAL_TURN_OFF_LASER_ACK, buffer, 1);
	if (obj == 0)
		experiment_task_int_laser_switchoff(p_experiment_task);
	else if (obj == 1)
		experiment_task_ext_laser_switchoff(p_experiment_task);
	min_shell_debug_print("TURN_OFF_EXT_LASER_ACK\r\n");
}

static void MIN_Handler_GET_LASER_CURRENT_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint16_t int_current = bsp_laser_get_int_current();
	uint16_t ext_current = bsp_laser_get_ext_current();

	uint8_t buffer[5] = {MIN_RESP_OK, (int_current >> 8) & 0xFF, int_current & 0xFF, (ext_current >> 8) & 0xFF, ext_current & 0xFF};
	MIN_Send(ctx, GET_LASER_CURRENT_ACK, buffer, 5);
	min_shell_debug_print("int_laser_current: %d mA\r\n", int_current);
	min_shell_debug_print("ext_laser_current: %d mA\r\n", ext_current);
	return;
}

static void MIN_Handler_GET_LOG_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    uint8_t buffer[1] = {MIN_ERROR_OK};
    lwl_log_send_to_spi();
    min_shell_debug_print("Already prepare log chunk\r\n");
    MIN_Send(ctx, GET_LOG_ACK, buffer, 1);
}

// =================================================================
// Command Table
// =================================================================

static const MIN_Command_t command_table[] = {
    {PLEASE_RESET_CMD, MIN_Handler_PLEASE_RESET_CMD},
    {TEST_CONNECTION_CMD, MIN_Handler_TEST_CONNECTION_CMD},
    {SET_WORKING_RTC_CMD, MIN_Handler_SET_WORKING_RTC_CMD},
//    {REVERSE_6, MIN_Handler_REVERSE_6 },
    {SET_NTC_CONTROL_CMD, MIN_Handler_SET_NTC_CONTROL_CMD},

    {SET_TEMP_PROFILE_CMD, MIN_Handler_SET_TEMP_PROFILE_CMD},
    {START_TEMP_PROFILE_CMD, MIN_Handler_START_TEMP_PROFILE_CMD},
    {STOP_TEMP_PROFILE_CMD, MIN_Handler_STOP_TEMP_PROFILE_CMD},

    {SET_OVERRIDE_TEC_PROFILE_CMD, MIN_Handler_SET_OVERRIDE_TEC_PROFILE_CMD},
    {START_OVERRIDE_TEC_PROFILE_CMD, MIN_Handler_START_OVERRIDE_TEC_PROFILE_CMD},
    {STOP_OVERRIDE_TEC_PROFILE_CMD, MIN_Handler_STOP_OVERRIDE_TEC_PROFILE_CMD},

    {SET_PDA_PROFILE_CMD, MIN_Handler_SET_PDA_PROFILE_CMD},
    {SET_LASER_INTENSITY_CMD, MIN_Handler_SET_LASER_INTENSITY_CMD},
    {SET_POSITION_CMD, MIN_Handler_SET_POSITION_CMD},

    {START_SAMPLE_CYCLE_CMD, MIN_Handler_START_SAMPLE_CYCLE_CMD},
    {GET_INFO_SAMPLE_CMD, MIN_Handler_GET_INFO_SAMPLE_CMD},
    {GET_CHUNK_CMD, MIN_Handler_GET_CHUNK_CMD},
    {GET_CHUNK_CRC_CMD, MIN_Handler_GET_CHUNK_CRC_CMD},

    {GET_LASER_CURRENT_DATA_CMD, MIN_Handler_GET_LASER_CURRENT_DATA_CMD},
    {GET_LASER_CURRENT_CRC_CMD, MIN_Handler_GET_LASER_CURRENT_CRC_CMD},

    {MANUAL_SET_LASER_INTENSITY_CMD, MIN_Handler_MANUAL_SET_LASER_INTENSITY_CMD},
    {MANUAL_TURN_ON_LASER_CMD, MIN_Handler_MANUAL_TURN_ON_LASER_CMD},
    {MANUAL_TURN_OFF_LASER_CMD, MIN_Handler_MANUAL_TURN_OFF_LASER_CMD},

	{GET_LOG_CMD, MIN_Handler_GET_LOG_CMD},

    {GET_LASER_CURRENT_CMD, MIN_Handler_GET_LASER_CURRENT_CMD},

};

static const int command_table_size = sizeof(command_table) / sizeof(command_table[0]);

// =================================================================
// Helper Functions
// =================================================================

const MIN_Command_t *MIN_GetCommandTable(void)
{
    return command_table;
}

int MIN_GetCommandTableSize(void)
{
    return command_table_size;
}
