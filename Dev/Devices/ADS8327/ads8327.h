/*
 * ads8327.h
 *
 *  Created on: Mar 2, 2025
 *      Author: DELL
 */

#ifndef ADS8327_H_
#define ADS8327_H_

#include "board.h"
#include <stdbool.h>


#define ADS8327_COMMAND_LENGTH		2		  // 2 byte
#define ADS8327_FACTORY_CFR_DEFAULT	0x0EFD

// Command Register
#define ADS8327_CMD_SELECT_CH0      (0x0000)  // Chọn kênh đầu vào 0
#define ADS8327_CMD_SELECT_CH1      (0x1000)  // Chọn kênh đầu vào 1
#define ADS8327_CMD_WAKE_UP    		(0xB000)  // Đánh thức
#define ADS8327_CMD_READ_CONFIG     (0xC000)  // Đọc cấu hình
#define ADS8327_CMD_READ_DATA    	(0xD000)  // Đọc dữ liệu
#define ADS8327_CMD_WRITE_CONFIG    (0xE000)  // Ghi cấu hình
#define ADS8327_CMD_DEFAULT_MODE    (0xF000)  // Chế độ mặc định

// Config Register
#define ADS8327_CFR_SEL_MODE		(11)
#define ADS8327_CFR_CLK_SRC			(10)
#define ADS8327_CFR_TRIG_SEL		(9)
#define ADS8327_CFR_EOC_MODE		(7)
#define ADS8327_CFR_EOC_INT			(6)
#define ADS8327_CFR_CHAIN_SEL		(5)
#define ADS8327_CFR_AUTO_NAP		(4)
#define ADS8327_CFR_NAP_PWR_DOWN	(3)
#define ADS8327_CFR_DEEP_PWR_DOWN	(2)
#define ADS8327_CFR_TAG_BIT			(1)
#define ADS8327_CFR_RESET			(0)

// Data Config
#define ADS8327_CFG_CHANNEL_MANN_MODE		(0)
#define ADS8327_CFG_CHANNEL_AUTO_MODE		(1)
#define ADS8327_CFG_CLK_EXT					(0)
#define ADS8327_CFG_CLK_INT					(1)
#define ADS8327_CFG_TRIG_AUTO				(0)
#define ADS8327_CFG_TRIG_MANN				(1)
#define ADS8327_CFG_EOC_HIGH				(0)
#define ADS8327_CFG_EOC_LOW					(1)
#define ADS8327_CFG_IO_SEL_INT				(0)
#define ADS8327_CFG_IO_SEL_EOC				(1)
#define ADS8327_CFG_CHAIN					(0)
#define ADS8327_CFG_NO_CHAIN				(1)
#define ADS8327_CFG_AUTO_NAP_EN				(0)
#define ADS8327_CFG_AUTO_NAP_DIS			(1)
#define ADS8327_CFG_NAP_PWR_DOWN_ACTIVE		(0)
#define ADS8327_CFG_NAP_PWR_DOWN_REMOVE		(1)
#define ADS8327_CFG_DEEP_PWR_DOWN_ACTIVE	(0)
#define ADS8327_CFG_DEEP_PWR_DOWN_REMOVE	(1)
#define ADS8327_CFG_TAG_BIT_DIS				(0)
#define ADS8327_CFG_TAG_BIT_EN				(1)
#define ADS8327_CFG_RESET					(0)
#define ADS8327_CFG_NORMAL					(1)


typedef enum {
	USER_DEFAULT,
	FACTORY_DEFAULT
}CFR_default_t;

typedef struct {
    SPI_TypeDef *spi;              // Peripheral SPI (SPI1, SPI2, v.v.)
    GPIO_TypeDef *cs_port;         // Port của chân CS
    uint32_t cs_pin;               // Chân CS
	GPIO_TypeDef *convst_port;     // Port của chân convst
	uint32_t convst_pin;           // Chân convst
	GPIO_TypeDef *EOC_port;        // Port của chân EOC
	uint32_t EOC_pin;              // Chân EOC
	uint16_t CMD;				   // Command
    uint16_t CFR_value;			   // Giá trị thanh ghi Config
    uint16_t ADC_val;			   // Giá trị ADC
    uint8_t tran_ind;
    uint8_t buf[2];
    uint32_t transfer_size;         // Kích thước truyền
    uint8_t transfer_done;          // Cờ hoàn thành
} ADS8327_Device_t;



//void ISR_SPI_IRQHandler(ADS8327_Device_t *dev);
//void ISR_TIMTrigger_IRQHandler(ADS8327_Device_t *dev);

void SPI_SetDataLength(SPI_TypeDef *spi, uint32_t DataWidth);
void SPI_SetPrescaler(SPI_TypeDef *spi, uint32_t Prescaler);
void ADS8327_Wake_Up(ADS8327_Device_t *dev);
void ADS8327_Write_CFR(ADS8327_Device_t *dev, uint16_t CFR);
void ADS8327_Default_CFR(ADS8327_Device_t *dev, CFR_default_t CFR_default);
uint16_t ADS8327_Read_Data_Polling(ADS8327_Device_t *dev, int timeout);
void ADS8327_Device_Init(	ADS8327_Device_t *dev);





#endif /* ADS8327_H_ */
