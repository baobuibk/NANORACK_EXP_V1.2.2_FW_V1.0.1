#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "module.h"
#include "log.h"
#include "lwl.h"
#include "error_codes.h"
#include "bsp_handshake.h"
#include "bsp_spi_slave.h"
#include "spi_transmit.h"
#include "experiment_task.h"

////////////////////////////////////////////////////////////////////////////////
// EXTERN
////////////////////////////////////////////////////////////////////////////////
extern spi_transmit_task_t spi_transmit_task_inst;
static spi_transmit_task_t *p_spi_transmit_task = &spi_transmit_task_inst;


////////////////////////////////////////////////////////////////////////////////
// Common macros
////////////////////////////////////////////////////////////////////////////////

#define LWL_START_BYTE				0xAA // Start byte for each log record

#ifdef CONFIG_LWL_BUF_SIZE
    #define 	LWL_BUF_SIZE 		(CONFIG_LWL_BUF_SIZE)
 	 #define 	LWL_BUF_THRESHOLD 	(CONFIG_LWL_BUF_THRESHOLD)
#else
    #define LWL_BUF_SIZE 			EXPERIMENT_BUFFER_BYTE_SIZE
	#define LWL_BUF_THRESHOLD 		(LWL_BUF_SIZE - 1024)
#endif



////////////////////////////////////////////////////////////////////////////////
// CRC-8 calculation (using CRC-8-ATM polynomial 0x07)
////////////////////////////////////////////////////////////////////////////////
static const uint8_t crc8_table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

static uint8_t calculate_crc8(const uint8_t *data, uint32_t len) {
    uint8_t crc = 0x00; // Initial value
    for (uint32_t i = 0; i < len; i++) {
        crc = crc8_table[crc ^ data[i]];
    }
    return crc;
}

////////////////////////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////////////////////////

// Structure for log message metadata
struct lwl_msg {
    const char *fmt;       // Format string
    uint8_t num_arg_bytes; // Total number of argument bytes
};

// For writing to flash, this structure needs to be a multiple of 8 bytes.
struct lwl_data_buffer {
    uint32_t put_idx;
    uint8_t *p_buf;
};

typedef struct lwl_t{
	uint32_t lwl_working_buf_index;
	uint32_t lwl_full_buf_index;
	bool	 lwl_buf_over_threshold;
	struct lwl_data_buffer lwl_data_buf[2];
}lwl_t;

__attribute__((aligned(4))) static uint8_t lwl_data_buf_0[LWL_BUF_SIZE] = {0};
__attribute__((aligned(4))) static uint8_t lwl_data_buf_1[LWL_BUF_SIZE] = {0};

static lwl_t lwl = {
    .lwl_working_buf_index = 0,
    .lwl_full_buf_index = 1,
    .lwl_buf_over_threshold = false,
    .lwl_data_buf = {
        { .put_idx = 0, .p_buf = (uint8_t *)lwl_data_buf_0 },
        { .put_idx = 0, .p_buf = (uint8_t *)lwl_data_buf_1 }
    }
};

// Log message table (ID is the index)
static const struct lwl_msg lwl_msg_table[] = {
    {NULL, 0},                                       // ID 0: EXP_INVALID
    {"Time: Day %1d, %1d:%1d:%1d", 4},               // ID 1: EXP_TIMESTAMP
    {"Temperature: NTC[%1d]: %2d", 3},               // ID 2: EXP_TEMP_SINGLE_NTC
    {"Temperature: Set temperature profile: Target_temp: %2d, Min_temp: %2d, Max_temp: %2d, NTC_primary: %1d, NTC_secondary: %1d, Auto_recover: %1d, TEC_mask: %1d, Heater_mask: %1d, TEC_vol: %2d, Heater_duty: %1d", 14}, // ID 3: EXP_TEMP_PROFILE_SET
    {"Temperature: MANUAL mode", 0},                 // ID 4: EXP_TEMP_MANUAL_MODE
    {"Temperature: AUTO mode", 0},                   // ID 5: EXP_TEMP_AUTO_MODE
    {"Temperature: COOLING", 0},                     // ID 6: EXP_TEMP_COOLING
    {"Temperature: HEATING", 0},                     // ID 7: EXP_TEMP_HEATING
    {"Temperature: ERROR, Primary NTC = %1d Secondary NTC = %1d", 2}, // ID 8: EXP_TEMP_ERROR
    {"Temperature: Set TEC override profile: Interval: %2d, TEC[%1d]: %2d mV", 5}, // ID 9: EXP_TEMP_TEC_OVERRIDE_PROFILE
    {"Temperature: TEC override ON", 0},     // ID 10: EXP_TEMP_TEC_OVERRIDE_ON
    {"Temperature: TEC override OFF", 0},    // ID 11: EXP_TEMP_TEC_OVERRIDE_OFF
    {"TEC: MANUAL mode: TEC[%1d] ON", 1},            // ID 12: EXP_TEC_MANUAL_ON
    {"TEC: MANUAL mode: TEC[%1d] OFF", 1},           // ID 13: EXP_TEC_MANUAL_OFF
    {"TEC: AUTO mode: TEC ON with %2dmV", 2},        // ID 14: EXP_TEC_AUTO_ON
    {"TEC: AUTO mode: TEC OFF", 0},                  // ID 15: EXP_TEC_AUTO_OFF
    {"Heater: MANUAL mode: Heater[%1d] ON", 1},      	// ID 16: EXP_HEATER_MANUAL_ON
    {"Heater: MANUAL mode: Heater[%1d] OFF", 1},     	// ID 17: EXP_HEATER_MANUAL_OFF
    {"Heater: AUTO mode: Heater ON with duty %1d", 1}, 	// ID 18: EXP_HEATER_AUTO_ON
    {"Heater: AUTO mode: Heater OFF", 0},            	// ID 19: EXP_HEATER_AUTO_OFF
    {"Internal laser: MANUAL mode: Switch ON: Index: %1d, Current: %1d", 2}, // ID 20: EXP_LASER_INT_MANUAL_ON
    {"Internal laser: MANUAL mode: Switch OFF", 0},  // ID 21: EXP_LASER_INT_MANUAL_OFF
    {"Internal laser: SAMPLING mode: Switch ON: Index: %1d, Current: %1d", 2}, // ID 22: EXP_LASER_INT_SAMPLE_ON
    {"Internal laser: SAMPLING mode: Switch OFF", 0}, // ID 23: EXP_LASER_INT_SAMPLE_OFF
    {"External laser: MANUAL mode: Switch ON: Index: %1d, Current: %1d", 2}, // ID 24: EXP_LASER_EXT_MANUAL_ON
    {"External laser: MANUAL mode: Switch OFF", 0},  // ID 25: EXP_LASER_EXT_MANUAL_OFF
    {"External laser: SAMPLING mode: Switch ON: Index: %1d, Current: %1d", 2}, // ID 26: EXP_LASER_EXT_SAMPLE_ON
    {"External laser: SAMPLING mode: Switch OFF", 0}, // ID 27: EXP_LASER_EXT_SAMPLE_OFF
    {"Photo: SAMPLING mode: Switch ON: Index: %1d", 1}, // ID 28: EXP_PHOTO_SAMPLE_ON
    {"Photo: SAMPLING mode: Switch OFF", 0},         // ID 29: EXP_PHOTO_SAMPLE_OFF
    {"Experiment: Set photo profile: Sample_rate: %4d (sample/s), Pre_duration: %2d (mS), Sample_duration: %2d (mS), Post_duration: %2d (mS)", 10}, // ID 30: EXP_SET_PHOTO_PROFILE
    {"Experiment: Set laser intensity: %1d", 1},      // ID 31: EXP_SET_LASER_INTENSITY
    {"Experiment: Set laser&photo index: %1d", 1},   // ID 32: EXP_SET_LASER_PHOTO_INDEX
    {"Experiment: Start", 0},                        // ID 33: EXP_START
    {"Experiment: Stop", 0},                         // ID 34: EXP_STOP
    {"System: Reset OTA", 0},                        // ID 35: EXP_SYS_RESET_OTA
	{"System: Set time: Day: %1d, Hour: %1d, Minute: %1d, Second: %1d", 4},                        // ID 36:
	{"Transmit: Send photo chunk[%1d] data", 1},
	{"Transmit: Send laser current data", 0},
	{"Transmit: Send log data", 0},
    {"[OBC-STM32] Log Block------------------------", 0}, //OBC_STM32------------LOG,      // ID 40
    {"[OBC-STM32] Test Log", 0},  // ID 41
    {"[OBC-STM32] Start Up", 0},  // ID 42
    {"[OBC-STM32] Booting at %1d/%1d/%2d %1d:%1d:%1d", 7}, // 43
    {"[OBC-STM32] LogTest at %1d/%1d/%1d %1d:%1d:%1d", 6}, // 44
	{"[OBC-STM32] [Init] Step %1d - return code %1d", 2}, // 45
	{"[OBC-STM32] [Alive] Miss Heart Beat at %1d/%1d/20%1d %1d:%1d:%1d", 6}, // 46
	{"[OBC-STM32] [FileSystem] SD Release at %1d/%1d/20%1d %1d:%1d:%1d", 6}, // 47
	{"[OBC-STM32] [FileSystem] SD Lockin at %1d/%1d/20%1d %1d:%1d:%1d", 6}, // 48
	{"[OBC-STM32] [FileSystem] SD Clean at %1d/%1d/20%1d %1d:%1d:%1d", 6}, // 49
	{"[OBC-STM32] [SimpleTransf] CM4 not response at %1d/%1d/20%1d %1d:%1d:%1d", 6}, // 50
	{"[OBC-STM32] [SimpleTransf] File Write Fail at %1d/%1d/20%1d %1d:%1d:%1d", 6},  // 51
	{"[OBC-STM32] [ScriptManager] RTC is set to %1d/%1d/20%1d %1d:%1d:%1d", 6}, // 52
	{"[OBC-STM32] [ScriptManager] Time point at %1d/%1d/20%1d %1d:%1d:%1d, Index: %1d", 7},  //53
	{"[OBC-STM32] [ScriptManager] Execute Routine INIT: %1d, Step: %2d", 3}, 	// 54
	{"[OBC-STM32] [ScriptManager] Execute Routine DLS: %1d, Step: %2d", 3}, 	// 55
	{"[OBC-STM32] [ScriptManager] Execute Routine CAM: %1d, Step: %2d", 3}, 	// 56
	{"[OBC-STM32] [ScriptManager] Execute Routine Return Code: %1d", 1}, 	// 57
	{"[OBC-STM32] [MIN-EXP] Callback of ID: %1d, return size: %1d", 2}, 		// 58
	{"[OBC-STM32] [MODFSP-CM4] Callback of ID: %1d, return size: %2d", 3}, 		// ID 59: EXP_SYS_RESET_OTA
	{"[OBC-STM32] Reset cause: %4d", 4}, 		// 60
};

static const uint8_t lwl_msg_table_size = sizeof(lwl_msg_table) / sizeof(lwl_msg_table[0]);


void lwl_init(lwl_t *lwl) {
    // Xóa toàn bộ cấu trúc về 0 bằng memset
    memset(lwl->lwl_data_buf[0].p_buf, 0, LWL_BUF_SIZE);
    memset(lwl->lwl_data_buf[1].p_buf, 0, LWL_BUF_SIZE);
}

void lwl_start() {
    // Xóa toàn bộ cấu trúc về 0 bằng memset
//	lwl_init(&lwl);
	lwl_clear_notification();
}



#include "embedded_cli.h"
extern EmbeddedCli * shell_uart_cli;

//#define CLI_DEBUG

#ifdef CLI_DEBUG

#define cli_debug(id)  do { \
    switch (id) { \
        case 4: \
            cli_printf(shell_uart_cli, "Temperature: MANUAL mode\r\n"); \
            break; \
        case 5: \
            cli_printf(shell_uart_cli, "Temperature: AUTO mode\r\n"); \
            break; \
        case 6: \
            cli_printf(shell_uart_cli, "Temperature: COOLING\r\n"); \
            break; \
        case 7: \
            cli_printf(shell_uart_cli, "Temperature: HEATING\r\n"); \
            break; \
        default: \
            cli_printf(shell_uart_cli, "Unknown debug ID: %d\r\n", id); \
            break; \
    } \
} while (0)

#else
#define cli_debug(id)  ((void)0)
#endif


/*
 * @brief Record a lightweight log with start byte, length, and CRC.
 *
 * @param[in] id The log message ID (index into lwl_msg_table).
 * @param[in] ... Variable arguments, each 1 byte (from LWL_n macros).
 *
 * Each log record format: [START_BYTE (0xAA)][LENGTH][ID][ARG_BYTES][CRC]
 * LENGTH = 1 (length) + 1 (id) + num_arg_bytes + 1 (CRC)
 * CRC is calculated over [ID][ARG_BYTES]
 */
void LWL(uint8_t id, ...)
{
	// CLI_debug
	cli_debug(id);


    va_list ap;
    uint32_t put_idx;

    struct lwl_data_buffer * lwl_data_buf = &lwl.lwl_data_buf[lwl.lwl_working_buf_index];

    // Validate ID
    if (id == 0 || id >= lwl_msg_table_size || lwl_msg_table[id].fmt == NULL) {
        return; // Invalid ID
    }

    const struct lwl_msg *msg = &lwl_msg_table[id];
    uint8_t length = 1 + 1 + msg->num_arg_bytes + 1; // length + id + args + CRC

    va_start(ap, id);


    put_idx = lwl_data_buf->put_idx % LWL_BUF_SIZE;
    lwl_data_buf->put_idx = (put_idx + length + 1) % LWL_BUF_SIZE; // +1 for START_BYTE

    // Write start byte
    *(lwl_data_buf->p_buf + put_idx) = LWL_START_BYTE;
    put_idx = (put_idx + 1) % LWL_BUF_SIZE;

    // Write length
    *(lwl_data_buf->p_buf + put_idx) = length;
    put_idx = (put_idx + 1) % LWL_BUF_SIZE;

    // Write ID
    *(lwl_data_buf->p_buf + put_idx) = id;
    uint8_t crc_data[1 + msg->num_arg_bytes]; // Buffer for CRC calculation
    crc_data[0] = id;
    put_idx = (put_idx + 1) % LWL_BUF_SIZE;

    // Write arguments and collect for CRC
    for (uint8_t i = 0; i < msg->num_arg_bytes; i++) {
        uint32_t arg = va_arg(ap, unsigned);
        *(lwl_data_buf->p_buf + put_idx) = (uint8_t)(arg & 0xFF);
        crc_data[i + 1] = *(lwl_data_buf->p_buf + put_idx);
        put_idx = (put_idx + 1) % LWL_BUF_SIZE;
    }

    // Calculate and write CRC
    uint8_t crc = calculate_crc8(crc_data, 1 + msg->num_arg_bytes);
    *(lwl_data_buf->p_buf + put_idx) = crc;

    if (lwl_data_buf->put_idx > LWL_BUF_THRESHOLD) 		//buffer nearly full
    {
    	lwl.lwl_buf_over_threshold = 1;

    	if (lwl.lwl_working_buf_index == 0)
		{
    		lwl.lwl_working_buf_index = 1;
    		lwl.lwl_full_buf_index = 0;
    		lwl.lwl_data_buf[0].put_idx = 0;
		}
    	else
    	{
    		lwl.lwl_working_buf_index = 0;
    		lwl.lwl_full_buf_index = 1;
    		lwl.lwl_data_buf[1].put_idx = 0;
    	}

    	lwl_buffer_full_notify();
    }
    va_end(ap);
}

//int32_t cmd_lwl_test() {
//    LWL(TIMESTAMP, LWL_4(0x123456)); // 4 bytes: 0x56, 0x34, 0x12, 0x00
//    LWL(TEMPERATURE_NTC, LWL_2(0), LWL_2(1), LWL_2(2), LWL_2(3), LWL_2(4), LWL_2(5), LWL_2(6), LWL_2(7), LWL_2(8)); // 16 bytes
//    LWL(TEMPERATURE_AUTOMMODE_ON); // 0 bytes
//    return 0;
//}

__weak void lwl_buffer_full_notify()
{
	bsp_handshake_spi_buffer_full_notify();
}

__weak void lwl_clear_notification()
{
	lwl.lwl_buf_over_threshold = 0;
	bsp_handshake_spi_clear_notification();
}

//uint8_t * lwl_get_log_buffer(uint32_t index)
//{
//	return &(lwl.lwl_data_buf[index].buf[0]);
//}
//uint32_t lwl_get_log_size(uint32_t index)
//{
//	return lwl.lwl_data_buf[index].put_idx;
//}
//uint32_t lwl_get_current_index(void)
//{
//	return lwl.lwl_working_buf_index;
//}

void lwl_reinit_buffer(struct lwl_data_buffer * lwl_buffer)
{
//    memset(lwl->lwl_data_buf[0].p_buf, 0, LWL_BUF_SIZE);
//    memset(lwl->lwl_data_buf[1].p_buf, 0, LWL_BUF_SIZE);
}

uint16_t * lwl_get_full_buffer_addr(void)
{
	return (uint16_t *)lwl.lwl_data_buf[lwl.lwl_full_buf_index].p_buf;
}

uint32_t lwl_log_send_to_spi(void)
{
	// Add log
	LWL(LWL_EXP_TRANS_LOG_DATA);

	lwl_clear_notification();
	// Cấu hình địa chỉ buffer
	SPI_SlaveDevice_CollectData((uint16_t *)lwl_get_full_buffer_addr());
	// Bật tín hiệu DataReady
	spi_transmit_task_data_ready(p_spi_transmit_task);
	return ERROR_OK;
}

