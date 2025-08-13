/*
 * lt8722.h
 *
 *  Created on: Nov 27, 2024
 *      Author: SANG HUYNH
 */

#ifndef DEVICES_LT8722_LT8722_H_
#define DEVICES_LT8722_LT8722_H_

#include "board.h"
#include "stm32f7xx.h"
#include "stdbool.h"

typedef enum {TEC_COOL = 0, TEC_HEAT = 1} tec_dir_t;

#define LT8722_CRC_POLYNOMIAL          			 0x07

#define LT8722_MAX_REGISTER_SIZE      			 4
#define LT8722_NUM_REGISTERS          			 8

/*-------------------Command Byte-----------------*/
#define LT8722_STATUS_ACQUISITION_COMMAND        0xF0
#define LT8722_DATA_WRITE_COMMAND                0xF2
#define LT8722_DATA_READ_COMMAND                 0xF4

/*-------------Command Size in Bytes--------------*/
#define LT8722_STATUS_ACQUISITION_COMMAND_SIZE   4
#define LT8722_DATA_WRITE_COMMAND_SIZE           8
#define LT8722_DATA_READ_COMMAND_SIZE            8

/*-------------------Acknowledge-----------------*/
#define LT8722_ACK_ACKNOWLEDGE             		 0xA5
#define LT8722_ACK_NON_ACKNOWLEDGE         		 0xC3
#define LT8722_ACK_UNSUPPORTED_ADDRESS     		 0x0F
#define LT8722_ACK_STUCK_AT_0              		 0x00
#define LT8722_ACK_STUCK_AT_1              		 0xFF

/*----------------Register Address----------------*/
#define LT8722_SPIS_COMMAND       				 0x00
#define LT8722_SPIS_STATUS        				 0x01
#define LT8722_SPIS_DAC_ILIMN     				 0x02
#define LT8722_SPIS_DAC_ILIMP     				 0x03
#define LT8722_SPIS_DAC							 0x04
#define LT8722_SPIS_OV_CLAMP      				 0x05
#define LT8722_SPIS_UV_CLAMP      				 0x06
#define LT8722_SPIS_AMUX      					 0x07

/*--------------Register Size in Bits-------------*/
#define LT8722_SPIS_COMMAND_SIZE           		 22
#define LT8722_SPIS_STATUS_SIZE            		 11
#define LT8722_SPIS_DAC_ILIMN_SIZE         		 9
#define LT8722_SPIS_DAC_ILIMP_SIZE         		 9
#define LT8722_SPIS_DAC_SIZE               		 32
#define LT8722_SPIS_OV_CLAMP_SIZE          		 4
#define LT8722_SPIS_UV_CLAMP_SIZE          		 4
#define LT8722_SPIS_AMUX_SIZE              		 7

/*-------------Register Default Value------------*/
#define LT8722_SPIS_COMMAND_DEFAULT_VALUE  		0x08A214
#define LT8722_SPIS_STATUS_DEFAULT_VALUE   		0x0
#define LT8722_SPIS_DAC_ILIMN_DEFAULT_VALUE		0x1FF
#define LT8722_SPIS_DAC_ILIMP_DEFAULT_VALUE		0x000
#define LT8722_SPIS_DAC_DEFAULT_VALUE      		0xFF000000
#define LT8722_SPIS_OV_CLAMP_DEFAULT_VALUE 		0xF
#define LT8722_SPIS_UV_CLAMP_DEFAULT_VALUE 		0x0
#define LT8722_SPIS_AMUX_DEFAULT_VALUE     		0x00

#define BIT(x)	(1 << (x))
#define BITS_PER_LONG 32
#define GENMASK(h, l) ({uint32_t t = (uint32_t)(~0UL);			\
		t = t << (BITS_PER_LONG - (h - l + 1));		\
		t = t >> (BITS_PER_LONG - (h + 1));		\
		t;						\
})

// LT8722 SPIS_COMMAND Register
#define LT8722_ENABLE_REQ_MASK             BIT(0)
#define LT8722_SWEN_REQ_MASK               BIT(1)
#define LT8722_SW_FRQ_SET_MASK             GENMASK(4, 2)
#define LT8722_SW_FRQ_ADJ_MASK             GENMASK(6, 5)
#define LT8722_SYS_DC_MASK                 GENMASK(8, 7)
#define LT8722_VCC_VREG_MASK               BIT(9)
#define LT8722_SW_VC_INT_MASK              GENMASK(13, 11)
#define LT8722_SPI_RST_MASK                BIT(14)
#define LT8722_PWR_LIM_POS                 15
#define LT8722_PWR_LIM_MASK                GENMASK(18, 15)

// LT8722 SPIS_COMMAND Register SPI_RST
#define LT8722_SPI_RST_RESET               0b1

// LT8722 SPIS_STATUS Register
#define LT8722_SWEN_MASK                   BIT(0)
#define LT8722_SRVO_ILIM_MASK              BIT(1)
#define LT8722_SRVO_PLIM_MASK              BIT(2)
#define LT8722_MIN_OT_MASK                 BIT(3)
#define LT8722_POR_OCC_MASK                BIT(4)
#define LT8722_OVER_CURRENT_MASK           BIT(5)
#define LT8722_TSD_MASK                    BIT(6)
#define LT8722_VCC_UVLO_MASK               BIT(7)
#define LT8722_VDDIO_UVLO_MASK             BIT(8)
#define LT8722_CP_UVLO_MASK                BIT(9)
#define LT8722_V2P5_UVLO_MASK              BIT(10)

// LT8722 faults mask
#define LT8722_FAULTS_MASK                 GENMASK(10, 5)

// LT8722 SPIS_DAC_ILIMN Register
#define LT8722_SPIS_DAC_ILIMN_MASK         GENMASK(8, 0)

// LT8722 SPIS_DAC_ILIMN Register SPIS_DAC_ILIMN
#define LT8722_SPIS_DAC_ILIMN_MIN          0x030
#define LT8722_SPIS_DAC_ILIMN_MAX          0x1FF

// LT8722 SPIS_DAC_ILIMP Register
#define LT8722_SPIS_DAC_ILIMP_MASK         GENMASK(8, 0)

// LT8722 SPIS_DAC_ILIMP Register SPIS_DAC_ILIMP
#define LT8722_SPIS_DAC_ILIMP_MIN          0x000
#define LT8722_SPIS_DAC_ILIMP_MAX          0x1CE

// LT8722 SPIS_DAC Register
#define LT8722_SPIS_DAC_MASK               GENMASK(31, 0)

// LT8722 SPIS_OV_CLAMP Register
#define LT8722_SPIS_OV_CLAMP_MASK          GENMASK(3, 0)

// LT8722 SPIS_UV_CLAMP Register
#define LT8722_SPIS_UV_CLAMP_MASK          GENMASK(3, 0)

// LT8722 SPIS_AMUX Register
#define LT8722_AMUX_MASK                   GENMASK(3, 0)
#define LT8722_AMUX_TEST_MASK              GENMASK(5, 4)
#define LT8722_AOUT_EN_MASK                BIT(6)

#define LT8722_DAC_RESOLUTION              25
#define LT8722_DAC_OFFSET                  1250000000LL  // nanovolts
#define LT8722_DAC_VREF                    2500000000LL // nanovolts
#define LT8722_VOLTAGE_SCALE			   NANO

/**
 * @enum lt8722_enable_req
 * @brief VCC LDO enable bit and linear power stage enable request bit
 */
enum lt8722_enable_req {
	/** Disable bit */
	LT8722_ENABLE_REQ_DISABLED,
	/** Enable bit */
	LT8722_ENABLE_REQ_ENABLED,
};

/**
 * @enum lt8722_swen_req
 * @brief PWM switch enable request bit
 */
enum lt8722_swen_req {
	/** Disable bit */
	LT8722_SWEN_REQ_DISABLED,
	/** Enable bit */
	LT8722_SWEN_REQ_ENABLED,
};

/**
 * @enum lt8722_sw_frq_set
 * @brief PWM switch frequency control bits
 */
enum lt8722_sw_frq_set {
	/** 0.5 MHz */
	LT8722_SW_FRQ_SET_0_5MHZ,
	/** 1 MHz */
	LT8722_SW_FRQ_SET_1MHZ,
	/** 1.5 MHz */
	LT8722_SW_FRQ_SET_1_5MHZ,
	/** 2 MHz */
	LT8722_SW_FRQ_SET_2MHZ,
	/** 2.5 MHz */
	LT8722_SW_FRQ_SET_2_5MHZ,
	/** 3 MHz */
	LT8722_SW_FRQ_SET_3MHZ,
	/** 3 MHz */
	LT8722_SW_FRQ_SET_3MHZ_2,
	/** 3 MHz */
	LT8722_SW_FRQ_SET_3MHZ_3,
};

/**
 * @enum lt8722_sw_frq_adj
 * @brief PWM switch frequency adjustment bits
 */
enum lt8722_sw_frq_adj {
	/** 0% */
	LT8722_SW_FRQ_ADJ_0_PERCENT,
	/** +15% */
	LT8722_SW_FRQ_ADJ_POSITIVE_15_PERCENT,
	/** -15% */
	LT8722_SW_FRQ_ADJ_NEGATIVE_15_PERCENT,
	/** 0% */
	LT8722_SW_FRQ_ADJ_0_PERCENT_2,
};

/**
 * @enum lt8722_vcc_vreg
 * @brief VCC LDO regulation control bits
 */
enum lt8722_vcc_vreg {
	/** 3.1V */
	LT8722_VCC_VREG_3_1V,
	/** 3.4V */
	LT8722_VCC_VREG_3_4V,
};

/**
 * @enum lt8722_sw_vc_int
 * @brief Typical peak inductor current after BST-SW refresh period control bits
 */
enum lt8722_sw_vc_int {
	/** 0.252A */
	LT8722_SW_VC_INT_0_252A,
	/** 0.594A */
	LT8722_SW_VC_INT_0_594A,
	/** 0.936A */
	LT8722_SW_VC_INT_0_936A,
	/** 1.278A */
	LT8722_SW_VC_INT_1_278A,
	/** 1.620A */
	LT8722_SW_VC_INT_1_620A,
	/** 1.962A */
	LT8722_SW_VC_INT_1_962A,
	/** 2.304A */
	LT8722_SW_VC_INT_2_304A,
	/** 2.646A */
	LT8722_SW_VC_INT_2_646A,
};

/**
 * @enum lt8722_pwr_lim
 * @brief Linear power stage MOSFET power limit control bits
 */
enum lt8722_pwr_lim {
	/** 2W */
	LT8722_PWR_LIM_2W,
	/** No limit */
	LT8722_PWR_LIM_NO_LIMIT,
	/** 3W */
	LT8722_PWR_LIM_3W,
	/** 3.5W */
	LT8722_PWR_LIM_3_5W,
};

/**
 * @enum lt8722_sys_dc
 * @brief PWM duty cycle control bits
 */
enum lt8722_sys_dc {
	/** 20% - 80% */
	LT8722_SYS_DC_20_80_DUTY_CYCLE,
	/** 15% - 85% */
	LT8722_SYS_DC_15_85_DUTY_CYCLE,
	/** 10% - 90% */
	LT8722_SYS_DC_10_90_DUTY_CYCLE,
};

/**
 * @enum lt8722_amux
 * @brief Analog multiplexer control bits
 */
enum lt8722_amux {
	LT8722_AMUX_VILIMP,
	LT8722_AMUX_VILIMN,
	LT8722_AMUX_VDAC,
	LT8722_AMUX_VOUT,
	LT8722_AMUX_IOUT,
	LT8722_AMUX_V2P5,
	LT8722_AMUX_V1P25,
	LT8722_AMUX_V1P65,
	LT8722_AMUX_VTEMP,
	LT8722_AMUX_VIN,
	LT8722_AMUX_VCC,
	LT8722_AMUX_VDDIO,
	LT8722_AMUX_VSFB,
	LT8722_AMUX_VSFB_2,
	LT8722_AMUX_VSFB_3,
	LT8722_AMUX_VSFB_4
};

/**
 * @enum lt8722_aout_en
 * @brief Analog output buffer enable control bits
 */
enum lt8722_aout_en {
	/** Disable */
	LT8722_AOUT_EN_DISABLED,
	/** Enable */
	LT8722_AOUT_EN_ENABLED,
};

/**
 * @struct lt8722_reg
 * @brief LT8722 register structure.
 */
struct lt8722_reg {
	uint8_t address;
	uint32_t default_val;
	uint8_t size;
};
/**
 * @struct lt8722_command
 * @brief LT8722 command structure.
 */
struct lt8722_command {
	uint8_t byte;
	uint8_t size;
};

/**
 * @struct lt8722_packet
 * @brief LT8722 packet structure.
 */
struct lt8722_packet {
	struct lt8722_command command;
	struct lt8722_reg reg;
	uint32_t data;
	uint8_t crc;
	uint8_t ack;
	uint16_t status;
};

/**
 * @struct lt8722_dev
 * @brief Device descriptor for LT8722.
 */
#define TEC_INIT_POS 			0
#define TEC_ENABLED_POS 		1
#define TEC_SWITCH_ENABLED_POS 	2
#define TEC_DIR_POS 			3
#define TEC_FAULT_POS 			4
struct lt8722_dev {
	SPI_TypeDef *hspi;
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
	GPIO_TypeDef *en_port;
	uint16_t en_pin;
	GPIO_TypeDef *swen_port;
	uint16_t swen_pin;
	uint8_t status; //							FAULT | DIR | SWITCH_ENABLED | ENABLED | INIT
	int64_t  voltage;

};

int32_t lt8722_voltage_to_dac(int64_t voltage);
int64_t lt8722_dac_to_voltage(int32_t dac);
int8_t lt8722_transaction(struct lt8722_dev *dev, struct lt8722_packet *packet);
int8_t lt8722_reg_read(struct lt8722_dev *dev, uint8_t address, uint32_t *data);
int8_t lt8722_reg_write(struct lt8722_dev *dev, uint8_t address, uint32_t data);
int8_t lt8722_reg_write_mask(struct lt8722_dev *dev, uint8_t address, uint32_t mask, uint32_t data);

int8_t lt8722_set_enable_req(struct lt8722_dev *dev, bool value);
int8_t lt8722_set_swen_req(struct lt8722_dev *dev, bool value);
int8_t lt8722_reset(struct lt8722_dev *dev);

int8_t lt8722_clear_faults(struct lt8722_dev *dev);
int8_t lt8722_clear_status(struct lt8722_dev *dev);
int8_t lt8722_get_status(struct lt8722_dev *dev, uint16_t *status);

int8_t lt8722_set_dac(struct lt8722_dev *dev, uint32_t value);
int8_t lt8722_get_dac(struct lt8722_dev *dev, uint32_t *value);

int8_t lt8722_set_spis_ov_clamp(struct lt8722_dev *dev, uint8_t value);
int8_t lt8722_get_spis_ov_clamp(struct lt8722_dev *dev, uint8_t *value);

int8_t lt8722_set_spis_uv_clamp(struct lt8722_dev *dev, uint8_t value);
int8_t lt8722_get_spis_uv_clamp(struct lt8722_dev *dev, uint8_t *value);

int8_t lt8722_init(struct lt8722_dev *dev);
int8_t lt8722_set_output_voltage_channel(struct lt8722_dev *dev, tec_dir_t dir, int64_t value);

#endif /* DEVICES_LT8722_LT8722_H_ */
