/*
 * board.c
 *
 *  Created on: Apr 4, 2025
 *      Author: HTSANG
 */


#include "board.h"
#include "lt8722.h"
#include "adg1414.h"
#include "ads8327.h"
#include "mb85rs2mt.h"


struct mb85rs2mt_dev fram = {
		  .hspi = SPI6,
		  .cs_port = FRAM_CS_GPIO_Port,
		  .cs_pin = FRAM_CS_Pin
};

//MCP4902_Device_t DAC_device;
//ADG1414_Device_t laser_int;
//ADG1414_Device_t laser_ext;
////ADC_DMA_Device_t laser_adc;
//ADG1414_Device_t photo_sw;

