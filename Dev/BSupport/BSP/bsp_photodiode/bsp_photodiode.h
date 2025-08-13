/*
 * bsp_photodiode.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Admin
 */

#ifndef BSUPPORT_BSP_BSP_PHOTODIODE_BSP_PHOTODIODE_H_
#define BSUPPORT_BSP_BSP_PHOTODIODE_BSP_PHOTODIODE_H_

#include "board.h"
#include "adg1414.h"
#include "main.h"


typedef struct bsp_photodiode_time_t
{
	uint32_t pre_time; //in us
	uint32_t sampling_time;
	uint32_t post_time;
	uint32_t sampling_rate; //in Khz
	uint32_t pos;
}bsp_photodiode_time_t;

typedef enum {PHOTO_SAMPLING_START, PHOTO_SAMPLED_PRE, PHOTO_SAMPLED_SAMPLING, PHOTO_SAMPLING_STOP} photo_state_t;

typedef struct photo_diode_t{
    SPI_TypeDef *spi;               // Instance SPI (SPI2)
    GPIO_TypeDef *cs_port;          // Cổng CS (GPIOD)
    uint32_t cs_pin;                // Chân CS (PD7)
    DMA_TypeDef *dma;               // Instance DMA (DMA2)
    uint32_t dma_stream_rx;         // Stream RX (Stream 6)
    uint32_t ram_current_address;
    uint32_t block_count;
    bsp_photodiode_time_t timing;	//contain the timing
} photo_diode_t;

void bsp_photodiode_set_spi_mode(spi_mode_t spi_mode);
void bsp_photodiode_set_spi_data_len(uint32_t DataWidth);
void bsp_photodiode_set_spi_prescaler(uint32_t Prescaler);
void bsp_photodiode_sw_spi_change_mode(void);
void bsp_photodiode_adc_spi_change_mode(void);

void bsp_photodiode_init(void);
void bsp_photo_set_time(bsp_photodiode_time_t * init_photo_time);
void bsp_photo_switch_on(uint32_t channel_idx);
void bsp_photo_switch_off_all(void);

void bsp_photodiode_sample_start(void);

void bsp_photodiode_dma_sampling_irq(void);


extern ADS8327_Device_t photo_adc;
#endif /* BSUPPORT_BSP_BSP_PHOTODIODE_BSP_PHOTODIODE_H_ */
