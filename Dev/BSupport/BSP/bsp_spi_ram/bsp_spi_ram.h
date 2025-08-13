/*
 * bsp_spi_ram.h
 *
 *  Created on: Jun 26, 2025
 *      Author: Admin
 */

#ifndef BSP_SPI_RAM_H_
#define BSP_SPI_RAM_H_

#include "board.h"

void bsp_spi_ram_init(void);
void bsp_spi_ram_write_polling(uint32_t address, uint32_t size, uint8_t *buffer);
void bsp_spi_ram_read_polling(uint32_t address, uint32_t size, uint8_t *buffer);
void bsp_spi_ram_read_id(uint8_t * buffer);
void bsp_spi_ram_fast_read_polling(uint32_t address, uint32_t size, uint8_t *buffer);
void bsp_spi_ram_write_dma(uint32_t address, uint32_t size, uint8_t *buffer);
void bsp_spi_ram_read_dma(uint32_t address, uint32_t size, uint8_t *buffer);
uint8_t bsp_spi_ram_is_transfer_done(void);
#endif /* BSUPPORT_BSP_BSP_SPI_RAM_BSP_SPI_RAM_H_ */
