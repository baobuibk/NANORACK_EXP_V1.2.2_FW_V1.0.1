/*
 * app_configs.h
 *
 *  Created on: Jun 11, 2025
 *      Author: Admin
 */

#ifndef APP_CONFIGS_H_
#define APP_CONFIGS_H_

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#ifndef UNUSED_FUNC
#define UNUSED_FUNC __attribute__((unused))
#endif

/* En/Dis Debug/AutoComplete CMD */
#define DEBUG_ENABLE
#define CLI_AUTOCOMPLETE_ENABLE


#define SHELL_UART_INITATION		    "EXP $ "

// Kích thước của uint16_t (thường là 2 byte)
#define SHELL_UART_UINT_SIZE 2

// Số lượng binding nội bộ của SHELL_UART
#define SHELL_UART_INTERNAL_BINDING_COUNT 3

// Số lượng binding tối đa do người dùng cấu hình
#define SHELL_UART_MAX_BINDING_COUNT 5

// Kích thước bộ đệm nhận (receive buffer) tính bằng ký tự
#define SHELL_UART_RX_BUFFER_SIZE 64

// Kích thước bộ đệm lệnh (command buffer) tính bằng ký tự
#define SHELL_UART_CMD_BUFFER_SIZE 32

// Kích thước bộ đệm lịch sử (history buffer) tính bằng ký tự
#define SHELL_UART_HISTORY_BUFFER_SIZE 128

// Macro chuyển đổi từ byte sang đơn vị uint16_t, làm tròn lên
#define BYTES_TO_SHELL_UART_UINTS(bytes) (((bytes) + SHELL_UART_UINT_SIZE - 1) / SHELL_UART_UINT_SIZE)

// Macro tính kích thước buffer cần thiết cho SHELL_UART (tính bằng byte)
#define SHELL_UART_BUFFER_SIZE 1024


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define for watchdog
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WDG_NUM_WDGS				10
#define WDG_INIT_TIMEOUT_MS		20000

//
#define WDG_CAUSE_RESET_NORMAL	0
#define WDG_CAUSE_RESET_OTA		1
#define WDG_CAUSE_RESET_WDG		2

#define WDG_ADC_MONITOR_ID		1		// OK
//#define WDG_EXPERIMENT_ID		2
#define WDG_MINSHELL_ID			3		// OK
//#define WDG_PHOTO_COOL_ID		4
//#define WDG_SHELL_ID			5
#define WDG_SPI_TRANSMIT_ID		6		// OK
#define WDG_SYSTEM_LOG_ID		7		// OK
#define WDG_TEMP_CTRL_ID		8		// OK

#define WDG_ADC_MONITOR_TIMEOUT			300			//adc_monitor interval: 10
//#define WDG_EXPERIMENT_TIMEOUT		300
#define WDG_MINSHELL_TIMEOUT			300			//min_shell interval: 10
//#define WDG_PHOTO_COOL_TIMEOUT		7000
//#define WDG_SHELL_TIMEOUT				300
#define WDG_SPI_TRANSMIT_TIMEOUT		300			//spi_transmit: 5
#define WDG_SYSTEM_LOG_TIMEOUT			3000		//log interval: 1000
#define WDG_TEMP_CTRL_TIMEOUT			500			//temp_ctrl interval: 100


#endif /* CONFIGS_APP_CONFIGS_H_ */
