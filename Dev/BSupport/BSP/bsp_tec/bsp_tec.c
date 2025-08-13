/*
 * bsp_tec.c
 *
 *  Created on: Jun 22, 2025
 *      Author: Admin
 */

#include "board.h"
#include "bsp_tec.h"
#include "stdint.h"
#include "error_codes.h"


struct lt8722_dev tec_0 = {
		  .hspi = SPI3,
		  .cs_port = TEC_1_CS_GPIO_Port,
		  .cs_pin = TEC_1_CS_Pin,
		  .en_port = TEC_1_EN_GPIO_Port,
		  .en_pin = TEC_1_EN_Pin,
		  .swen_port = TEC_1_SWEN_GPIO_Port,
		  .swen_pin = TEC_1_SWEN_Pin,

		  .status = 0
};

struct lt8722_dev tec_1 = {
		  .hspi = SPI3,
		  .cs_port = TEC_2_CS_GPIO_Port,
		  .cs_pin = TEC_2_CS_Pin,
		  .en_port = TEC_2_EN_GPIO_Port,
		  .en_pin = TEC_2_EN_Pin,
		  .swen_port = TEC_2_SWEN_GPIO_Port,
		  .swen_pin = TEC_2_SWEN_Pin,

		  .status = 0
};

struct lt8722_dev tec_2 = {
		  .hspi = SPI3,
		  .cs_port = TEC_3_CS_GPIO_Port,
		  .cs_pin = TEC_3_CS_Pin,
		  .en_port = TEC_3_EN_GPIO_Port,
		  .en_pin = TEC_3_EN_Pin,
		  .swen_port = TEC_3_SWEN_GPIO_Port,
		  .swen_pin = TEC_3_SWEN_Pin,

		  .status = 0
};

struct lt8722_dev tec_3 = {
		  .hspi = SPI3,
		  .cs_port = TEC_4_CS_GPIO_Port,
		  .cs_pin = TEC_4_CS_Pin,
		  .en_port = TEC_4_EN_GPIO_Port,
		  .en_pin = TEC_4_EN_Pin,
		  .swen_port = TEC_4_SWEN_GPIO_Port,
		  .swen_pin = TEC_4_SWEN_Pin,

		  .status = 0
};

//struct lt8722_dev * tec_table[] = {&tec_0, &tec_1, &tec_2, &tec_3};
//
//uint32_t bsp_tec_enable(uint32_t tec_idx)
//{
//	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;
//	return lt8722_set_swen_req(tec_table[tec_idx], LT8722_SWEN_REQ_ENABLED);
//}
//uint32_t bsp_tec_disable(uint32_t tec_idx)
//{
//	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;
//	return lt8722_set_swen_req(tec_table[tec_idx], LT8722_SWEN_REQ_DISABLED);
//}

uint32_t bsp_temperature_power_on(void)
{
	LL_GPIO_SetOutputPin(EF_5_EN_GPIO_Port, EF_5_EN_Pin);
	return ERROR_OK;
}
uint32_t bsp_temperature_power_off(void)
{
	LL_GPIO_ResetOutputPin(EF_5_EN_GPIO_Port, EF_5_EN_Pin);
	return ERROR_OK;
}
//
//uint32_t bsp_tec_init(uint32_t tec_idx)
//{
//	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;
//	return lt8722_init(tec_table[tec_idx]);
//}
//uint32_t bsp_tec_output_voltage(uint32_t tec_idx, uint32_t dir, uint16_t voltage)
//{
//	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;
//	return lt8722_set_output_voltage_channel(tec_table[tec_idx], dir, (int64_t)voltage * 1000000);
//}
