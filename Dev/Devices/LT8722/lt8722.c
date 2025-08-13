/*
 * lt8722.c
 *
 *  Created on: Nov 27, 2024
 *      Author: SANG HUYNH
 */

#include "lt8722.h"
#include "board.h"
#include "main.h"


struct lt8722_reg lt8722_regs[LT8722_NUM_REGISTERS] = {
	{
		LT8722_SPIS_COMMAND, LT8722_SPIS_COMMAND_DEFAULT_VALUE,
		LT8722_SPIS_COMMAND_SIZE
	},
	{
		LT8722_SPIS_STATUS, LT8722_SPIS_STATUS_DEFAULT_VALUE,
		LT8722_SPIS_STATUS_SIZE
	},
	{
		LT8722_SPIS_DAC_ILIMN, LT8722_SPIS_DAC_ILIMN_DEFAULT_VALUE,
		LT8722_SPIS_DAC_ILIMN_SIZE
	},
	{
		LT8722_SPIS_DAC_ILIMP, LT8722_SPIS_DAC_ILIMP_DEFAULT_VALUE,
		LT8722_SPIS_DAC_ILIMP_SIZE
	},
	{
		LT8722_SPIS_DAC, LT8722_SPIS_DAC_DEFAULT_VALUE,
		LT8722_SPIS_DAC_SIZE
	},
	{
		LT8722_SPIS_OV_CLAMP, LT8722_SPIS_OV_CLAMP_DEFAULT_VALUE,
		LT8722_SPIS_OV_CLAMP_SIZE
	},
	{
		LT8722_SPIS_UV_CLAMP, LT8722_SPIS_UV_CLAMP_DEFAULT_VALUE,
		LT8722_SPIS_UV_CLAMP_SIZE
	},
	{
		LT8722_SPIS_AMUX, LT8722_SPIS_AMUX_DEFAULT_VALUE,
		LT8722_SPIS_AMUX_SIZE
	},
};

/* SPI support function --------------------------------------------------*/
static inline void csLOW(struct lt8722_dev *dev) {
	LL_GPIO_ResetOutputPin(dev->cs_port, dev->cs_pin);
}

static inline void csHIGH(struct lt8722_dev *dev) {
	LL_GPIO_SetOutputPin(dev->cs_port, dev->cs_pin);
}

static uint8_t SPI_LL_Transmit(struct lt8722_dev *dev, uint8_t data)
{
	LL_SPI_TransmitData8(SPI_TEC, data);
	while(!LL_SPI_IsActiveFlag_RXNE(dev->hspi));
	return LL_SPI_ReceiveData8(SPI_TEC);
}

uint8_t SPI_write_and_read_buffer(struct lt8722_dev *dev, uint8_t *buffer, uint8_t byte_number)
{
    uint8_t received_data = 0;
    csLOW(dev);
    for (uint8_t i = 0; i < byte_number; i++)
    {
        received_data = SPI_LL_Transmit(dev, buffer[i]);
        buffer[i] = received_data;
    }
    csHIGH(dev);
    return received_data;
}

/* Private support function prototype -----------------------------------*/
uint8_t Calculate_CRC8(uint8_t *data, uint8_t length);
void put_unaligned_be32(uint32_t val, uint8_t *buf);
void put_unaligned_be16(uint16_t val, uint8_t *buf);
uint32_t get_unaligned_be32(uint8_t *buf);
uint32_t get_unaligned_be16(uint8_t *buf);
uint32_t find_first_set_bit(uint32_t word);
uint32_t field_prep(uint32_t mask, uint32_t val);
uint32_t field_get(uint32_t mask, uint32_t word);


/**
 * @brief Convert voltage to DAC code.
 * @param voltage - Voltage value in nanovolts.
 * @return DAC code.
 */
int32_t lt8722_voltage_to_dac(int64_t voltage)
{
	return (LT8722_DAC_OFFSET - voltage) * (1 << LT8722_DAC_RESOLUTION) / LT8722_DAC_VREF;
}

/**
 * @brief Convert DAC code to nanovolts.
 * @param dac - DAC code.
 * @return Voltage value in nanovolts.
 */
int64_t lt8722_dac_to_voltage(int32_t dac)
{
	return LT8722_DAC_OFFSET - dac * LT8722_DAC_VREF /(1 << LT8722_DAC_RESOLUTION);
}

/**
 * @brief LT8722 device SPI transaction.
 * @param dev - LT8722 device descriptor
 * @param packet - LT8722 packet.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_transaction(struct lt8722_dev *dev, struct lt8722_packet *packet)
{
	uint8_t buffer[8] = {0};
	buffer[0] = packet->command.byte;
	buffer[1] = packet->reg.address << 1;
	if (packet->command.byte == LT8722_DATA_WRITE_COMMAND)
	{
		put_unaligned_be32(packet->data, &buffer[2]);
		buffer[6] = Calculate_CRC8(buffer, 6);
	} else
		buffer[2] = Calculate_CRC8(buffer, 2);
	SPI_write_and_read_buffer(dev, buffer, packet->command.size);
	packet->status = (get_unaligned_be16(&buffer[0]) & GENMASK(10, 0));
	if (packet->command.byte == LT8722_DATA_WRITE_COMMAND)
	{
		packet->crc = buffer[2];
		packet->ack = buffer[7];
	} else if (packet->command.byte == LT8722_DATA_READ_COMMAND) {
		packet->data = get_unaligned_be32(&buffer[2]);
		packet->crc = buffer[6];
		packet->ack = buffer[7];
	} else {
		packet->crc = buffer[2];
		packet->ack = buffer[3];
	}
	if (packet->ack != LT8722_ACK_ACKNOWLEDGE)
	{
		dev->status |= (1 << TEC_FAULT_POS); //communication fault
		return -1;
	}
	dev->status &= ~(1 << TEC_FAULT_POS); //communication OK
	return 0;
}

/**
 * @brief Read data from LT8722 device.
 * @param address - Register address.
 * @param data - Received data.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_reg_read(struct lt8722_dev *dev, uint8_t address, uint32_t *data)
{
	int8_t ret = 0;
	struct lt8722_packet packet;
	struct lt8722_command command = {
		LT8722_DATA_READ_COMMAND,
		LT8722_DATA_READ_COMMAND_SIZE
	};
	packet.command = command;
	packet.reg = lt8722_regs[address];
	ret = lt8722_transaction(dev, &packet);
	if (ret)
		return ret;
	*data = packet.data;
	return 0;
}

/**
 * @brief Write data to LT8722 device.
 * @param address - Register address.
 * @param data - Data to be written.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_reg_write(struct lt8722_dev *dev, uint8_t address, uint32_t data)
{
	struct lt8722_packet packet;
	struct lt8722_command command = {
		LT8722_DATA_WRITE_COMMAND,
		LT8722_DATA_WRITE_COMMAND_SIZE
	};
	packet.command = command;
	packet.reg = lt8722_regs[address];
	packet.data = data;
	return lt8722_transaction(dev, &packet);
}

/**
 * @brief Write to LT8722 device register with mask.
 * @param address - Register address.
 * @param mask - Mask to be applied.
 * @param data - Data to be written.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_reg_write_mask(struct lt8722_dev *dev, uint8_t address, uint32_t mask, uint32_t data)
{
	uint32_t reg_data;
	lt8722_reg_read(dev, address, &reg_data);
	reg_data &= ~mask;
	reg_data |= field_prep(mask, data);
	return lt8722_reg_write(dev, address, reg_data);
}

/**
 * @brief Set ENABLE_REQ field in LT8722 device.
 * @param value - Enable if true, disabled otherwise
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_set_enable_req(struct lt8722_dev *dev, bool value)
{
	if (value) dev->status |= (1 << TEC_ENABLED_POS);
	else dev->status &= ~(1 << TEC_ENABLED_POS);
	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND, LT8722_ENABLE_REQ_MASK, value);
}

/**
 * @brief Set switching enable of LT8722 device.
 * @param value - Enable if true, disabled otherwise
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_set_swen_req(struct lt8722_dev *dev, bool value)
{
	if (value) dev->status |= (1 << TEC_SWITCH_ENABLED_POS);
	else dev->status &= ~(1 << TEC_SWITCH_ENABLED_POS);
	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND, LT8722_SWEN_REQ_MASK, value);
}

/**
 * @brief Shutdown the LT8722 device.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_reset(struct lt8722_dev *dev)
{
	dev->status &= ~((1 << TEC_INIT_POS) | (1 << TEC_ENABLED_POS) | (1 << TEC_SWITCH_ENABLED_POS) | ((1 << TEC_DIR_POS)));
	return lt8722_reg_write_mask(dev, LT8722_SPIS_COMMAND, LT8722_SPI_RST_MASK, LT8722_SPI_RST_RESET);
}

/**
 * @brief Clear LT8722 device faults.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_clear_faults(struct lt8722_dev *dev)
{
	return lt8722_reg_write_mask(dev, LT8722_SPIS_STATUS, LT8722_FAULTS_MASK, 0);
}

/**
 * @brief Clear LT8722 status register.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_clear_status(struct lt8722_dev *dev)
{
	return lt8722_reg_write(dev, LT8722_SPIS_STATUS, 0);
}

/**
 * @brief Get LT8722 device status.
 * @param status - Status value to be returned.
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_get_status(struct lt8722_dev *dev, uint16_t *status)
{
	int8_t ret;
	struct lt8722_packet packet;
	struct lt8722_command command = {
		LT8722_STATUS_ACQUISITION_COMMAND,
		LT8722_STATUS_ACQUISITION_COMMAND_SIZE
	};
	packet.command = command;
	packet.reg = lt8722_regs[LT8722_SPIS_STATUS];
	ret = lt8722_transaction(dev, &packet);
	if (ret)
		return ret;
	*status = packet.status;
	return 0;
}

/**
 * @brief Set DAC code of LT8722 device.
 * @param value - DAC value
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_set_dac(struct lt8722_dev *dev, uint32_t value)
{
	return lt8722_reg_write_mask(dev, LT8722_SPIS_DAC, LT8722_SPIS_DAC_MASK, value);
}

/**
 * @brief Get DAC code of LT8722 device.
 * @param value - DAC value
 * @return 0 in case of succes, negative error code otherwise
 */
int8_t lt8722_get_dac(struct lt8722_dev *dev, uint32_t *value)
{
	int ret;
	uint32_t data;
	ret = lt8722_reg_read(dev, LT8722_SPIS_DAC, &data);
	if (ret)
		return ret;
	*value = field_get(LT8722_SPIS_DAC_MASK, data);
	return 0;
}

/**
 * @brief Set positive output voltage limit of LT8722 device.
 * @param value - Positive output voltage limit value
 * @return 0 in case of success, negative error code otherwise
 */
int8_t lt8722_set_spis_ov_clamp(struct lt8722_dev *dev, uint8_t value)
{
	return lt8722_reg_write_mask(dev, LT8722_SPIS_OV_CLAMP, LT8722_SPIS_OV_CLAMP_MASK, value);
}

/**
 * @brief Get positive output voltage limit of LT8722 device.
 * @param value - Positive output voltage limit value
 * @return 0 in case of success, negative error code otherwise
 */
int8_t lt8722_get_spis_ov_clamp(struct lt8722_dev *dev, uint8_t *value)
{
	int ret;
	uint32_t data;
	ret = lt8722_reg_read(dev, LT8722_SPIS_OV_CLAMP, &data);
	if (ret)
		return ret;
	*value = field_get(LT8722_SPIS_OV_CLAMP_MASK, data);
	return 0;
}

/**
 * @brief Set negative output voltage limit of LT8722 device.
 * @param value - Negative output voltage limit value
 * @return 0 in case of success, negative error code otherwise
 */
int8_t lt8722_set_spis_uv_clamp(struct lt8722_dev *dev, uint8_t value)
{
	return lt8722_reg_write_mask(dev, LT8722_SPIS_UV_CLAMP, LT8722_SPIS_UV_CLAMP_MASK, value);
}

/**
 * @brief Get negative output voltage limit of LT8722 device.
 * @param value - Negative output voltage limit value
 * @return 0 in case of success, negative error code otherwise
 */
int8_t lt8722_get_spis_uv_clamp(struct lt8722_dev *dev, uint8_t *value)
{
	int ret;
	uint32_t data;
	ret = lt8722_reg_read(dev, LT8722_SPIS_UV_CLAMP, &data);
	if (ret)
		return ret;
	*value = field_get(LT8722_SPIS_UV_CLAMP_MASK, data);
	return 0;
}

/**
 * @brief Initialize the LT8722 device.
 * @param init_param - Initialization parameter containing information about the
 * 		LT8722 device to be initialized.
 * @return 0 in case of success, negative error code otherwise
*/
int8_t lt8722_init(struct lt8722_dev *dev)
{
	int8_t ret = 0;
	int32_t dac;
	int64_t voltage;
	int64_t start_voltage;
	int64_t end_voltage;
//	LL_GPIO_ResetOutputPin((GPIO_TypeDef*)en_port[channel], en_pin[channel]);
//	LL_GPIO_ResetOutputPin((GPIO_TypeDef*)swen_port[channel], swen_pin[channel]);
	LL_GPIO_ResetOutputPin(dev->en_port, dev->en_pin);
	LL_GPIO_ResetOutputPin(dev->swen_port, dev->swen_pin);
//		LL_GPIO_ResetOutputPin(TEC_1_EN_GPIO_Port, TEC_1_EN_Pin);
//		LL_GPIO_ResetOutputPin(TEC_1_SWEN_GPIO_Port, TEC_1_SWEN_Pin);
	/*
	 * Reset LT8722
	 */
	lt8722_reset(dev);
	/*
	 * Start-up sequence
	 * 1. Apply proper VIN and VDDIO voltages
	 *
	 * 2. Enable VCC LDO and other LT8722 circuitry
	 */
	ret = lt8722_clear_faults(dev);

//	LL_GPIO_SetOutputPin((GPIO_TypeDef*)en_port[channel], en_pin[channel]);
	LL_GPIO_SetOutputPin(dev->en_port, dev->en_pin);

	ret = lt8722_set_enable_req(dev, LT8722_ENABLE_REQ_ENABLED);
	ret = lt8722_reg_write(dev, LT8722_SPIS_COMMAND, 0x00003A01);
	/*
	 * 3. Configure output voltage control DAC to 0xFF000000
	 */
	ret = lt8722_set_dac(dev, 0xFF000000);
	/*
	 * 4. Write all SPIS_STATUS registers to 0
	 */
	ret = lt8722_reg_write(dev, LT8722_SPIS_STATUS, 0);
	LL_mDelay(1);
	ret = lt8722_reg_write(dev, LT8722_SPIS_COMMAND, 0x00003A01);
	/*
	 * 5. Ramp the output voltage control DAC from 0xFF000000 to 0x00000000
	 */
	start_voltage = lt8722_dac_to_voltage(0xFF000000);
	end_voltage = lt8722_dac_to_voltage(0x00000000);
	for (uint8_t i = 0;  i < 5; i++)
	{
		voltage = (start_voltage + (end_voltage - start_voltage) * i / 4);
		dac = lt8722_voltage_to_dac(voltage);
		ret = lt8722_set_dac(dev, dac);
		LL_mDelay(1);
	}
	/*
	 * 6. Enable the PWM switching behavior
	 */
//	LL_GPIO_SetOutputPin((GPIO_TypeDef*)swen_port[channel], swen_pin[channel]);
	LL_GPIO_SetOutputPin(dev->swen_port, dev->swen_pin);
	ret = lt8722_set_swen_req(dev, LT8722_SWEN_REQ_ENABLED);
//	delay_us(200);
	LL_mDelay(1);

	/*
	 * 7. Set the desired output voltage
	 */
//		lt8722_set_output_voltage_channel(channel, TEC_COOL, 200000000);
//
//		delay_us(255);
//		delay_us(255);
//
//		ret = lt8722_set_swen_req(channel, LT8722_SWEN_REQ_DISABLED);
	if (!ret)
		{
		dev->status |= ((1 << TEC_INIT_POS) | (1 << TEC_ENABLED_POS) | (1 << TEC_SWITCH_ENABLED_POS)); //tec is initted
		}
	else dev->status = 0;
	dev->voltage = 0;
	return ret;
}

/**
 * @brief Set output volatge of LT8722 device.
 * @param channel - Channel of lt8722.
 * @param value - Output voltage value in nanovolts.
 * @return 0 in case of success, negative error code otherwise
 */
int8_t lt8722_set_output_voltage_channel(struct lt8722_dev *dev, tec_dir_t dir, int64_t value)
{
	uint8_t ret = 0;
	int64_t vdac = 0;
	int32_t dac = 0x0;
	if (dir == TEC_COOL)
	{
		dev->status &= ~(1 << TEC_DIR_POS);
		vdac = LT8722_DAC_OFFSET - value / 16;
	}
	if (dir == TEC_HEAT)
	{
		dev->status |= (1 << TEC_DIR_POS);
		vdac = LT8722_DAC_OFFSET + value / 16;
	}
	dac = lt8722_voltage_to_dac(vdac);
	ret = lt8722_set_dac(dev, dac);
	dev->voltage = value;

	return ret;
}

/* Private support function definition ------------------------------------*/
uint8_t Calculate_CRC8(uint8_t *data, uint8_t length)
{
    uint8_t crc = 0x00;
    uint8_t poly = 0x07;
    for (uint8_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ poly;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void put_unaligned_be32(uint32_t val, uint8_t *buf)
{
	buf[3] = val & 0xFF;
	buf[2] = (val >> 8) & 0xFF;
	buf[1] = (val >> 16) & 0xFF;
	buf[0] = val >> 24;
}
void put_unaligned_be16(uint16_t val, uint8_t *buf)
{
	buf[1] = val & 0xFF;
	buf[0] = val >> 8;
}
uint32_t get_unaligned_be32(uint8_t *buf)
{
	return buf[3] | ((uint16_t)buf[2] << 8) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[0] << 24);
}
uint32_t get_unaligned_be16(uint8_t *buf)
{
	return buf[1] | ((uint16_t)buf[0] << 8);
}
uint32_t find_first_set_bit(uint32_t word)
{
	uint32_t first_set_bit = 0;
	while (word) {
		if (word & 0x1)
			return first_set_bit;
		word >>= 1;
		first_set_bit ++;
	}
	return 32;
}
uint32_t field_prep(uint32_t mask, uint32_t val)
{
	return (val << find_first_set_bit(mask)) & mask;
}
uint32_t field_get(uint32_t mask, uint32_t word)
{
	return (word & mask) >> find_first_set_bit(mask);
}
