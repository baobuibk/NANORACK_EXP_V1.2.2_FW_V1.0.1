#include "is66wvs4m8bll.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_dma.h"

static uint8_t data_dummy = 0xAA;

// Hàm khởi tạo SRAM
void SRAM_Initialize(IS66_t *config)
{
	LL_DMA_DisableStream(config->dma, config->dma_stream_tx);
	LL_DMA_DisableStream(config->dma, config->dma_stream_rx);

	LL_DMA_SetMode(config->dma, config->dma_stream_tx, LL_DMA_MODE_NORMAL);
	LL_DMA_SetMode(config->dma, config->dma_stream_rx, LL_DMA_MODE_NORMAL);

	LL_DMA_SetPeriphAddress(config->dma, config->dma_stream_tx, (uint32_t)&(config->spi->DR));
	LL_DMA_SetPeriphAddress(config->dma, config->dma_stream_rx, (uint32_t)&(config->spi->DR));


	if (LL_DMA_IsActiveFlag_TC6(config->dma))
	{
		LL_DMA_ClearFlag_TC6(config->dma);
	}
	if (LL_DMA_IsActiveFlag_TC5(config->dma))
	{
		LL_DMA_ClearFlag_TC5(config->dma);
	}

	LL_DMA_EnableIT_TC(config->dma, config->dma_stream_rx);		// Kích hoạt ngắt DMA hoàn tất (cho RX)
}


void SRAM_read_id(IS66_t *config, uint8_t *buffer)
{
	uint32_t i;
    uint8_t cmd[4] = {SRAM_READ_ID_CMD, 0, 0, 0};
    LL_GPIO_ResetOutputPin(config->cs_port, config->cs_pin); // CS LOW
    for (i = 0; i < 4; i++) {
        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
        LL_SPI_TransmitData8(config->spi, cmd[i]);
        while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
        LL_SPI_ReceiveData8(config->spi); // Đọc bỏ dummy
    }
    for (i = 0; i < 8; i++) {
        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
        LL_SPI_TransmitData8(config->spi, 0xAA);
        while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
        buffer[i] = LL_SPI_ReceiveData8(config->spi);
    }
    LL_GPIO_SetOutputPin(config->cs_port, config->cs_pin); // CS HIGH

}

// Hàm ghi dữ liệu vào SRAM với DMA
void SRAM_write_polling(IS66_t *config, uint32_t address, uint32_t size, uint8_t *buffer) {

	uint32_t i;
	uint8_t cmd[4] = {SRAM_WRITE_CMD, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};
	LL_GPIO_SetOutputPin(config->cs_port, config->cs_pin); // make sure CS is high
	LL_GPIO_ResetOutputPin(config->cs_port, config->cs_pin); // CS LOW
	    for (i = 0; i < 4; i++) {
	        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
	        LL_SPI_TransmitData8(config->spi, cmd[i]);
	        while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
	        LL_SPI_ReceiveData8(config->spi); // Đọc bỏ dummy
	    }
	    for (i = 0; i < size; i++) {
	        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
	        LL_SPI_TransmitData8(config->spi, buffer[i]);
	        while (!LL_SPI_IsActiveFlag_RXNE(config->spi)){};
	         LL_SPI_ReceiveData8(config->spi);
	    }
	    LL_GPIO_SetOutputPin(config->cs_port, config->cs_pin); // CS HIGH
}

void SRAM_read_polling(IS66_t *config, uint32_t address, uint32_t size, uint8_t *buffer) {

	uint32_t i;
	uint8_t cmd[4] = {SRAM_READ_CMD, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF};
	LL_GPIO_SetOutputPin(config->cs_port, config->cs_pin); // make sure CS is high
	LL_GPIO_ResetOutputPin(config->cs_port, config->cs_pin); // CS LOW
	    for (i = 0; i < 4; i++) {
	        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
	        LL_SPI_TransmitData8(config->spi, cmd[i]);
	        while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
	        LL_SPI_ReceiveData8(config->spi); // Đọc bỏ dummy
	    }
	    for (i = 0; i < size; i++) {
	        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
	        LL_SPI_TransmitData8(config->spi, 0xAA);
	        while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
	        buffer[i] = LL_SPI_ReceiveData8(config->spi);
	    }
	    LL_GPIO_SetOutputPin(config->cs_port, config->cs_pin); // CS HIGH
}


void SRAM_fast_read_polling(IS66_t *config, uint32_t address, uint32_t size, uint8_t *buffer) {

	uint32_t i;
	uint8_t cmd[5] = {SRAM_FAST_READ_CMD, (address >> 16) & 0xFF, (address >> 8) & 0xFF, address & 0xFF,0};
	LL_GPIO_SetOutputPin(config->cs_port, config->cs_pin); // make sure CS is high
	LL_GPIO_ResetOutputPin(config->cs_port, config->cs_pin); // CS LOW
	    for (i = 0; i < 5; i++) {
	        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
	        LL_SPI_TransmitData8(config->spi, cmd[i]);
	        while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
	        LL_SPI_ReceiveData8(config->spi); // Đọc bỏ dummy
	    }
	    for (i = 0; i < size; i++) {
	        while (!LL_SPI_IsActiveFlag_TXE(config->spi));
	        LL_SPI_TransmitData8(config->spi, 0xAA);
	        while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
	        buffer[i] = LL_SPI_ReceiveData8(config->spi);
	    }
	    LL_GPIO_SetOutputPin(config->cs_port, config->cs_pin); // CS HIGH
}




/*
 * Ghi hoặc đọc dữ liệu SRAM sử dụng DMA qua SPI.
 * @param config: Con trỏ đến cấu trúc thiết bị IS66 chứa thông tin SPI, DMA và chân CS.
 * @param address: Địa chỉ trong SRAM để đọc/ghi dữ liệu.
 * @param size: Kích thước dữ liệu cần đọc/ghi (đơn vị: byte).
 */
void SRAM_DMA_Transfer(IS66_t *config, uint32_t size)
{
	LL_DMA_DisableStream(config->dma, config->dma_stream_tx);
	LL_DMA_DisableStream(config->dma, config->dma_stream_rx);

	LL_DMA_SetDataLength(config->dma, config->dma_stream_tx, size);
	LL_DMA_SetDataLength(config->dma, config->dma_stream_rx, size);

    // Stream TX:
    LL_DMA_SetMemoryAddress(config->dma, config->dma_stream_tx,
        (config->sram_mode == SRAM_MODE_WRITE) ? (uint32_t)config->buffer : (uint32_t)&data_dummy);
    LL_DMA_SetMemoryIncMode(config->dma, config->dma_stream_tx,
        (config->sram_mode == SRAM_MODE_WRITE) ? LL_DMA_MEMORY_INCREMENT : LL_DMA_MEMORY_NOINCREMENT);
    // Stream RX:
    LL_DMA_SetMemoryAddress(config->dma, config->dma_stream_rx,
        (config->sram_mode == SRAM_MODE_WRITE) ? (uint32_t)&data_dummy : (uint32_t)config->buffer);
    LL_DMA_SetMemoryIncMode(config->dma, config->dma_stream_rx,
        (config->sram_mode == SRAM_MODE_WRITE) ? LL_DMA_MEMORY_NOINCREMENT : LL_DMA_MEMORY_INCREMENT);


	uint8_t cmd[5] = {config->sram_mode == SRAM_MODE_WRITE ? SRAM_WRITE_CMD : SRAM_FAST_READ_CMD,
	                      (config->address >> 16) & 0xFF, (config->address >> 8) & 0xFF, config->address & 0xFF, 0};
	uint8_t cmd_len = config->sram_mode == SRAM_MODE_WRITE ? 4 : 5;

	config->cs_port->BSRR = config->cs_pin << 16;				// CS_LOW

	for (uint8_t i = 0; i < cmd_len; i++)
	{
		while (!LL_SPI_IsActiveFlag_TXE(config->spi));
		LL_SPI_TransmitData8(config->spi, cmd[i]);
		while (!LL_SPI_IsActiveFlag_RXNE(config->spi));
		LL_SPI_ReceiveData8(config->spi);
	}

	config->remain_size -= size;
	config->address += size;
	config->prev_size = size;

	// Kích hoạt DMA
	LL_SPI_EnableDMAReq_TX(config->spi);
	LL_SPI_EnableDMAReq_RX(config->spi);
	LL_DMA_EnableStream(config->dma, config->dma_stream_rx); 	// RX trước
	LL_DMA_EnableStream(config->dma, config->dma_stream_tx); 	// TX sau

}



// Hàm kiểm tra trạng thái truyền
uint8_t SRAM_IsTransferDone(IS66_t *config)
{
    return config->transfer_done;
}
