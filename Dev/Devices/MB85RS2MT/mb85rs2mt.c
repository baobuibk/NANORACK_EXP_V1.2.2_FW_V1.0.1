/*
 * mb85rs2mt.c
 *
 *  Created on: Mar 12, 2025
 *      Author: HTSANG
 */

#include "mb85rs2mt.h"

// Hàm hỗ trợ - Chọn FRAM
static inline void MB85RS2MT_Select(mb85rs2mt_dev_t *dev)
{
    LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
}

// Hàm hỗ trợ - Bỏ chọn FRAM
static inline void MB85RS2MT_Deselect(mb85rs2mt_dev_t *dev)
{
    LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}

// Gửi dữ liệu qua SPI (LL)
static void MB85RS2MT_SPI_Transmit(mb85rs2mt_dev_t *dev, uint8_t *data, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        while (!LL_SPI_IsActiveFlag_TXE(dev->hspi));
        LL_SPI_TransmitData8(dev->hspi, data[i]);
        while (!LL_SPI_IsActiveFlag_RXNE(dev->hspi));
        LL_SPI_ReceiveData8(dev->hspi); // Đọc dữ liệu rác từ SPI
    }
}

// Nhận dữ liệu qua SPI (LL)
static void MB85RS2MT_SPI_Receive(mb85rs2mt_dev_t *dev, uint8_t *data, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        while (!LL_SPI_IsActiveFlag_TXE(dev->hspi));
        LL_SPI_TransmitData8(dev->hspi, 0xFF); // Gửi dummy byte
        while (!LL_SPI_IsActiveFlag_RXNE(dev->hspi));
        data[i] = LL_SPI_ReceiveData8(dev->hspi);
    }
}

// Khởi tạo FRAM
void MB85RS2MT_Init(mb85rs2mt_dev_t *dev)
{
    MB85RS2MT_Deselect(dev);  // CS mặc định ở mức cao
}

// Đọc dữ liệu từ FRAM
void MB85RS2MT_Read(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length)
{
    uint8_t cmd[4];
    cmd[0] = MB85RS2MT_CMD_READ;
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, cmd, 4);
    MB85RS2MT_SPI_Receive(dev, buffer, length);
    MB85RS2MT_Deselect(dev);
}

// Ghi dữ liệu vào FRAM
void MB85RS2MT_Write(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length)
{
    uint8_t cmd[4];
    cmd[0] = MB85RS2MT_CMD_WRITE;
    cmd[1] = (address >> 16) & 0xFF;
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF;

    // Cho phép ghi
    uint8_t wren = MB85RS2MT_CMD_WREN;
    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, &wren, 1);
    MB85RS2MT_Deselect(dev);

    // Ghi dữ liệu
    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, cmd, 4);
    MB85RS2MT_SPI_Transmit(dev, buffer, length);
    MB85RS2MT_Deselect(dev);
}

// Đọc thanh ghi trạng thái
uint8_t MB85RS2MT_ReadStatus(mb85rs2mt_dev_t *dev)
{
    uint8_t cmd = MB85RS2MT_CMD_RDSR;
    uint8_t status = 0;

    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, &cmd, 1);
    MB85RS2MT_SPI_Receive(dev, &status, 1);
    MB85RS2MT_Deselect(dev);

    return status;
}

// Ghi thanh ghi trạng thái
void MB85RS2MT_WriteStatus(mb85rs2mt_dev_t *dev, uint8_t status)
{
    uint8_t cmd[2] = {MB85RS2MT_CMD_WRSR, status};

    // Cho phép ghi
    uint8_t wren = MB85RS2MT_CMD_WREN;
    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, &wren, 1);
    MB85RS2MT_Deselect(dev);

    // Ghi thanh ghi trạng thái
    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, cmd, 2);
    MB85RS2MT_Deselect(dev);
}

// Đưa FRAM vào chế độ ngủ
void MB85RS2MT_Sleep(mb85rs2mt_dev_t *dev)
{
    uint8_t cmd = MB85RS2MT_CMD_SLEEP;

    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, &cmd, 1);
    MB85RS2MT_Deselect(dev);
}

// Đánh thức FRAM khỏi chế độ ngủ
void MB85RS2MT_Wakeup(mb85rs2mt_dev_t *dev)
{
    uint8_t cmd = MB85RS2MT_CMD_RDID;
    uint8_t dummy = 0;

    MB85RS2MT_Select(dev);
    MB85RS2MT_SPI_Transmit(dev, &cmd, 1);
    MB85RS2MT_SPI_Receive(dev, &dummy, 1);
    MB85RS2MT_Deselect(dev);
}
