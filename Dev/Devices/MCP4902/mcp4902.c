/*
 * mcp4902.c
 *
 *  Tạo vào: 2 tháng 3, 2025
 *      Tác giả: DELL
 */

#include "mcp4902.h"
#include "stm32f7xx_ll_spi.h"

/*
 * Chuyển đổi giá trị điện áp (milivolt) sang giá trị DAC (Bộ chuyển đổi số sang analog).
 * @param voltage: Giá trị điện áp đầu vào (đơn vị: milivolt).
 * @return: Giá trị DAC 8-bit được tính dựa trên điện áp tham chiếu (_VREF_DAC).
 */
uint8_t v2dac(uint16_t voltage)
{

	return (voltage * 255.0f)/_VREF_DAC;
}

/*
 * Chuyển đổi giá trị DAC sang giá trị điện áp (milivolt).
 * @param dac: Giá trị DAC 8-bit.
 * @return: Giá trị điện áp (đơn vị: milivolt) dựa trên điện áp tham chiếu (_VREF_DAC).
 */
uint16_t dac2v(uint8_t dac)
{
	return (dac*_VREF_DAC)/255.0f;
}

/*
 * Gửi dữ liệu đến MCP4902 DAC qua giao thức SPI cho cả hai kênh.
 * @param dev: Con trỏ đến cấu trúc thiết bị MCP4902 chứa cấu hình và dữ liệu.
 */
static void MCP4902_Write(MCP4902_Device_t *dev)
{
	uint16_t temp;

    while (!LL_SPI_IsActiveFlag_TXE(dev->spi));

    for (int i = 0; i < MCP4902_NUM_CHANNEL; i++)
    {
    	temp = i ? ((1<<MCP4902_AB_BIT)|(1<<MCP4902_GA_BIT)|(1<<MCP4902_SHDN_BIT)|(dev->dac_channel[i]<<4)):
				   ((1<<MCP4902_GA_BIT)|(1<<MCP4902_SHDN_BIT)|(dev->dac_channel[i]<<4));

    	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

        while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
        LL_SPI_TransmitData8(dev->spi, (uint8_t)(temp>>8));

        while (!LL_SPI_IsActiveFlag_RXNE(dev->spi)) {};

		LL_SPI_ReceiveData8(dev->spi);

		while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
        LL_SPI_TransmitData8(dev->spi, (uint8_t)temp);

        while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
        LL_SPI_ReceiveData8(dev->spi);

        LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);

		LL_GPIO_ResetOutputPin(dev->latch_port, dev->latch_pin);
		__NOP();
		LL_GPIO_SetOutputPin(dev->latch_port, dev->latch_pin);
    }
}

/*
 * Đưa kênh DAC được chỉ định vào chế độ tắt.
 * @param dev: Con trỏ đến cấu trúc thiết bị MCP4902.
 * @param channel: Kênh DAC cần tắt (0 cho CHA, 1 cho CHB).
 */
void MCP4902_Shutdown(MCP4902_Device_t *dev, uint8_t channel)
{
	uint16_t temp = channel ? ((1<<MCP4902_AB_BIT)|(1<<MCP4902_GA_BIT)) : (1<<MCP4902_GA_BIT);

	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);

	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
	LL_SPI_TransmitData8(dev->spi, (uint8_t)(temp>>8));

	while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
	LL_SPI_ReceiveData8(dev->spi);

	while (!LL_SPI_IsActiveFlag_TXE(dev->spi));
	LL_SPI_TransmitData8(dev->spi, (uint8_t)temp);

	while (!LL_SPI_IsActiveFlag_RXNE(dev->spi));
	LL_SPI_ReceiveData8(dev->spi);

	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);

    LL_GPIO_ResetOutputPin(dev->latch_port, dev->latch_pin);
	__NOP();
	LL_GPIO_SetOutputPin(dev->latch_port, dev->latch_pin);
}

/*
 * Thiết lập giá trị DAC cho kênh được chỉ định.
 * @param dev: Con trỏ đến cấu trúc thiết bị MCP4902.
 * @param channel: Kênh DAC (0 cho CHA, 1 cho CHB).
 * @param DAC_val: Giá trị DAC 8-bit cần thiết lập.
 */
void MCP4902_Set_DAC(MCP4902_Device_t *dev, uint8_t channel, uint8_t DAC_val)
{
	dev->dac_channel[channel] = DAC_val;
	MCP4902_Write(dev);
}

/*
 * Thiết lập điện áp cho kênh DAC được chỉ định.
 * @param dev: Con trỏ đến cấu trúc thiết bị MCP4902.
 * @param channel: Kênh DAC (0 cho CHA, 1 cho CHB).
 * @param voltage: Giá trị điện áp (đơn vị: milivolt).
 */
void MCP4902_Set_Voltage(MCP4902_Device_t *dev, uint8_t channel, uint16_t voltage)
{
	dev->dac_channel[channel] = v2dac(voltage);
	MCP4902_Write(dev);
}

/*
 * Khởi tạo thiết bị MCP4902.
 * @param dev: Con trỏ đến cấu trúc thiết bị MCP4902.
 * @param spi: Con trỏ đến giao diện SPI được sử dụng.
 * @param cs_port: Cổng GPIO cho chân Chip Select.
 * @param cs_pin: Chân GPIO cho Chip Select.
 * @param latch_port: Cổng GPIO cho chân Latch.
 * @param latch_pin: Chân GPIO cho Latch.
 */
void MCP4902_Device_Init(	MCP4902_Device_t *dev,
							SPI_TypeDef *spi,
							GPIO_TypeDef *cs_port,
							uint32_t cs_pin,
							GPIO_TypeDef *latch_port,
							uint32_t latch_pin)
{
	dev->spi = spi;
	dev->cs_port = cs_port;
	dev->cs_pin = cs_pin;
	dev->latch_port = latch_port;
	dev->latch_pin = latch_pin;

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = dev->cs_pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(dev->cs_port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = dev->latch_pin;
	LL_GPIO_Init(dev->latch_port, &GPIO_InitStruct);

	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
	LL_GPIO_SetOutputPin(dev->latch_port, dev->latch_pin);

	for (int i = 0; i < MCP4902_NUM_CHANNEL; i++)
	{
		dev->dac_channel[i] = 0;
	}

	while (!LL_SPI_IsEnabled(dev->spi))
	{
		LL_SPI_Enable(dev->spi);
		__NOP();
	}

	MCP4902_Write(dev);
	MCP4902_Shutdown(dev, MCP4902_CHA);
	MCP4902_Shutdown(dev, MCP4902_CHB);
}
