/*
 * bsp_laser.c
 *
 *  Created on: Jun 25, 2025
 *      Author: Admin
 */
#include "bsp_laser.h"
#include "app_signals.h"
#include "adc_monitor.h"
#include "experiment_task.h"
#include "error_codes.h"
#include "bsp_spi_slave.h"
#include "spi_transmit.h"
#include "lwl.h"

#include <stdbool.h>

#ifdef HW_V120
	#define LASER_SPI				SPI4
	#define LASER_DMA				DMA2
	#define LASER_INT_DMA_STREAM	LL_DMA_STREAM_2
	#define LASER_EXT_DMA_STREAM	LL_DMA_STREAM_1
#endif

#define HW_V122
#ifdef HW_V122
	#define LASER_SPI				SPI2
	#define LASER_DMA				DMA2
	#define LASER_INT_DMA_STREAM	LL_DMA_STREAM_2
	#define LASER_EXT_DMA_STREAM	LL_DMA_STREAM_1
#endif

bool collect_data_flag = false;
__attribute__((aligned(4))) static uint16_t laser_int_current_buffer[EXPERIMENT_LASER_CURRENT_SIZE];
static uint16_t laser_int_current_idx;

static uint16_t ld_adc_value[2] = {0};
static uint16_t ld_adc_value_buff[2][10] = {0};
static uint8_t ld_adc_value_buff_idx = 0;

MCP4902_Device_t DAC_device;
ADG1414_Device_t laser_int;
ADG1414_Device_t laser_ext;

static uint8_t bsp_laser_int_current = 0;
static uint8_t bsp_laser_ext_current = 0;
static uint8_t bsp_laser_ext_mask = 0;

extern spi_transmit_task_t spi_transmit_task_inst;
static spi_transmit_task_t *p_spi_transmit_task = &spi_transmit_task_inst;

static uint16_t current_calculate(uint16_t adc_val)	// return mA
{
	float temp = (adc_val * ADC_VREF * 10) / ADC_MAX;	//mV x 10 times
	temp /= ADC_RES_SHUNT;
	return (uint16_t)(temp);
}

void bsp_laser_set_spi_prescaler(uint32_t Prescaler)
{
    LL_SPI_Disable(LASER_SPI);
    LL_SPI_SetBaudRatePrescaler(LASER_SPI, Prescaler);
    LL_SPI_Enable(LASER_SPI);
}

void bsp_laser_set_spi_mode(spi_mode_t spi_mode)
{
	LL_SPI_Disable(LASER_SPI);
	switch(spi_mode)
	{
		case SPI_MODE_0:
			LL_SPI_SetClockPolarity(LASER_SPI, LL_SPI_POLARITY_LOW);
			LL_SPI_SetClockPhase(LASER_SPI, LL_SPI_PHASE_1EDGE);
		break;

		case SPI_MODE_1:
			LL_SPI_SetClockPolarity(LASER_SPI, LL_SPI_POLARITY_LOW);
			LL_SPI_SetClockPhase(LASER_SPI, LL_SPI_PHASE_2EDGE);
		break;

		case SPI_MODE_2:
			LL_SPI_SetClockPolarity(LASER_SPI, LL_SPI_POLARITY_HIGH);
			LL_SPI_SetClockPhase(LASER_SPI, LL_SPI_PHASE_1EDGE);
		break;

		case SPI_MODE_3:
			LL_SPI_SetClockPolarity(LASER_SPI, LL_SPI_POLARITY_HIGH);
			LL_SPI_SetClockPhase(LASER_SPI, LL_SPI_PHASE_2EDGE);
		break;
	}
	LL_SPI_Enable(LASER_SPI);
}

void bsp_laser_init(void)
{
	bsp_laser_set_spi_mode(SPI_MODE_0);
	MCP4902_Device_Init(&DAC_device, LASER_SPI, LASER_DAC_CS_GPIO_Port, LASER_DAC_CS_Pin, LASER_DAC_LATCH_GPIO_Port, LASER_DAC_LATCH_Pin);

	bsp_laser_set_spi_mode(SPI_MODE_1);
	ADG1414_Chain_Init(&laser_int, LASER_SPI, LASER_INT_SW_CS_GPIO_Port, LASER_INT_SW_CS_Pin, INTERNAL_CHAIN_SWITCH_NUM);
	ADG1414_Chain_Init(&laser_ext, LASER_SPI, LASER_EXT_SW_CS_GPIO_Port, LASER_EXT_SW_CS_Pin, EXTERNAL_CHAIN_SWITCH_NUM);

	// Switch off all laser (turn on laser with current 0mA)
	bsp_laser_int_switch_off_all();
}

void bsp_laser_int_switch_on(uint32_t channel_idx)
{
	bsp_laser_set_spi_mode(SPI_MODE_1);
	ADG1414_Chain_SwitchOn(&laser_int, channel_idx);
	bsp_laser_set_spi_mode(SPI_MODE_0);
	MCP4902_Set_DAC(&DAC_device, 0, bsp_laser_int_current);
}

void bsp_laser_int_switch_off_all(void)
{
//	bsp_laser_set_spi_mode(SPI_MODE_1);
//	ADG1414_Chain_SwitchAllOff(&laser_int);

	bsp_laser_set_spi_mode(SPI_MODE_0);
	MCP4902_Set_DAC(&DAC_device, 0, 0);
	bsp_laser_set_spi_mode(SPI_MODE_1);
	ADG1414_Chain_SwitchAllOn(&laser_int);
}

void bsp_laser_ext_switch_on(uint32_t channel_idx)
{
	bsp_laser_ext_mask |= (1 << (channel_idx - 1));
	bsp_laser_set_spi_mode(SPI_MODE_1);
	ADG1414_Chain_SwitchOn(&laser_ext, bsp_laser_ext_mask);
	bsp_laser_set_spi_mode(SPI_MODE_0);
	MCP4902_Set_DAC(&DAC_device, 1, bsp_laser_ext_current);
}

void bsp_laser_ext_switch_on_mask(uint32_t mask)
{
	bsp_laser_ext_mask = mask;
	bsp_laser_set_spi_mode(SPI_MODE_1);
	ADG1414_Chain_SwitchOn(&laser_ext, bsp_laser_ext_mask);
	bsp_laser_set_spi_mode(SPI_MODE_0);
	MCP4902_Set_DAC(&DAC_device, 1, bsp_laser_ext_current);
}

void bsp_laser_ext_switch_off_all(void)
{
//	bsp_laser_set_spi_mode(SPI_MODE_1);
//	ADG1414_Chain_SwitchAllOff(&laser_ext);

	bsp_laser_ext_mask = 0;
	bsp_laser_set_spi_mode(SPI_MODE_0);
	MCP4902_Set_DAC(&DAC_device, 1, bsp_laser_ext_mask);
	bsp_laser_set_spi_mode(SPI_MODE_1);
	ADG1414_Chain_SwitchAllOn(&laser_ext);
}

void bsp_laser_int_set_current(uint32_t percent)
{
//	bsp_laser_set_spi_mode(SPI_MODE_0);
//	if (percent > 100) percent = 100;
//	MCP4902_Set_DAC(&DAC_device, 0, 255*percent/100);

	if (percent > 100) percent = 100;
	bsp_laser_int_current = 255 *percent /100;
}

void bsp_laser_ext_set_current(uint32_t percent)
{
//	bsp_laser_set_spi_mode(SPI_MODE_0);
//	if (percent > 100) percent = 100;
//	MCP4902_Set_DAC(&DAC_device, 1, 255*percent/100);

	if (percent > 100) percent = 100;
	bsp_laser_ext_current = 255 *percent /100;
}

void bsp_laser_set_current(uint32_t id, uint32_t percent)
{
	if (id == 0) bsp_laser_int_set_current(percent);
	else bsp_laser_ext_set_current(percent);
}

uint16_t bsp_laser_get_int_current(void)
{
	uint32_t sum = 0;
	for(uint8_t i = 0; i < 10; i++) {
		sum += ld_adc_value_buff[0][i];
	}
	return current_calculate((uint16_t)(sum / 10));
}

uint16_t bsp_laser_get_ext_current(void)
{
	uint32_t sum = 0;
	for(uint8_t i = 0; i < 10; i++) {
		sum += ld_adc_value_buff[1][i];
	}
	return current_calculate((uint16_t)(sum / 10));
}

void bsp_laser_adc_init(void)
{
	// Disable stream DMA before configure
	LL_DMA_DisableStream(LASER_DMA, LASER_INT_DMA_STREAM);
	while (LL_DMA_IsEnabledStream(LASER_DMA, LASER_INT_DMA_STREAM));

	// Configure DMA
    LL_DMA_SetPeriphIncMode(LASER_DMA, LASER_INT_DMA_STREAM, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(LASER_DMA, LASER_INT_DMA_STREAM, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(LASER_DMA, LASER_INT_DMA_STREAM, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(LASER_DMA, LASER_INT_DMA_STREAM, LL_DMA_MDATAALIGN_HALFWORD);
    LL_DMA_SetDataLength(LASER_DMA, LASER_INT_DMA_STREAM, 2); // 2 kênh
    LL_DMA_SetPeriphAddress(LASER_DMA, LASER_INT_DMA_STREAM, (uint32_t)&ADC2->DR);
    LL_DMA_SetMemoryAddress(LASER_DMA, LASER_INT_DMA_STREAM, (uint32_t)ld_adc_value);
    LL_DMA_SetMode(LASER_DMA, LASER_INT_DMA_STREAM, LL_DMA_MODE_CIRCULAR);

    // Enable half-transfer và transfer-complete interrupt
    LL_DMA_EnableIT_TC(LASER_DMA, LASER_INT_DMA_STREAM);

    // Enable ADC
    LL_ADC_Enable(ADC2);

    // Enable DMA stream
    LL_DMA_EnableStream(LASER_DMA, LASER_INT_DMA_STREAM);
}

void bsp_laser_trigger_adc(void)
{
	LL_ADC_REG_StartConversionSWStart(ADC2);
}

void bsp_laser_collect_current_data_to_buffer(void)
{
	collect_data_flag = true;
	laser_int_current_idx = 0;
}

uint32_t bsp_laser_send_current_to_spi(void)
{
	// Add log
	LWL(LWL_EXP_TRANS_LOG_DATA);

	// Cấu hình địa chỉ buffer
	SPI_SlaveDevice_CollectData((uint16_t *)laser_int_current_buffer);
	// Bật tín hiệu DataReady
	spi_transmit_task_data_ready(p_spi_transmit_task);
	return ERROR_OK;
}

void bsp_laser_dma_adc_current_irq(void)
{
	if (LASER_DMA->LISR & DMA_LISR_TCIF2)
	{
		LASER_DMA->LIFCR = DMA_LIFCR_CTCIF2;
		if (collect_data_flag)
		{
			laser_int_current_buffer[laser_int_current_idx] = current_calculate(ld_adc_value[0]);
			laser_int_current_idx ++;
			if (laser_int_current_idx >= EXPERIMENT_LASER_CURRENT_SIZE)
			{
				collect_data_flag = false;
				laser_int_current_idx = 0;
			}
		}

		ld_adc_value_buff[0][ld_adc_value_buff_idx] = ld_adc_value[0];
		ld_adc_value_buff[1][ld_adc_value_buff_idx] = ld_adc_value[1];
		ld_adc_value_buff_idx++;
		if (ld_adc_value_buff_idx >= 10) ld_adc_value_buff_idx = 0;
	}
	else
	{
		LASER_DMA->LIFCR = DMA_LIFCR_CHTIF2;		// Clear alf-transfer flag
		LASER_DMA->LIFCR = DMA_LIFCR_CTEIF2;		// Clear Transfer-error flag
		LASER_DMA->LIFCR = DMA_LIFCR_CDMEIF2;		// Clear Direct-mode-error flag
		LASER_DMA->LIFCR = DMA_LIFCR_CFEIF2;		// Clear FIFO-error flag
	}
}

#include "embedded_cli.h"
extern EmbeddedCli * shell_uart_cli;

void haha(void)
{
    for (int i = 0; i < 512; i ++) {
		cli_printf(shell_uart_cli, "%d  ", laser_int_current_buffer[i]);
    }
    cli_printf(shell_uart_cli, "");
}


