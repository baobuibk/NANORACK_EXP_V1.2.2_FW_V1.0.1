/*
 * i2c.h
 *
 *  Created on: Nov 29, 2024
 *      Author: SANG HUYNH
 */

#ifndef I2C_I2C_H_
#define I2C_I2C_H_

#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_i2c.h"

#define I2C_TIMEOUT 5000 // Timeout value in milliseconds
typedef enum {I2C_Success = 0, I2C_Error} I2C_Status_t;

I2C_Status_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t Addr, uint8_t reg, uint8_t data);
I2C_Status_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t *pData);
I2C_Status_t I2C_ReadMulti(I2C_TypeDef *I2Cx, uint8_t addr, uint8_t reg, uint8_t* pData, uint16_t size);

#endif /* I2C_I2C_H_ */
