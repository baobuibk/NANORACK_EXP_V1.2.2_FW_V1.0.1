/*
 * bsp_ntc.c
 *
 *  Created on: Jun 22, 2025
 *      Author: Admin
 */


#include "bsp_ntc.h"
#include "ntc.h"
#include "temperature_control.h"
#include "stm32f7xx_ll_dma.h"
#include "app_signals.h"
#include "error_codes.h"

#include "stdlib.h"

//#include "adc_monitor.h"
//extern monitor_task_t monitor_task_inst;

#ifdef	HW_V120
#define NTC_DMA				DMA2
#define NTC_DMA_STREAM		LL_DMA_STREAM_0
#endif

#define HW_V122
#ifdef	HW_V122
#define NTC_DMA				DMA2
#define NTC_DMA_STREAM		LL_DMA_STREAM_4
#endif

static ntc_confi_t ntc_config = {
		.dma = NTC_DMA,
		.dma_stream = NTC_DMA_STREAM
};

static uint16_t ntc_ADC_value[8] = {0,0,0,0,0,0,0,0};
//static uint16_t ntc_ADC_value_average[8] = {0,0,0,0,0,0,0,0};


static uint16_t ntc_adc_value_buff[8][10] = {0};
static uint8_t ntc_adc_value_buff_idx = 0;

//static monitor_evt_t const ntc_adc_evt = {.super = {.sig = EVT_MONITOR_NTC_ADC_COMPLETED} };

void bsp_ntc_adc_init(void)
{
	ntc_adc_dma_init(&ntc_config, ntc_ADC_value, 8);
//	ntc_adc_dma_init(ntc_ADC_value, 8);
}

void bsp_ntc_trigger_adc(void)
{
	ntc_adc_trigger();
}

int16_t bsp_ntc_get_temperature(uint32_t ntc_channel)
{
//	return ntc_convert(ntc_ADC_value_average[ntc_channel]);

	uint32_t sum_adc = 0;
	for(uint8_t i = 0; i < 10; i++) {
		sum_adc += ntc_adc_value_buff[ntc_channel][i];
	}
	return ntc_convert((uint16_t)(sum_adc / 10));
}

uint16_t temperature_monitor_get_ntc_error(uint32_t channel1, uint32_t channel2, int16_t max_temp, int16_t min_temp)
{
	int16_t ntc[2] = {ntc_convert(ntc_ADC_value[channel1]), ntc_convert(ntc_ADC_value[channel2])};
	if((ntc[0] < min_temp) || (ntc[0] > max_temp) || (abs(ntc[0] - ntc[1]) > TEMPERATURE_MAX_ERROR))
	{
		return ERROR_FAIL;
	}
	return ERROR_OK;
}


void bsp_ntc_dma_adc_irq(void)
{
#ifdef HW_V120
	// Transfer-complete (stream 0)
	if (NTC_DMA->LISR & DMA_LISR_TCIF0)
	{
		NTC_DMA->LIFCR = DMA_LIFCR_CTCIF0;	// Clear Transfer-complete flag
		if (sample_count == 0)
		{
			for (uint32_t i = 0; i < 8; i++ ) ntc_ADC_value_average[i] = ntc_ADC_value[i];
			sample_count++;
		}
		else if(sample_count < 10)
		{
			for (uint32_t i = 0; i< 8; i++ ) ntc_ADC_value_average[i] = (ntc_ADC_value[i] + ntc_ADC_value_average[i])/2;
			sample_count++;
		}
		else
		{
			sample_count = 0;
			SST_Task_post(&monitor_task_inst.super, (SST_Evt *)&ntc_adc_evt); //post to temperature monitor task
		}
	}

	// Clear all unexpected interrupt flag (stream 0)
	else
	{
		NTC_DMA->LIFCR = DMA_LIFCR_CHTIF0;		// Clear alf-transfer flag
		NTC_DMA->LIFCR = DMA_LIFCR_CTEIF0;		// Clear Transfer-error flag
		NTC_DMA->LIFCR = DMA_LIFCR_CDMEIF0;		// Clear Direct-mode-error flag
		NTC_DMA->LIFCR = DMA_LIFCR_CFEIF0;		// Clear FIFO-error flag
	}
#endif

#ifdef HW_V122
	// Transfer-complete (stream 4)
	if (NTC_DMA->HISR & DMA_HISR_TCIF4)
	{
		NTC_DMA->HIFCR = DMA_HIFCR_CTCIF4;	// Clear Transfer-complete flag

		ntc_adc_value_buff[0][ntc_adc_value_buff_idx] = ntc_ADC_value[0];
		ntc_adc_value_buff[1][ntc_adc_value_buff_idx] = ntc_ADC_value[1];
		ntc_adc_value_buff[2][ntc_adc_value_buff_idx] = ntc_ADC_value[2];
		ntc_adc_value_buff[3][ntc_adc_value_buff_idx] = ntc_ADC_value[3];
		ntc_adc_value_buff[4][ntc_adc_value_buff_idx] = ntc_ADC_value[4];
		ntc_adc_value_buff[5][ntc_adc_value_buff_idx] = ntc_ADC_value[5];
		ntc_adc_value_buff[6][ntc_adc_value_buff_idx] = ntc_ADC_value[6];
		ntc_adc_value_buff[7][ntc_adc_value_buff_idx] = ntc_ADC_value[7];

		ntc_adc_value_buff_idx++;
		if (ntc_adc_value_buff_idx >= 10) ntc_adc_value_buff_idx = 0;

	}

	// Clear all unexpected interrupt flag (stream 4)
	else
	{
		NTC_DMA->HIFCR = DMA_HIFCR_CHTIF4;		// Clear alf-transfer flag
		NTC_DMA->HIFCR = DMA_HIFCR_CTEIF4;		// Clear Transfer-error flag
		NTC_DMA->HIFCR = DMA_HIFCR_CDMEIF4;		// Clear Direct-mode-error flag
		NTC_DMA->HIFCR = DMA_HIFCR_CFEIF4;		// Clear FIFO-error flag
	}
#endif
}
