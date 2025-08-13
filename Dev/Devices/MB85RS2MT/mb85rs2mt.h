/*
 * mb85rs2mt.h
 *
 *  Created on: Mar 12, 2025
 *      Author: HTSANG
 */

#ifndef DEVICES_MB85RS2MT_MB85RS2MT_H_
#define DEVICES_MB85RS2MT_MB85RS2MT_H_

#include "stm32f7xx.h"
#include "main.h"

// SPI Command List for MB85RS2MTA
#define MB85RS2MT_CMD_WREN  	0x06  // Enable Write
#define MB85RS2MT_CMD_WRDI  	0x04  // Disable Write
#define MB85RS2MT_CMD_RDSR  	0x05  // Read Status
#define MB85RS2MT_CMD_WRSR  	0x01  // Write Status
#define MB85RS2MT_CMD_READ  	0x03  // Read Data
#define MB85RS2MT_CMD_WRITE 	0x02  // Write Data
#define MB85RS2MT_CMD_SLEEP 	0xB9  // Sleep Mode
#define MB85RS2MT_CMD_RDID  	0xAB  // Wake Up


typedef struct mb85rs2mt_dev
{
    SPI_TypeDef *hspi;				// Peripheral SPI (SPI1, SPI2, v.v.)
    GPIO_TypeDef *cs_port;			// Port của chân CS
    uint16_t cs_pin;				// Chân CS
} mb85rs2mt_dev_t;

void MB85RS2MT_Init(mb85rs2mt_dev_t *dev);
void MB85RS2MT_Read(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);
void MB85RS2MT_Write(mb85rs2mt_dev_t *dev, uint32_t address, uint8_t *buffer, uint16_t length);
uint8_t MB85RS2MT_ReadStatus(mb85rs2mt_dev_t *dev);
void MB85RS2MT_WriteStatus(mb85rs2mt_dev_t *dev, uint8_t status);
void MB85RS2MT_Sleep(mb85rs2mt_dev_t *dev);
void MB85RS2MT_Wakeup(mb85rs2mt_dev_t *dev);

#endif /* DEVICES_MB85RS2MT_MB85RS2MT_H_ */
