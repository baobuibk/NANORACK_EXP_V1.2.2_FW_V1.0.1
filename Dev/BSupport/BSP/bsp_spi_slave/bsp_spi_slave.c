/*
 * bsp_spi_slave.c
 *
 *  Created on: Jul 15, 2025
 *      Author: HTSANG
 */

#include "bsp_spi_slave.h"
#include "stm32f7xx_ll_spi.h"
#include "stm32f7xx_ll_dma.h"
#include "bsp_spi_ram.h"
#include "main.h"
#include "experiment_task.h"
#include "bsp_crc.h"


static SPI_SlaveDevice_t spi_device_instance = {
    .transfer_state = SPI_TRANSFER_WAIT,
	.data_context = {
		.p_tx_buffer = 0x00,
	    .crc = 0x0000,
	    .is_valid = false
	},
    .is_initialized = false
};

//static uint32_t CRC_HW_Calculation(uint8_t *data_buffer, uint32_t length)
//{
//    if (length == 0) return 0;
//    uint8_t* p_data = data_buffer;
//
//    CRC->CR = CRC_CR_RESET;
//
//    for (uint32_t i = 0; i < length; i ++)
//    {
//    	CRC->DR = *p_data;
//    	p_data ++ ;
//    }
//
//    return CRC->DR;
//}

SPI_SlaveDevice_t* SPI_SlaveDevice_GetHandle(void)
{
    return &spi_device_instance;
}

Std_ReturnType SPI_SlaveDevice_Init(uint16_t * p_tx_buffer)
{
	// Force reset SPI1
	LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_SPI1);
	// Release reset SPI1
	LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_SPI1);

	SPI1_ReInit();

    LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_3);
    LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_3);

    spi_device_instance.data_context.is_valid = false;
    spi_device_instance.is_initialized = true;
    spi_device_instance.transfer_state = SPI_TRANSFER_PREPARE;

    spi_device_instance.data_context.p_tx_buffer = p_tx_buffer;
	return E_OK;
}

Std_ReturnType SPI_SlaveDevice_CollectData(uint16_t * p_tx_buffer)
{
//    if (!spi_device_instance.is_initialized) {
//        return E_ERROR;
//    }

	SPI_SlaveDevice_Init(p_tx_buffer);

    uint32_t crc = 0x0000;
    crc = CRC_HW_Calculation((uint8_t *)spi_device_instance.data_context.p_tx_buffer, EXPERIMENT_BUFFER_BYTE_SIZE);

	spi_device_instance.data_context.crc = crc;
	spi_device_instance.data_context.is_valid = true;

	if (SPI_SlaveDevice_ReinitDMA() != E_OK) {
		return E_ERROR;
	}

    return E_OK;
}

Std_ReturnType SPI_SlaveDevice_GetDataInfo(DataProcessContext_t *context)
{
    if (!spi_device_instance.is_initialized || !spi_device_instance.data_context.is_valid) {
        return E_ERROR;
    }

    *context = spi_device_instance.data_context;
    return E_OK;
}

Std_ReturnType SPI_SlaveDevice_ReinitDMA()
{
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
    while (LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_3));

//    LL_SPI_DisableDMAReq_TX(SPI1);
//    LL_SPI_Disable(SPI1);
//
//    LL_DMA_ClearFlag_TC3(DMA2);
//    LL_DMA_ClearFlag_TE3(DMA2);

    LL_DMA_ConfigAddresses(DMA2, LL_DMA_STREAM_3, (uint32_t)spi_device_instance.data_context.p_tx_buffer,
    		LL_SPI_DMA_GetRegAddr(SPI1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_3, EXPERIMENT_BUFFER_BYTE_SIZE);

    // Enable DMA Request từ SPI
    LL_SPI_EnableDMAReq_TX(SPI1);

    // Enable SPI (nếu chưa)
    LL_SPI_Enable(SPI1);

    // Enable DMA stream
    LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_3);

    spi_device_instance.transfer_state = SPI_TRANSFER_WAIT;
    return E_OK;
}

Std_ReturnType SPI_SlaveDevice_Disable(void)
{
    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
    LL_SPI_DisableDMAReq_TX(SPI1);

    LL_DMA_ClearFlag_TC3(DMA2);
    LL_DMA_ClearFlag_TE3(DMA2);

    spi_device_instance.transfer_state = SPI_TRANSFER_WAIT;
    spi_device_instance.data_context.is_valid = false;
    return E_OK;
}

SPI_TransferState_t SPI_SlaveDevice_GetTransferState(void)
{
    return spi_device_instance.transfer_state;
}

void SPI_SlaveDevice_SetTransferState(SPI_TransferState_t state)
{
    spi_device_instance.transfer_state = state;
}

uint32_t SPI_SlaveDevide_GetDataCRC(void) {
	return spi_device_instance.data_context.crc;
}

void DMA2_Stream3_IRQHandler(void)
{
//	if (LL_DMA_IsActiveFlag_TC3(DMA2)) {
//		LL_DMA_ClearFlag_TC3(DMA2);
//		SPI_SlaveDevice_SetTransferState(SPI_TRANSFER_COMPLETE);
//		SPI_SlaveDevice_Disable();
//	}
//	if (LL_DMA_IsActiveFlag_TE3(DMA2)) {
//		LL_DMA_ClearFlag_TE3(DMA2);
//		SPI_SlaveDevice_SetTransferState(SPI_TRANSFER_ERROR);
//		SPI_SlaveDevice_Disable();
//	}

	if (DMA2->LISR & DMA_LISR_TCIF3)
	{
		DMA2->LIFCR = DMA_LIFCR_CTCIF3;
		SPI_SlaveDevice_SetTransferState(SPI_TRANSFER_COMPLETE);
		LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
		LL_SPI_DisableDMAReq_TX(SPI1);
	}
	// Clear all unexpected interrupt flag (stream 2)
	else
	{
		DMA2->LIFCR = DMA_LIFCR_CHTIF3;		// Clear Half-transfer flag
		DMA2->LIFCR = DMA_LIFCR_CTEIF3;		// Clear Transfer-error flag
		DMA2->LIFCR = DMA_LIFCR_CDMEIF3;	// Clear Direct-mode-error flag
		DMA2->LIFCR = DMA_LIFCR_CFEIF3;		// Clear FIFO-error flag

		SPI_SlaveDevice_SetTransferState(SPI_TRANSFER_ERROR);
		LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
		LL_SPI_DisableDMAReq_TX(SPI1);
	}
}
