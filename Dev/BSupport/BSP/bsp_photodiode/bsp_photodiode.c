/*
 * bsp_photodiode.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Admin
 */

#include "bsp_photodiode.h"
#include "bsp_laser.h"
#include "main.h"
#include "experiment_task.h"
#include "app_signals.h"
#include "bsp_spi_ram.h"


#ifdef HW_V120
	#define PHOTO_DMA			DMA1
	#define PHOTO_DMA_STREAM	LL_DMA_STREAM_1

	#define PHOTO_SPI			SPI2
#endif

#define HW_V122
#ifdef HW_V122
	#define PHOTO_DMA			DMA2
	#define PHOTO_DMA_STREAM	LL_DMA_STREAM_0

	#define PHOTO_SPI			SPI4
#endif

///////////////////// CLI CONSOLE //////////////////////
#include "embedded_cli.h"
extern EmbeddedCli * shell_uart_cli;
///////////////////// CLI CONSOLE //////////////////////

uint16_t photo_data_buffer[BUFFER_FULL_SIZE];
uint16_t * const upper_data_buffer = photo_data_buffer + BUFFER_HALF_SIZE;

photo_diode_t photo_diode_adc = {
		.spi = PHOTO_SPI,
		.dma = PHOTO_DMA,
		.dma_stream_rx = PHOTO_DMA_STREAM
};

typedef struct bsp_photodiode_timer_param_t
{
	uint32_t pre_time_ARR;
	uint32_t sampling_time_ARR;
	uint32_t post_time_ARR;
	uint32_t sampling_period_ARR;
} bsp_photodiode_timer_param_t;


ADG1414_Device_t photo_sw;
ADS8327_Device_t ads8327_dev = {
		.spi = PHOTO_SPI,
		.cs_port = PHOTO_ADC_CS_GPIO_Port,
		.cs_pin = PHOTO_ADC_CS_Pin,
		.convst_port = PHOTO_ADC_CONV_GPIO_Port,
		.convst_pin = PHOTO_ADC_CONV_Pin,
		.EOC_port = PHOTO_ADC_EOC_GPIO_Port,
		.EOC_pin = PHOTO_ADC_EOC_Pin
};

static photo_state_t photo_diode_state = PHOTO_SAMPLED_PRE;


bsp_photodiode_timer_param_t timer_timing;

static experiment_evt_t const finish_pre_phase_evt = {.super = {.sig = EVT_EXPERIMENT_FINISH_PRE_SAMPLING} };
static experiment_evt_t const finish_sampling_phase_evt = {.super = {.sig = EVT_EXPERIMENT_FINISH_SAMPLING} };
static experiment_evt_t const finish_post_phase_evt = {.super = {.sig = EVT_EXPERIMENT_FINISH_POST_SAMPLING} };

extern experiment_task_t experiment_task_inst;
//static experiment_task_t * p_experiment_task = &experiment_task_inst;

void bsp_photo_set_time(bsp_photodiode_time_t * init_photo_time);
void bsp_photodiode_init(void);
void bsp_photodiode_set_spi_mode(spi_mode_t spi_mode);
void bsp_photodiode_set_spi_data_len(uint32_t DataWidth);
void bsp_photodiode_set_spi_prescaler(uint32_t Prescaler);
void bsp_photodiode_sw_spi_change_mode(void);
void bsp_photodiode_adc_spi_change_mode(void);
void bsp_photo_switch_on(uint32_t channel_idx);
void bsp_photo_switch_off_all(void);
void bsp_photodiode_sample_rate_timer_init(void);
void bsp_photodiode_set_pre_time(void);
void bsp_photodiode_set_sampling_time(void);
void bsp_photodiode_set_post_time(void);
void bsp_photodiode_timer1_init(uint32_t period);
void bsp_photodiode_timer2_init(uint32_t period);
void bsp_photo_prepare_timer_sampling(void);
void bsp_photo_start_timer_sampling(void);
void bsp_photodiode_sample_start(void);
void bsp_photodiode_start_dma(photo_diode_t *config, uint32_t *buffer, uint32_t size);

/////////////////////////////////////////////////////////////////////////

void bsp_photo_set_time(bsp_photodiode_time_t * init_photo_time)
{
	photo_diode_adc.timing = * init_photo_time;
	bsp_photodiode_time_t * photo_time = &photo_diode_adc.timing;
	//timer2 clock is 80 Mhz, meam 80 tick = 1us
	timer_timing.pre_time_ARR = photo_time->pre_time * 80000;
	timer_timing.sampling_time_ARR = photo_time->sampling_time * 80000;
	timer_timing.post_time_ARR = photo_time->post_time * 80000;
	//sampling rate in Khz, timer1 clock is 160 Mhz, mean 160 tick = 1us
	timer_timing.sampling_period_ARR = (160000000 / photo_time->sampling_rate); // ticks
}

void bsp_photodiode_init(void)
{
	bsp_photodiode_set_spi_mode(SPI_MODE_1);
	bsp_photodiode_sw_spi_change_mode();
	ADG1414_Chain_Init(&photo_sw, PHOTO_SPI, PHOTO_SW_CS_GPIO_Port, PHOTO_SW_CS_Pin, INTERNAL_CHAIN_SWITCH_NUM);
	bsp_photodiode_adc_spi_change_mode();
	ADS8327_Device_Init(&ads8327_dev);
}


void bsp_photodiode_set_spi_mode(spi_mode_t spi_mode)
{
	LL_SPI_Disable(PHOTO_SPI);
	switch(spi_mode)
	{
		case SPI_MODE_0:
			LL_SPI_SetClockPolarity(PHOTO_SPI, LL_SPI_POLARITY_LOW);
			LL_SPI_SetClockPhase(PHOTO_SPI, LL_SPI_PHASE_1EDGE);
		break;

		case SPI_MODE_1:
			LL_SPI_SetClockPolarity(PHOTO_SPI, LL_SPI_POLARITY_LOW);
			LL_SPI_SetClockPhase(PHOTO_SPI, LL_SPI_PHASE_2EDGE);
		break;

		case SPI_MODE_2:
			LL_SPI_SetClockPolarity(PHOTO_SPI, LL_SPI_POLARITY_HIGH);
			LL_SPI_SetClockPhase(PHOTO_SPI, LL_SPI_PHASE_1EDGE);
		break;

		case SPI_MODE_3:
			LL_SPI_SetClockPolarity(PHOTO_SPI, LL_SPI_POLARITY_HIGH);
			LL_SPI_SetClockPhase(PHOTO_SPI, LL_SPI_PHASE_2EDGE);
		break;
	}
	LL_SPI_Enable(PHOTO_SPI);
}

void bsp_photodiode_set_spi_data_len(uint32_t DataWidth)
{
    LL_SPI_SetDataWidth(PHOTO_SPI, DataWidth);
    if (DataWidth < LL_SPI_DATAWIDTH_9BIT)
	{
    	LL_SPI_SetRxFIFOThreshold(PHOTO_SPI, LL_SPI_RX_FIFO_TH_QUARTER);
	}
    else
    {
    	LL_SPI_SetRxFIFOThreshold(PHOTO_SPI, LL_SPI_RX_FIFO_TH_HALF);
    }
}

void bsp_photodiode_set_spi_prescaler(uint32_t Prescaler)
{
    LL_SPI_SetBaudRatePrescaler(PHOTO_SPI, Prescaler);
}

void bsp_photodiode_sw_spi_change_mode(void)
{
	LL_SPI_Disable(PHOTO_SPI);
	bsp_photodiode_set_spi_data_len(LL_SPI_DATAWIDTH_8BIT);
	bsp_photodiode_set_spi_prescaler(LL_SPI_BAUDRATEPRESCALER_DIV32);
	LL_SPI_Enable(PHOTO_SPI);
}

void bsp_photodiode_adc_spi_change_mode(void)
{
	LL_SPI_Disable(PHOTO_SPI);
	bsp_photodiode_set_spi_data_len(LL_SPI_DATAWIDTH_16BIT);
	bsp_photodiode_set_spi_prescaler(LL_SPI_BAUDRATEPRESCALER_DIV2);
	LL_SPI_Enable(PHOTO_SPI);
}

void bsp_photo_switch_on(uint32_t channel_idx)
{
	ADG1414_Chain_SwitchOn(&photo_sw, channel_idx);
}

void bsp_photo_switch_off_all(void)
{
	ADG1414_Chain_SwitchAllOff(&photo_sw);
}


void bsp_photodiode_timer1_init(uint32_t period) 			// period in tick
{
    uint16_t prescaler = 0;
    uint16_t arr = period - 1;

    if (period > 0xFFFF)
    {
        prescaler = (period/ 0xFFFF) + 1;
        arr = (period / prescaler) - 1;
    }

    TIM1->CR1 = 0;
	TIM1->DIER = 0;
	TIM1->SR = 0;
	TIM1->CNT = 0;

    TIM1->ARR = arr;
    TIM1->PSC = prescaler;

    TIM1->DIER |= TIM_DIER_UIE;
}


void bsp_photodiode_timer2_init(uint32_t period) 			// period in tick
{
    uint16_t prescaler = 0;
    uint32_t arr = period - 1; 	// tick - 1

    TIM2->CR1 = 0;
	TIM2->DIER = 0;
	TIM2->SR = 0;
	TIM2->CNT = 0;

    TIM2->ARR = arr;
    TIM2->PSC = prescaler;

    TIM2->DIER |= TIM_DIER_UIE;
}

void bsp_photo_prepare_timer_sampling(void)
{
	bsp_photodiode_timer1_init(timer_timing.sampling_period_ARR);
	bsp_photodiode_timer2_init(timer_timing.pre_time_ARR);
}

void bsp_photo_start_timer_sampling(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM1->CR1 |= TIM_CR1_CEN;
	photo_diode_state = PHOTO_SAMPLED_PRE;
}

void bsp_photodiode_start_dma(photo_diode_t *config, uint32_t *buffer, uint32_t size)
{
	// Disable stream DMA before configure
	LL_DMA_DisableStream(config->dma, config->dma_stream_rx);
	while (LL_DMA_IsEnabledStream(config->dma, config->dma_stream_rx));

	//Config stream rx
	LL_DMA_SetMode(config->dma, config->dma_stream_rx, LL_DMA_MODE_CIRCULAR);
	LL_DMA_ConfigAddresses(	config->dma,
							config->dma_stream_rx,
							(uint32_t)&(config->spi->DR),
							(uint32_t)buffer,
							LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_SetDataLength(config->dma, config->dma_stream_rx, size);
	LL_DMA_SetMemoryIncMode(config->dma, config->dma_stream_rx, LL_DMA_MEMORY_INCREMENT);

	// Xóa toàn bộ ngắt cho DMA0 stream 0
	PHOTO_DMA->LIFCR = DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0;

	// Kích hoạt ngắt half-transfer và transfer-complete
	LL_DMA_EnableIT_HT(config->dma, config->dma_stream_rx);
	LL_DMA_EnableIT_TC(config->dma, config->dma_stream_rx);

	// Enable DMA Request từ SPI
	LL_SPI_EnableDMAReq_RX(config->spi);

	// Enable SPI (nếu chưa)
	LL_SPI_Enable(PHOTO_SPI);

	// Enable DMA stream
	LL_DMA_EnableStream(config->dma, config->dma_stream_rx);
}

void bsp_photodiode_sample_start()
{
	bsp_photodiode_adc_spi_change_mode();
	bsp_photo_prepare_timer_sampling();
	bsp_photodiode_time_t * timing = &photo_diode_adc.timing;
	uint32_t num_sample = ((timing->pre_time + timing->sampling_time + timing->post_time) * timing->sampling_rate) / 1000;
	if (num_sample % BUFFER_FULL_SIZE) photo_diode_adc.block_count = (num_sample / BUFFER_FULL_SIZE) + 1;
	else photo_diode_adc.block_count = (num_sample / BUFFER_FULL_SIZE);
	photo_diode_adc.ram_current_address = 0;

	experiment_task_inst.num_chunk = photo_diode_adc.block_count;
	experiment_task_inst.num_data_real = experiment_task_inst.num_chunk * BUFFER_FULL_SIZE;

	bsp_photodiode_start_dma(&photo_diode_adc, (uint32_t *)&photo_data_buffer, BUFFER_FULL_SIZE);
	bsp_photo_start_timer_sampling();
}


// Hàm xử lí DMA ADC
void bsp_photodiode_dma_sampling_irq(void)
{
//	TIM1->CR1 &= ~TIM_CR1_CEN;		// Stop timer trigger
//	PHOTO_ADC_CS_GPIO_Port->BSRR = PHOTO_ADC_CS_Pin;	// CS_HIGH

	// Half-transfer
	if (PHOTO_DMA->LISR & DMA_LISR_HTIF0)
	{
		PHOTO_DMA->LIFCR = DMA_LIFCR_CHTIF0;	// Clear Half-transfer flag
		bsp_spi_ram_write_dma(photo_diode_adc.ram_current_address, BUFFER_HALF_SIZE_BYTE, (uint8_t *)photo_data_buffer);
		photo_diode_adc.ram_current_address += BUFFER_HALF_SIZE_BYTE;
//		TIM1->CR1 |= TIM_CR1_CEN;		// Continue start timer trigger
	}

	// Transfer-complete
	else if (PHOTO_DMA->LISR & DMA_LISR_TCIF0)
	{
		PHOTO_DMA->LIFCR = DMA_LIFCR_CTCIF0;	// Clear Transfer-complete flag
		bsp_spi_ram_write_dma(photo_diode_adc.ram_current_address, BUFFER_HALF_SIZE_BYTE, (uint8_t *)upper_data_buffer);
		photo_diode_adc.ram_current_address += BUFFER_HALF_SIZE_BYTE;
		photo_diode_adc.block_count --;

//		TIM1->CR1 |= TIM_CR1_CEN;		// Continue start timer trigger

		if ((photo_diode_adc.block_count) == 0)
		{
			TIM1->CR1 &= ~TIM_CR1_CEN;
			TIM1->DIER &= ~TIM_DIER_UIE;
			TIM1->SR = ~TIM_SR_UIF;

//			TIM2->CR1 &= ~TIM_CR1_CEN;
//			TIM2->DIER &= ~TIM_DIER_UIE;
//			TIM2->SR = ~TIM_SR_UIF;

			NVIC_ClearPendingIRQ(TIM1_UP_TIM10_IRQn);

		    // Dừng DMA và SPI
		    LL_DMA_DisableIT_TC(PHOTO_DMA, PHOTO_DMA_STREAM);
		    LL_DMA_DisableIT_HT(PHOTO_DMA, PHOTO_DMA_STREAM);
		    LL_DMA_DisableStream(PHOTO_DMA, PHOTO_DMA_STREAM);
		    while (LL_DMA_IsEnabledStream(PHOTO_DMA, PHOTO_DMA_STREAM));  // đảm bảo đã disable

		    // Xoá cờ ngắt TC/HT cho DMA2 Stream 0
		    DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0;

		    LL_SPI_DisableDMAReq_RX(photo_diode_adc.spi);

			// Done finish post-sampling event trigger
			SST_Task_post((SST_Task *)&experiment_task_inst.super, (SST_Evt *)&finish_post_phase_evt);
		}

	}

	// Clear all unexpected interrupt flag (stream 0)
	else
	{
		// Xóa toàn bộ ngắt cho DMA0 stream 0
		PHOTO_DMA->LIFCR = DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0;
//		TIM1->CR1 |= TIM_CR1_CEN;
	}

}

// Hàm xử lý ngắt Timer ADC trigger
void TIM1_UP_TIM10_IRQHandler(void)
{
////	GPIOD->BSRR = GPIO_BSRR_BS_9;  	// CS HIGH
//	PHOTO_ADC_CS_GPIO_Port->BSRR = PHOTO_ADC_CS_Pin;
//	PHOTO_ADC_CS_GPIO_Port->BSRR = PHOTO_ADC_CS_Pin;
//
//	TIM1->SR = ~TIM_SR_UIF;			// Clear timer flag
//
////	GPIOD->BSRR = GPIO_BSRR_BR_10; 	// CV LOW
//	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin << 16;
//	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin << 16;	//50ns
//
////	GPIOD->BSRR = GPIO_BSRR_BR_9; 	// CS LOW
//	PHOTO_ADC_CS_GPIO_Port->BSRR = PHOTO_ADC_CS_Pin << 16;		//50ns
//
////	SPI2->DR = 0xAAAA;
//	PHOTO_SPI->DR = 0xAAAA;										//50ns
//
////	GPIOD->BSRR = GPIO_BSRR_BS_10; 	// CV HIGH
//	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin;		//50ns




	PHOTO_ADC_CS_GPIO_Port->BSRR = PHOTO_ADC_CS_Pin;  	// CS HIGH

	TIM1->SR = ~TIM_SR_UIF;			// Clear timer flag

	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin << 16;	// CV LOW
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin << 16;	// CV LOW

	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH
	PHOTO_ADC_CONV_GPIO_Port->BSRR = PHOTO_ADC_CONV_Pin; 	// CV HIGH

	PHOTO_ADC_CS_GPIO_Port->BSRR = PHOTO_ADC_CS_Pin << 16; 	// CS LOW

	PHOTO_SPI->DR = 0xAAAA;
}

void TIM2_IRQHandler(void)
{
	TIM2->SR = ~TIM_SR_UIF;	// Xóa cờ ngắt update
	switch (photo_diode_state)
	{
		case PHOTO_SAMPLED_PRE:
			// Switch on laser[pos] (Start sampling time)
			bsp_laser_int_switch_on(photo_diode_adc.timing.pos);

			photo_diode_state = PHOTO_SAMPLED_SAMPLING;
			// Set laser timer with sampling time
			TIM2->ARR = timer_timing.sampling_time_ARR - 1;

			// Done finish pre-sampling event trigger
			SST_Task_post((SST_Task *)&experiment_task_inst.super, (SST_Evt *)&finish_pre_phase_evt);

			// Enable timer2 counter
			TIM2->CR1 |= TIM_CR1_CEN;
		break;

		case PHOTO_SAMPLED_SAMPLING:
			// Switch off laser[pos] (Stop sampling time)
			bsp_laser_int_switch_off_all();

			// Disable timer2 counter
			TIM2->CR1 &= ~TIM_CR1_CEN;		// stop timer

			photo_diode_state = PHOTO_SAMPLING_STOP;

			// Done finish sampling event trigger
			SST_Task_post((SST_Task *)&experiment_task_inst.super, (SST_Evt *)&finish_sampling_phase_evt);

		break;
		default: break;
	}
}
