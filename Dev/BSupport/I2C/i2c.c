/*
 * i2c.c
 *
 *  Created on: Nov 29, 2024
 *      Author: SANG HUYNH
 */

#include "i2c.h"


I2C_Status_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t data)
{
    uint32_t timeout = I2C_TIMEOUT;

    // Đảm bảo I2C không bận
    while ((LL_I2C_IsActiveFlag_BUSY(I2Cx)) && timeout--);
    if (timeout == 0) return I2C_Error;

    // Cấu hình CR2: địa chỉ, số byte (2: reg + data), mode write, start
    LL_I2C_HandleTransfer(I2Cx, addr << 1, LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    // Gửi byte 1: reg
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXIS(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_TransmitData8(I2Cx, reg);

    // Gửi byte 2: data
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXIS(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;
    LL_I2C_TransmitData8(I2Cx, data);

    // Chờ stop
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_STOP(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // Xóa cờ stop
    LL_I2C_ClearFlag_STOP(I2Cx);

    return I2C_Success;
}

I2C_Status_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t* pData)
{
    uint32_t timeout = I2C_TIMEOUT;

    // Đảm bảo I2C không bận
    while (LL_I2C_IsActiveFlag_BUSY(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    // Bước 1: Gửi địa chỉ thanh ghi cần đọc (giai đoạn write)
    LL_I2C_HandleTransfer(I2Cx, addr << 1, LL_I2C_ADDRSLAVE_7BIT, 1,
                          LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXIS(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_TransmitData8(I2Cx, reg);

    // Bước 2: Khởi động đọc 1 byte từ thiết bị
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TC(I2Cx) && timeout--); // Chờ kết thúc truyền trước
    if (timeout == 0) return I2C_Error;

    LL_I2C_HandleTransfer(I2Cx, addr << 1, LL_I2C_ADDRSLAVE_7BIT, 1,
                          LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

    // Bước 3: Nhận dữ liệu
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_RXNE(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    *pData = LL_I2C_ReceiveData8(I2Cx);

    // Bước 4: Chờ STOP
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_STOP(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_ClearFlag_STOP(I2Cx);
    return I2C_Success;
}

I2C_Status_t I2C_ReadMulti(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t* pData, uint16_t size)
{
    uint32_t timeout = I2C_TIMEOUT;

    // Bước 1: Gửi địa chỉ thanh ghi (phase ghi)
    while (LL_I2C_IsActiveFlag_BUSY(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_HandleTransfer(I2Cx, addr << 1, LL_I2C_ADDRSLAVE_7BIT, 1,
                          LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TXIS(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_TransmitData8(I2Cx, reg);

    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_TC(I2Cx) && timeout--);  // Chờ truyền xong thanh ghi
    if (timeout == 0) return I2C_Error;

    // Bước 2: Đọc nhiều byte
    LL_I2C_HandleTransfer(I2Cx, addr << 1, LL_I2C_ADDRSLAVE_7BIT, size,
                          LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

    for (uint16_t i = 0; i < size; i++) {
        timeout = I2C_TIMEOUT;
        while (!LL_I2C_IsActiveFlag_RXNE(I2Cx) && timeout--);
        if (timeout == 0) return I2C_Error;

        pData[i] = LL_I2C_ReceiveData8(I2Cx);
    }

    // Bước 3: Chờ Stop
    timeout = I2C_TIMEOUT;
    while (!LL_I2C_IsActiveFlag_STOP(I2Cx) && timeout--);
    if (timeout == 0) return I2C_Error;

    LL_I2C_ClearFlag_STOP(I2Cx);
    return I2C_Success;
}
