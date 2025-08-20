/*
 * cli_command.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#include "cli_command.h"
#include "ntc.h"
#include "lt8722.h"
#include "temperature_control.h"
#include "experiment_task.h"
#include "photodiode_cool.h"
#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "system_reset.h"
#include "bsp_spi_ram.h"
#include "bsp_laser.h"
#include "bsp_ntc.h"
#include "min_shell.h"

extern temperature_control_task_t temperature_control_task_inst;
static temperature_control_task_t *p_temperature_control_task = &temperature_control_task_inst;

extern experiment_task_t experiment_task_inst;
static experiment_task_t *p_experiment_task = &experiment_task_inst;

extern min_shell_task_t min_shell_task_inst;
static min_shell_task_t *p_min_shell_task = &min_shell_task_inst;

static bool is_measured = false;

/*************************************************
 *                Private variable                 *
 *************************************************/


/*************************************************
 *                Command Define                 *
 *************************************************/


/*************************************************
 *                Command Define                 *
 *************************************************/

static void CMD_Clear_CLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_Reset(EmbeddedCli *cli, char *args, void *context);

static void CMD_NTC_Get_Temp(EmbeddedCli *cli, char *args, void *context);
static void CMD_PWR_5V_Set(EmbeddedCli *cli, char *args, void *context);
static void CMD_PWR_5V_Get(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Init(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Set_Profile_Volt(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Get_Profile_Volt(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Profile_Register(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Profile_Get(EmbeddedCli *cli, char *args, void *context);

static void CMD_TEC_OVR_Set_Output(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_OVR_Start(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_OVR_Stop(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_OVR_Get(EmbeddedCli *cli, char *args, void *context);

static void CMD_TEC_Man_Set_Volt(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Man_Set_Output(EmbeddedCli *cli, char *args, void *context);
static void CMD_HTR_Set_Profile_Duty(EmbeddedCli *cli, char *args, void *context);
static void CMD_HTR_Get_Profile_Duty(EmbeddedCli *cli, char *args, void *context);
static void CMD_Heater_Profile_Register(EmbeddedCli *cli, char *args, void *context);
static void CMD_Heater_Profile_Get(EmbeddedCli *cli, char *args, void *context);

static void CMD_Ref_Get_Temp_Limit(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Set_Temp_Limit(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Set_Temp(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Get_Temp(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Set_NTC(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Get_NTC(EmbeddedCli *cli, char *args, void *context);
static void CMD_Get_Auto_Recover(EmbeddedCli *cli, char *args, void *context);
static void CMD_Set_Auto_Recover(EmbeddedCli *cli, char *args, void *context);
static void CMD_Start_Auto_Mode(EmbeddedCli *cli, char *args, void *context);
static void CMD_Stop_Auto_Mode(EmbeddedCli *cli, char *args, void *context);

static void CMD_Set_Laser_Int_Current(EmbeddedCli *cli, char *args, void *context);
static void CMD_Set_Laser_Ext_Current(EmbeddedCli *cli, char *args, void *context);
static void CMD_Laser_Get_Current(EmbeddedCli *cli, char *args, void *context);
static void CMD_Int_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ext_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context);
static void CMD_Int_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ext_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context);

static void cmd_exp_set_profile(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_get_profile(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_start_measuring(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_ram_read(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_data_transfer(EmbeddedCli *cli, char *args, void *context);

//static void CMD_TEC_Set_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_TEC_Get_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_HTR_Set_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_HTR_Get_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Temp_Set_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Temp_Get_Auto(EmbeddedCli *cli, char *args, void *context);
//
//static void CMD_Sens_List(EmbeddedCli *cli, char *args, void *context);
//static void CMD_LSMSens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_H3LSens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_BMESens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_H250Sens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_K33Sens_Get(EmbeddedCli *cli, char *args, void *context);
//
//// Laser Photo
//static void CMD_Set_Laser(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Get_Current(EmbeddedCli *cli, char *args, void *context);
//static void CMD_PD_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Set_PD(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Set_Rate(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Trig(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Status_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get_Char(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get_Buf(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get_Buf_Char(EmbeddedCli *cli, char *args, void *context);

static void CMD_test(EmbeddedCli *cli, char *args, void *context)
{
//	system_reset(system_reset_task);
	haha();

}



static void cmd_clear_fram(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t buffer[1024] = {0};
	bsp_spi_ram_write_polling(0, 1024, buffer);

	cli_printf(cli, "FRAM is empty\r\n");
	cli_printf(cli, "");
}

static void cmd_read_fram(EmbeddedCli *cli, char *args, void *context)
{
    uint8_t buffer[1024];
    bsp_spi_ram_read_polling(0, 1024, buffer);

    cli_printf(cli, "Buffer data in hex:\r\n");
    for (int i = 0; i < 1024; i++) {
        cli_printf(cli, "%02X ", buffer[i]);
        if ((i + 1) % 16 == 0) {
            cli_printf(cli, "\r\n");
        }
    }

    cli_printf(cli, "\r\n");
}





/*************************************************
 *                 Command  Array                *
 *************************************************/
// Guide: Command bindings are declared in the following order:
// { category, name, help, tokenizeArgs, context, binding }
// - category: Command group; set to NULL if grouping is not needed.
// - name: Command name (required)
// - help: Help string describing the command (required)
// - tokenizeArgs: Set to true to automatically split arguments when the command is called.
// - context: Pointer to a command-specific context; can be NULL.
// - binding: Callback function that handles the command.

static const CliCommandBinding cliStaticBindings_internal[] = {
    // Common
    { "Ultis", "help",         "Print list of all available CLI commands [Firmware: 1]", false,  NULL, CMD_Help },
    { "Ultis", "cls",          "Clear the console output screen",                        false,  NULL, CMD_Clear_CLI },
    { "Ultis", "reset",        "Perform MCU software reset",                             false,  NULL, CMD_Reset },
    // NTC
    { "NTC",   "ntc_get_temp", "Read temperature value from NTC sensor [ch: 0-7, a=all]", true,   NULL, CMD_NTC_Get_Temp },

    // Power
    { "PWR",   "pwr_5v_set",   "Turn ON/OFF 5V power supply [0:OFF / 1:ON]",             true,   NULL, CMD_PWR_5V_Set },
    { "PWR",   "pwr_5v_get",   "Read current state of 5V power supply",                  false,  NULL, CMD_PWR_5V_Get },

    // TEC
    { "TEC",   "tec_init",     "Initialize TEC driver for specified channel [0-3, a=all]", true,  NULL, CMD_TEC_Init },
    { "TEC",   "tec_profile_volt_set", "Set profile voltage for TEC channel: tec_volt [500~2500mV]", true,  NULL, CMD_TEC_Set_Profile_Volt },
    { "TEC",   "tec_profile_volt_get", "Get current profile voltage of TEC [0-3, a=all]",         true,  NULL, CMD_TEC_Get_Profile_Volt },
    { "TEC",   "tec_profile_set", "register Tec for profile: 0/1 0/1 0/1 0/1 ]", true,  NULL, CMD_TEC_Profile_Register},
    { "TEC",   "tec_profile_get", "get the tec profile ]", true,  NULL, CMD_TEC_Profile_Get},
	{ "TEC",   "tec_man_volt_set",  "Set TEC output : tec_idx C/H voltage",        true,  NULL, CMD_TEC_Man_Set_Volt },
	{ "TEC",   "tec_man_output_set",  "Set TEC output : tec_idx EN/DIS",        true,  NULL, CMD_TEC_Man_Set_Output },

	// TEC override
	{ "TEC",   "tec_ovr_set",  "Set TEC_override output with format: tec_ovr_set [interval_mS] [id] [volt_mV]",        true,  NULL, CMD_TEC_OVR_Set_Output },
	{ "TEC",   "tec_ovr_start",  "Set TEC_override enable",        true,  NULL, CMD_TEC_OVR_Start },
	{ "TEC",   "tec_ovr_stop",  "Set TEC_override disable",        true,  NULL, CMD_TEC_OVR_Stop },
	{ "TEC",   "tec_ovr_get",  "Get TEC_override status",        true,  NULL, CMD_TEC_OVR_Get },

	// Heater
    { "Heater", "htr_profile_duty_set","Set duty cycle for heater [0-3]: [0~100%]",              true,  NULL, CMD_HTR_Set_Profile_Duty },
    { "Heater", "htr_profile_duty_get","Read current duty cycle setting of heater [0-3, a=all]", true,  NULL, CMD_HTR_Get_Profile_Duty },
    { "Heater", "htr_profile_set", "register heater for profile: 0/1 0/1 0/1 0/1 ]", true,  NULL, CMD_Heater_Profile_Register},
	{ "Heater", "htr_profile_get", "get the heater profile ]", true,  NULL, CMD_Heater_Profile_Get},

	// Reference temperature
    { "Ref",   "ref_temp_set", "Set reference temperature for control logic (°C)",       true,  NULL, CMD_Ref_Set_Temp },
    { "Ref",   "ref_temp_get", "Read current reference temperature setting",              true,  NULL, CMD_Ref_Get_Temp },
    { "Ref",   "ref_ntc_set",  "Select NTC channel used for temperature feedback [pri_idx] [sec_idx]",  true,  NULL, CMD_Ref_Set_NTC },
    { "Ref",   "ref_ntc_get",  "Get currently selected NTC channel for control",          true,  NULL, CMD_Ref_Get_NTC },
	{ "Ref",   "ref_temp_lim_set",  "Set min and max temp for auto mode [max] [min]",          true,  NULL, CMD_Ref_Set_Temp_Limit },
	{ "Ref",   "ref_temp_lim_get",  "Get min and max temp for auto mode",          true,  NULL, CMD_Ref_Get_Temp_Limit },

	// Automatic control
    { "Auto",  "auto_control_start", "start auto control mode",      true,  NULL, CMD_Start_Auto_Mode },
    { "Auto",  "auto_control_stop", "stop auto control mode",      true,  NULL, CMD_Stop_Auto_Mode },
	{ "Auto",  "set_auto_recover", "set auto recover [0/1]",      true,  NULL, CMD_Set_Auto_Recover },
	{ "Auto",  "get_auto_recover", "get auto recover ",      true,  NULL, CMD_Get_Auto_Recover },

	// Laser Photo
	{ "Laser", "laser_int_set_current",    "format: laser_int_set_current [percent]",  true, NULL, CMD_Set_Laser_Int_Current },
	{ "Laser", "laser_ext_set_current",    "format: laser_ext_set_current  [percent]",  true, NULL, CMD_Set_Laser_Ext_Current },
	{ "Laser", "laser_get_current",    "format: laser_get_current [0/1]",  true, NULL, CMD_Laser_Get_Current },
	{ "Laser", "laser_int_switch_on",    "format: laser_int_switch_on [pos]",  true, NULL, CMD_Int_Laser_Switch_On },
	{ "Laser", "laser_ext_switch_on",    "format: laser_ext_switch_on [pos]",  true, NULL, CMD_Ext_Laser_Switch_On },
	{ "Laser", "laser_int_switch_off",    "format: laser_int_switch_off",  true, NULL, CMD_Int_Laser_Switch_Off },
	{ "Laser", "laser_ext_switch_off",    "format: laser_ext_switch_off",  true, NULL, CMD_Ext_Laser_Switch_Off },


	{ "Experiment", "exp_set_profile",    "format: exp_set_profile sampling_rate pos laser_percent pre_time experiment_time post_time",  true, NULL, cmd_exp_set_profile },
	{ "Experiment", "exp_get_profile",    "format: exp_get_profile",  true, NULL, cmd_exp_get_profile },
	{ "Experiment", "exp_start_measuring",    "format: exp_start_measuring",  true, NULL, cmd_exp_start_measuring },
	{ "Experiment", "exp_ram_read",    "format: exp_ram_read [address] [num_sample] [mode]",  true, NULL, cmd_exp_ram_read },
	{ "Experiment", "exp_data_transfer",    "format: exp_data_transfer",  true, NULL, cmd_exp_data_transfer },


	{ "TEST", "clear_fram",    "",  true, NULL, cmd_clear_fram },
	{ "TEST", "read_fram",    "",  true, NULL, cmd_read_fram },
	{ "TEST", "test",    "",  true, NULL, CMD_test },

//		{ NULL, "get_current",  "format: get_current [int/ext]",                                   true, NULL, CMD_Get_Current },
//	    { NULL, "pd_get",       "format: pd_get [pd_index]",                                       true, NULL, CMD_PD_Get },
//	    { NULL, "sp_set_pd",    "format: sp_set_pd [photo_index]",                                 true, NULL, CMD_Sample_Set_PD },
//	    { NULL, "sp_set_rate",  "format: sp_set_rate [sampling_rate] [num_samples]",               true, NULL, CMD_Sample_Set_Rate },
//	    { NULL, "sp_trig",      "format: sp_trig",                                                 true, NULL, CMD_Sample_Trig },
//	    { NULL, "sp_status",    "format: sp_status",                                               true, NULL, CMD_Sample_Status_Get },

//    { "Auto",  "auto_tec_set", "Enable/disable TECs for auto control [tecX_en 0/1]",      true,  NULL, CMD_TEC_Set_Auto },
//    { "Auto",  "auto_tec_get", "List enabled TEC channels for auto control [0-3, a]",     true,  NULL, CMD_TEC_Get_Auto },
//    { "Auto",  "auto_htr_set", "Enable/disable heaters for auto control [htrX_en 0/1]",   true,  NULL, CMD_HTR_Set_Auto },
//    { "Auto",  "auto_htr_get", "List enabled heater channels for auto control [0-3, a]",  true,  NULL, CMD_HTR_Get_Auto },
//    { "Auto",  "auto_temp_set","Enable/disable full temperature control routine [0/1]",   true,  NULL, CMD_Temp_Set_Auto },
//    { "Auto",  "auto_temp_get","Read current state of temperature control routine",       true,  NULL, CMD_Temp_Get_Auto },


//    { "TEC",   "tec_dir_get",  "Read direction setting of TEC [0-3, a=all]",              true,  NULL, CMD_TEC_Get_Dir },
//    // Sensors
//    { "Sensor","sens_list",    "List all available connected sensors",                    true,  NULL, CMD_Sens_List },
//    { "Sensor","lsm_sens_get", "Read LSM6DSOX data [0=all, 1=acc, 2=gyro]",               true,  NULL, CMD_LSMSens_Get },
//    { "Sensor","h3l_sens_get", "Read high-g acceleration data from H3LIS331 sensor",      true,  NULL, CMD_H3LSens_Get },
//    { "Sensor","bme_sens_get", "Read BME280 sensor [0=all, 1=temp, 2=RH, 3=pressure]",     true,  NULL, CMD_BMESens_Get },
//    { "Sensor","h250_sens_get","Read CO₂ concentration from H-250(G)-3V sensor",          false, NULL, CMD_H250Sens_Get },
//    { "Sensor","k33_sens_get", "Read CO₂ concentration from K33 ICB-F 10% sensor",        false, NULL, CMD_K33Sens_Get },
//
//	// Laser Photo
//    { NULL, "set_laser",    "format: set_laser [int/ext] [laser_index] [dac_val]",             true, NULL, CMD_Set_Laser },
//    { NULL, "get_current",  "format: get_current [int/ext]",                                   true, NULL, CMD_Get_Current },
//    { NULL, "pd_get",       "format: pd_get [pd_index]",                                       true, NULL, CMD_PD_Get },
//    { NULL, "sp_set_pd",    "format: sp_set_pd [photo_index]",                                 true, NULL, CMD_Sample_Set_PD },
//    { NULL, "sp_set_rate",  "format: sp_set_rate [sampling_rate] [num_samples]",               true, NULL, CMD_Sample_Set_Rate },
//    { NULL, "sp_trig",      "format: sp_trig",                                                 true, NULL, CMD_Sample_Trig },
//    { NULL, "sp_status",    "format: sp_status",                                               true, NULL, CMD_Sample_Status_Get },
//    { NULL, "sp_get",       "format: sp_get [num_samples]",                                    true, NULL, CMD_Sample_Get },
//    { NULL, "sp_get_c",     "format: sp_get_c [num_samples]",                                  true, NULL, CMD_Sample_Get_Char },
//    { NULL, "sp_get_buf",   "format: sp_get_buf",                                              true, NULL, CMD_Sample_Get_Buf },
//    { NULL, "sp_get_buf_c", "format: sp_get_buf_c",                                            true, NULL, CMD_Sample_Get_Buf_Char },
};

/*************************************************
 *             Command List Function             *
 *************************************************/


static void CMD_Clear_CLI(EmbeddedCli *cli, char *args, void *context) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "\33[2J");
    embeddedCliPrint(cli, buffer);
}

static void CMD_Reset(EmbeddedCli *cli, char *args, void *context) {
	NVIC_SystemReset();
    embeddedCliPrint(cli, "");
}

static void CMD_NTC_Get_Temp(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t channel = 0;
	int16_t temp = 0;
	uint8_t tokenCount = embeddedCliGetTokenCount(args);
		if (tokenCount != 1)
		{
			cli_printf(cli, "command require one argument\r\n");
			return;
		}
	const char *arg1 = embeddedCliGetToken(args, 1);
	if (*arg1 == 'a')
	{

		for (uint8_t channel = 0; channel < 8; channel++)
		{
//			temp = temperature_monitor_get_ntc_temperature(channel);
			temp = bsp_ntc_get_temperature(channel);
			if (temp == (int16_t)0x8000)	cli_printf(cli, "ntc[%d] = ERROR\r\n",channel);
			else cli_printf(cli, "ntc[%d] = %d C\r\n",channel,temp);
		}
	}
	else if ((*arg1 >= '0') &&(*arg1 <=  '7'))
	{
		channel = atoi(arg1);
//		temp = temperature_monitor_get_ntc_temperature(channel);
		temp = bsp_ntc_get_temperature(channel);
		if (temp == (int16_t)0x8000)	cli_printf(cli, "ntc[%d] = ERROR\r\n",channel);
		else cli_printf(cli, "ntc[%d] = %d C\r\n",channel,temp);
	}
	else cli_printf(cli, "Wrong arguments\r\n");

}

static void CMD_PWR_5V_Set(EmbeddedCli *cli, char *args, void *context) {
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 1)
	{
		cli_printf(cli, "command require one argument\r\n");
		return;
	}
	if (!temperature_control_is_in_man_state(p_temperature_control_task))
	{
		cli_printf(cli, "System is in auto mode, change to manual mode before controlling power\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t status = atoi(arg1);

	temperature_control_power_control(p_temperature_control_task , status);

	cli_printf(cli,"OK\r\n");
}

static void CMD_PWR_5V_Get(EmbeddedCli *cli, char *args, void *context) {
	if (temperature_control_is_powered_on(p_temperature_control_task)) {
		cli_printf(cli, "OK\r\n");
	}
	else {
		cli_printf(cli, "ERROR\r\n");
	}
}

static void CMD_TEC_Init(EmbeddedCli *cli, char *args, void *context) {
	uint32_t channel;
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 1)
	{
		cli_printf(cli, "command require one argument\r\n");
		return;
	}
	if (!temperature_control_is_in_man_state(p_temperature_control_task))
	{
		cli_printf(cli, "can not init TEC manually in AUTO mode, please change to MANUAL mode\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);

	uint32_t result = 0;

	if (*arg1 == 'a' ) {
		/* Init TEC 0 -> 3 */
		for (channel = 0; channel < 4; channel++) {
			result = temperature_control_tec_init(p_temperature_control_task, channel);
			// if init is success
			if (!result) {
				temperature_control_tec_enable_output(p_temperature_control_task,channel,0); //disable output
				cli_printf(cli, "tec[%d] inited\r\n",channel);
			}
			else
				cli_printf(cli, "tec[%d] init failed\r\n",channel);
		}
	}
	else if ((*arg1 >= '0') && (*arg1 <= '3')) {
		channel = atoi(arg1);
		result = temperature_control_tec_init(p_temperature_control_task, channel);
		if (!result) {
			temperature_control_tec_enable_output(p_temperature_control_task,channel,0); //disable output
			cli_printf(cli, "tec[%d] inited\r\n",channel);
		}
		else
			cli_printf(cli, "tec[%d] init failed\r\n",channel);

	}

}

static void CMD_TEC_Set_Profile_Volt(EmbeddedCli *cli, char *args, void *context) {
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 voltage value for 4 tecs\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	uint16_t volt;
	volt = atoi(arg1);


		if ((volt < 500) || (volt > 4500))
		{
			cli_printf(cli, "tec[%d] voltage is out of range (500mV-4500mV)\r\n");
			return;
		}

	temperature_control_profile_tec_voltage_set(p_temperature_control_task,volt);
	cli_printf(cli, "OK\r\n");
}

static void CMD_TEC_Get_Profile_Volt(EmbeddedCli *cli, char *args, void *context)
{

	uint16_t tec_volt;
	tec_volt = temperature_control_profile_tec_voltage_get(p_temperature_control_task);
	cli_printf(cli, "profile setpoint of TEC = %d mV\r\n",tec_volt);
	return;

}
static void CMD_TEC_Profile_Register(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t tec_ena[4];
	uint32_t i;
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 4)
	{
		cli_printf(cli, "require 4 value for 4 tecs (0/1)\r\n");
		return;
	}

	uint8_t tec_ovr = temperature_profile_tec_ovr_get(p_temperature_control_task);
	for (i=0;i<4;i++)
	{
		tec_ena[i] = atoi( embeddedCliGetToken(args, i+1));
		if (tec_ena[i] >  1) {
			cli_printf(cli, "argument %d need to be in range 0/1 \r\n",i);
			return;
		}
		if (tec_ena[i] == 0) temperature_control_profile_tec_unregister(p_temperature_control_task,i);
		else
		{
			if(i == tec_ovr) cli_printf(cli, "tec[%d] registered in ovr mode\r\n",i);
			else
			{
				temperature_control_profile_tec_register(p_temperature_control_task, i);
				cli_printf(cli, "tec[%d] registered\r\n",i);
			}
		}
	}
	cli_printf(cli, "OK\r\n");
}

static void CMD_TEC_Profile_Get(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t tec_profile = temperature_control_profile_tec_get(p_temperature_control_task);
	for (uint32_t i=0;i<4;i++)
	{
		if (tec_profile & (1<<i)) cli_printf(cli, "tec[%d] registered\r\n",i);
		else cli_printf(cli, "tec[%d] unregistered\r\n",i);
	}
}

static void CMD_TEC_Man_Set_Volt(EmbeddedCli *cli, char *args, void *context)
{
	if (!temperature_control_is_in_man_state(p_temperature_control_task))
	{
		cli_printf(cli, "Not in manual mode, can not set\r\n");
		return;
	}
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 3)
	{
		cli_printf(cli, "format: command tec_id tec_dir volt_mV\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t tec_id = atoi(arg1);
	if (tec_id  > 3)
	{
		cli_printf(cli, "arg1: tec id out of range (0-3)\r\n");
				return;
	}
	const char *arg2 = embeddedCliGetToken(args, 2);
	uint32_t tec_dir = atoi(arg2);

	if (tec_dir > 1)
	{
		cli_printf(cli, "arg2: dir is COOL[0] or HEAT[1]\r\n");
		return;
	}
	//tec_dir = TEC_HEAT;  tec_dir = TEC_COOL;

	const char *arg3 = embeddedCliGetToken(args, 3);
	uint16_t tec_volt_mV = atoi(arg3);
	if (tec_volt_mV > 3000)
	{
		cli_printf(cli, "arg3: voltage is out of range (0-3000mV)\r\n");
		return;
	}

	uint32_t ret;
	ret = temperature_control_tec_manual_set_output( p_temperature_control_task,tec_id, tec_dir, tec_volt_mV);
	if (ret)
	{
		cli_printf(cli, "ERROR\r\n");
		return;
	}
	cli_printf(cli, "OK\r\n");
}

//manuallly enable/disable tec output: cmd tec_id 0/1
static void CMD_TEC_Man_Set_Output(EmbeddedCli *cli, char *args, void *context)
{
	if (!temperature_control_is_in_man_state(p_temperature_control_task))
	{
		cli_printf(cli, "Not in manual mode, can not set\r\n");
		return;
	}
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 2)
	{
		cli_printf(cli, "format: command tec_id 0/1\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t tec_id = atoi(arg1);
	if (tec_id  > 3)
	{
		cli_printf(cli, "arg1: tec id out of range (0-3)\r\n");
				return;
	}
	const char *arg2 = embeddedCliGetToken(args, 2);
	uint32_t tec_out_ena = atoi(arg2);
	uint32_t ret = temperature_control_tec_enable_output(p_temperature_control_task,tec_id,tec_out_ena);
	if (ret)
	{
		cli_printf(cli, "ERROR\r\n");
		return;
	}
	cli_printf(cli, "OK\r\n");
}


static void CMD_TEC_OVR_Set_Output(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 3)
	{
		cli_printf(cli, "format: tec_ovr_set [interval_mS] [id] [volt_mV]\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	uint16_t interval = atoi(arg1);
	if (interval > 60000)
	{
		cli_printf(cli, "The time interval is greater than 1 minute\r\n");
		return;
	}

	const char *arg2 = embeddedCliGetToken(args, 2);
	uint8_t tec_profile = temperature_control_profile_tec_get(p_temperature_control_task);
	uint8_t tec_id;
	if(*arg2 == 'o')
	{
		tec_id = 4;
		cli_printf(cli, "tec ovr mode is off\r\n");
	}
	else if ((*arg2 >= '0') && (*arg2 <= '3'))
	{
		tec_id = atoi(arg2);
		if (tec_profile & (1<<tec_id))
		{
			cli_printf(cli, "tec[%d] registered in auto mode\r\n",tec_id);
			return;
		}
	}


	const char *arg3 = embeddedCliGetToken(args, 3);
	uint32_t mvolt = atoi(arg3);
	if ((mvolt < 500) || (mvolt > 3000))
	{
		cli_printf(cli, "tec[%d] voltage is out of range (500mV-3000mV)\r\n");
		return;
	}

	tec_over_set_interval(interval);
	temperature_profile_tec_ovr_register(p_temperature_control_task, tec_id);
	temperature_profile_tec_ovr_voltage_set(p_temperature_control_task, mvolt);
	cli_printf(cli, "OK\r\n");
}


static void CMD_TEC_OVR_Start(EmbeddedCli *cli, char *args, void *context)
{
	tec_ovr_start();
	cli_printf(cli, "OK\r\n");
}
static void CMD_TEC_OVR_Stop(EmbeddedCli *cli, char *args, void *context)
{
	tec_ovr_stop();
	cli_printf(cli, "OK\r\n");
}

static void CMD_TEC_OVR_Get(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t tec_idx = temperature_profile_tec_ovr_get(p_temperature_control_task);
	uint16_t volt_mv = temperature_profile_tec_ovr_get_voltage(p_temperature_control_task);
	if(tec_idx == 4)
	{
		cli_printf(cli, "tec ovr mode is off\r\n");
		return;
	}
	cli_printf(cli, "tec[%d] registered in ovr mode with %dmV\r\n",tec_idx, volt_mv);
}



static void CMD_HTR_Set_Profile_Duty(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 duty value for 4 heater\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint8_t duty = atoi(arg1);
	if (duty > 100)
	{
		cli_printf(cli, "require duty in range of 0-100\r\n");
		return;
	}

	temperature_control_profile_heater_duty_set(p_temperature_control_task,duty);
	cli_printf(cli, "OK\r\n");
}

static void CMD_HTR_Get_Profile_Duty(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t profile_duty = temperature_control_profile_heater_duty_get(p_temperature_control_task);
	cli_printf(cli, "heater profile duty = %d\r\n",profile_duty);
}
static void CMD_Heater_Profile_Register(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t heater_ena[4];
	uint32_t i;
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 4)
	{
		cli_printf(cli, "require 4 value for 4 heater (0/1)\r\n");
		return;
	}
	for (i=0;i<4;i++)
	{
		heater_ena[i] = atoi( embeddedCliGetToken(args, i+1));
		if (heater_ena[i] >  1) {
			cli_printf(cli, "argument %d need to be in range 0/1 \r\n",i);
			return;
		}
		if (heater_ena[i] == 0) temperature_control_profile_heater_unregister(p_temperature_control_task,i);
		else temperature_control_profile_heater_register(p_temperature_control_task, i);
	}
	cli_printf(cli, "OK\r\n");
}

static void CMD_Heater_Profile_Get(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t heater_profile = temperature_control_profile_heater_profile_get(p_temperature_control_task);
	for (uint32_t i=0;i<4;i++)
	{
		if (heater_profile & (1<<i)) cli_printf(cli, "heater[%d] registered\r\n");
		else cli_printf(cli, "heater[%d] unregistered\r\n");
	}
}


static void CMD_Ref_Set_Temp_Limit(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 2)
	{
		cli_printf(cli, "require 2 arg: min & max temperature (251 mean 25.1)\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	int16_t max_temp = atoi(arg1);
	if(max_temp > 1000)
	{
		cli_printf(cli, "max_temp out of range (<1000 mean 100.0C)\r\n");
		return;
	}
	const char *arg2 = embeddedCliGetToken(args, 2);
	int16_t min_temp = atoi(arg2);
	if(min_temp < -1000)
	{
		cli_printf(cli, "min_temp out of range (>-1000 mean -100.0C)\r\n");
		return;
	}
	temperature_control_profile_temp_lim_set(p_temperature_control_task, max_temp, min_temp);
	cli_printf(cli, "OK\r\n");

}

static void CMD_Ref_Get_Temp_Limit(EmbeddedCli *cli, char *args, void *context)
{
	int16_t limt[2];
	temperature_control_profile_temp_lim_get(p_temperature_control_task, limt);
	cli_printf(cli, "Max temperature lim: %d *C\r\n", limt[0]);
	cli_printf(cli, "Min temperature lim: %d *C\r\n", limt[1]);
}

static void CMD_Ref_Set_Temp(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 temperature (251 mean 25.1)\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	int16_t setpoint = atoi(arg1);
	temperature_control_profile_setpoint_set(p_temperature_control_task,setpoint);
	cli_printf(cli, "OK\r\n");

}

static void CMD_Ref_Get_Temp(EmbeddedCli *cli, char *args, void *context)
{

	int16_t setpoint = temperature_control_profile_setpoint_get(p_temperature_control_task);
	cli_printf(cli, "Reference Temperature: %d *C\r\n", setpoint);
}

static void CMD_Ref_Set_NTC(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 2)
	{
		cli_printf(cli, "require 2 NTC index\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t pri_ntc = atoi(arg1);
	if (pri_ntc > 7)
	{
		cli_printf(cli, "pri_ntc index out of range (0-7)\r\n");
		return;
	}

	const char *arg2 = embeddedCliGetToken(args, 2);
	uint32_t sec_ntc = atoi(arg2);
	if (sec_ntc > 7)
	{
		cli_printf(cli, "sec_ntc index out of range (0-7)\r\n");
		return;
	}

	temperature_control_profile_ntc_register(p_temperature_control_task, pri_ntc, sec_ntc);
	cli_printf(cli, "OK\r\n");
}

static void CMD_Ref_Get_NTC(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t NTC_Ref[2] = {0};
	temperature_control_profile_ntc_get(p_temperature_control_task, NTC_Ref);

	cli_printf(cli, "pri_ntc ref is %d\r\n", NTC_Ref[0]);
	cli_printf(cli, "sec_ntc ref is %d\r\n", NTC_Ref[1]);
}

static void CMD_Set_Auto_Recover(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 arg (1/0)\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint8_t status = atoi(arg1);
	if (status > 1)
	{
		cli_printf(cli, "auto_recover status out of range (0-1)\r\n");
		return;
	}
	temperature_control_profile_set_auto_recover(p_temperature_control_task, status);
	cli_printf(cli, "OK\r\n");
}

static void CMD_Get_Auto_Recover(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t status;
	status = temperature_control_profile_get_auto_recover(p_temperature_control_task);

	cli_printf(cli, "Auto recover status: %s\r\n", status ? "on":"off");
}

static void CMD_Start_Auto_Mode(EmbeddedCli *cli, char *args, void *context)
{
	temperature_control_auto_mode_set(p_temperature_control_task);
	cli_printf(cli, "OK\r\n");
}
static void CMD_Stop_Auto_Mode(EmbeddedCli *cli, char *args, void *context)
{
	temperature_control_man_mode_set(p_temperature_control_task);
	cli_printf(cli, "OK\r\n");
}

/*
 * format: laser_int_set_current  percent
 */
static void CMD_Set_Laser_Int_Current(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_set_current percent \r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int16_t percent = atoi(arg1);
	if (percent > 100)
	{
		cli_printf(cli, "argument 1 out of range,(0-100)\r\n");
		return;
	}

	experiment_task_laser_set_current(p_experiment_task, 0, percent);
	cli_printf(cli, "OK\r\n");
}

static void CMD_Set_Laser_Ext_Current(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_set_current 0/1 percent (0 for internal, 1 for external\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int16_t percent = atoi(arg1);
	if (percent > 100)

		{
			cli_printf(cli, "argument 1 out of range,(0-100)\r\n");
			return;
		}
	experiment_task_laser_set_current(p_experiment_task, 1, percent);
	cli_printf(cli, "OK\r\n");
}


static void CMD_Laser_Get_Current(EmbeddedCli *cli, char *args, void *context)
{
	uint16_t current_int = 0;
	uint16_t current_ext = 0;
	uint8_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 1)
	{
		cli_printf(cli, "command require one argument\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);

	if (*arg1 == 'a')
	{
		current_int = bsp_laser_get_int_current();
		current_ext = bsp_laser_get_ext_current();
		cli_printf(cli, "laser_current[int]: %d mA\r\nlaser_current[ext]: %d mA\r\n", current_int, current_ext);
	}
	else if (*arg1 == '0')
	{
		current_int = bsp_laser_get_int_current();
		cli_printf(cli, "laser_current[int]: %d mA\r\n", current_int);
	}
	else if (*arg1 == '1')
	{
		current_ext = bsp_laser_get_ext_current();
		cli_printf(cli, "laser_current[ext]: %d mA\r\n", current_ext);
	}
	else cli_printf(cli, "Wrong arguments\r\n");

}


static void CMD_Int_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_int_switch_on [pos]\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int32_t laser_idx = atoi(arg1);
	if ((laser_idx > INTERNAL_CHAIN_CHANNEL_NUM) || (laser_idx < 1))
	{
		cli_printf(cli, "argument 1 out of range,(1-36)\r\n");
		return;
	}
	experiment_task_int_laser_switchon(p_experiment_task, laser_idx);
	cli_printf(cli, "OK\r\n");
}



static void CMD_Ext_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_ext_switch_on [pos]\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int32_t laser_idx = atoi(arg1);
	if ((laser_idx > EXTERNAL_CHAIN_CHANNEL_NUM)||(laser_idx < 1))
	{
		cli_printf(cli, "argument 1 out of range,(1-8)\r\n");
		return;
	}
	experiment_task_ext_laser_switchon(p_experiment_task, laser_idx);
	cli_printf(cli, "OK\r\n");
}

static void CMD_Int_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context)
{
	bsp_laser_int_switch_off_all();
//	experiment_task_int_laser_switchoff(p_experiment_task);
	cli_printf(cli, "OK\r\n");
}
static void CMD_Ext_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context){
	experiment_task_ext_laser_switchoff(p_experiment_task);
	cli_printf(cli, "OK\r\n");
}

/*
 * format: exp_set_profile sampling_rate pos laser_percent pre_time experiment_time post_time
 * sampling rate in KSample
 * time in us unit
 */

static void cmd_exp_set_profile(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 6)
	{
		cli_printf(cli, "format: exp_set_profile [sampling_rate] [pos] [laser_percent] [pre_time] [experiment_time] [post_time]\r\n");
		return;
	}

	uint32_t sample_rate = atoi(embeddedCliGetToken(args, 1));
	if ((sample_rate < 1000) || (sample_rate > 850000))
	{
		cli_printf(cli, "sampling rate out of range (1K-800K)\r\n");
		return;
	}

	uint32_t pos = atoi(embeddedCliGetToken(args, 2));
	if ((pos == 0) || (pos > 36))
	{
		cli_printf(cli, "pos out of range (1-36)\r\n");
		return;
	}

	uint32_t percent = atoi(embeddedCliGetToken(args, 3));
	if (percent > 100)
	{
		cli_printf(cli, "percent out of range (0-100)\r\n");
		return;
	}

	uint32_t pre_time = atoi(embeddedCliGetToken(args, 4));
	if (pre_time == 0)
	{
		cli_printf(cli, "pre_time should be larger than 0\r\n");
		return;
	}

	uint32_t sample_time = atoi(embeddedCliGetToken(args, 5));
	if (sample_time == 0)
	{
		cli_printf(cli, "sample time should be larger than 0\r\n");
		return;
	}

	uint32_t post_time = atoi(embeddedCliGetToken(args, 6));
	if (post_time == 0)
	{
		cli_printf(cli, "post_time should be larger than 0\r\n");
		return;
	}

    uint32_t num_sample_x1024 = ((pre_time + sample_time + post_time) * sample_rate) /1024000;
	if (num_sample_x1024 > 2048)	//larger than 4MB
	{
		cli_printf(cli, "total sample must be less than 2048K \r\n");
		return;
	}

	experiment_profile_t profile;
	profile.sampling_rate = sample_rate;				// Hz
	profile.pos = pos;
	profile.laser_percent = percent;
	profile.pre_time = pre_time;						// mS
	profile.experiment_time = sample_time;				// mS
	profile.post_time = post_time;						// mS
	profile.num_sample_x1024 = num_sample_x1024;		// kSample
	profile.period = 1000000 / sample_rate;				// uS

	if(!experiment_task_set_profile(p_experiment_task, &profile))
		cli_printf(cli, "OK\r\n");
	else cli_printf(cli, "ERROR\r\n");
}

static void cmd_exp_get_profile(EmbeddedCli *cli, char *args, void *context)
{
	experiment_profile_t profile;
	experiment_task_get_profile(p_experiment_task, &profile);
	cli_printf(cli, "sampling_rate: %d Hz\r\n"
					"position     : %d\r\n"
					"percent      : %d %\r\n"
					"pre_time     : %d ms\r\n"
					"sample_time  : %d ms\r\n"
					"post_time    : %d ms\r\n"
					"num_sample   : %d kSample\r\n",
					profile.sampling_rate,
					profile.pos,
					profile.laser_percent,
					profile.pre_time,
					profile.experiment_time,
					profile.post_time,
					profile.num_sample_x1024);
}

static void cmd_exp_start_measuring(EmbeddedCli *cli, char *args, void *context)
{
	if (experiment_start_measuring(p_experiment_task))
	{
		cli_printf(cli, "Wrong profile, please check \r\n");
	}
	else
	{
		min_shell_busy_set(p_min_shell_task);
		is_measured = true;
	}
}

static void cmd_exp_ram_read(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 3)
	{
		cli_printf(cli, "format: exp_ram_read [address] [num_sample] [mode]\r\n");
		return;
	}

	if(	(p_experiment_task->sub_state != NO_SUBSTATE)&&
		(p_experiment_task->sub_state != S_AQUI_ERROR)&&
		(p_experiment_task->sub_state != S_AQUI_TIMEOUT))
	{
		cli_printf(cli, "EXP is in sampling process!\r\n");
		return;
	}

	// 32Mb = 4096kB = 2048kW = 2097152W -> addr < (2097152 - 1)W
	uint32_t start_address = atoi(embeddedCliGetToken(args, 1));		// calculated by halfword
	if (start_address > 2097151)
	{
		cli_printf(cli, "address out of range (0-2097151 nsamples)\r\n");
		return;
	}

	// 32Mb = 4096kB = 2048kW = 2097152W -> num_sample <= (2097152 - addr) sample
	uint32_t num_data = atoi(embeddedCliGetToken(args, 2));		// calculated by halfword
	if ((num_data > 2097152 - start_address)||(num_data < 512))
	{
		cli_printf(cli, "num samples out of range (512-%d samples)\r\n", 2097152 - start_address);
		return;
	}

	uint8_t mode = atoi(embeddedCliGetToken(args, 3));		// calculated by halfword
	if (mode > 1)
	{
		cli_printf(cli, "mode format is 0: print by ascii, 1: print by binary\r\n");
		return;
	}

	experiment_task_get_ram_data(p_experiment_task, start_address, num_data, mode);

}

static void cmd_exp_data_transfer(EmbeddedCli *cli, char *args, void *context)
{
	if(is_measured) experiment_task_data_transfer(p_experiment_task);
	else cli_printf(cli, "please use 'start measure' first!\r\n");
	return;
}


/*************************************************
 *                  End CMD List                 *
 *************************************************/

/*************************************************
 *                Getter - Helper                *
 *************************************************/
const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof(cliStaticBindings_internal) / sizeof(cliStaticBindings_internal[0]);
}
