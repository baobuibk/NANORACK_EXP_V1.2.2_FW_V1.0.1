/*
 * bsp_spi_slave.h
 *
 *  Created on: Jul 15, 2025
 *      Author: HTSANG
 */

#ifndef BSUPPORT_BSP_BSP_SPI_SLAVE_BSP_SPI_SLAVE_H_
#define BSUPPORT_BSP_BSP_SPI_SLAVE_BSP_SPI_SLAVE_H_

#include "board.h"
#include "basetypedef.h"
#include "stdbool.h"
#include "stdint.h"
#include "uart_dbg.h"

#define BSP_SPI_DEBUG_PRINTING

#ifdef BSP_SPI_DEBUG_PRINTING
    #define bsp_spi_debug_print(...) DBG(0,__VA_ARGS__)
#else
	#define bsp_spi_debug_print(...)
#endif

typedef enum {
	SPI_TRANSFER_PREPARE,
	SPI_TRANSFER_WAIT,
	SPI_TRANSFER_COMPLETE,
	SPI_TRANSFER_ERROR
} SPI_TransferState_t;

typedef struct {
	uint16_t * p_tx_buffer;
	uint32_t crc; // CRC16-XMODEM of the data
	_Bool is_valid; // Flag indicating if context is valid
} DataProcessContext_t;

typedef struct {
	SPI_TransferState_t transfer_state;
	DataProcessContext_t data_context;
	_Bool is_initialized;
} SPI_SlaveDevice_t;

SPI_SlaveDevice_t* SPI_SlaveDevice_GetHandle(void);
Std_ReturnType SPI_SlaveDevice_Init(uint16_t * p_tx_buffer);
Std_ReturnType SPI_SlaveDevice_CollectData(uint16_t * p_tx_buffer);
Std_ReturnType SPI_SlaveDevice_GetDataInfo(DataProcessContext_t *context);
Std_ReturnType SPI_SlaveDevice_ReinitDMA(void);
Std_ReturnType SPI_SlaveDevice_Disable(void);
SPI_TransferState_t SPI_SlaveDevice_GetTransferState(void);
void SPI_SlaveDevice_SetTransferState(SPI_TransferState_t state);
uint32_t SPI_SlaveDevide_GetDataCRC(void);

#endif /* BSUPPORT_BSP_BSP_SPI_SLAVE_BSP_SPI_SLAVE_H_ */
