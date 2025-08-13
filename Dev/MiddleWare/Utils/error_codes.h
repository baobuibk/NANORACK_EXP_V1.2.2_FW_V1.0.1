/*
 * error_codes.h
 *
 *  Created on: May 14, 2025
 *      Author: Admin
 */

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

// Mã lỗi chung (0x00 - 0x0F)
#define ERROR_OK                0x00U  // Thành công
#define ERROR_FAIL              0x01U  // Lỗi chung
#define ERROR_TIMEOUT           0x02U  // Hết thời gian chờ
#define ERROR_INVALID_PARAM     0x03U  // Tham số không hợp lệ
#define ERROR_NOT_READY         0x04U  // Thiết bị chưa sẵn sàng
#define ERROR_BUSY              0x05U  // Thiết bị đang bận
#define ERROR_NOT_SUPPORTED     0x06U  // Chức năng không được hỗ trợ
#define ERROR_OUT_OF_MEMORY     0x07U  // Hết bộ nhớ



#define ERROR_BUFFER_FULL         0x08U  // Bộ đệm đầy
#define ERROR_BUFFER_EMPTY        0x09U  // Bộ đệm rỗng


// Mã lỗi cho ADC (0x10 - 0x1F)
#define ERROR_ADC_OVERRUN       0x10U  // Lỗi overrun (dữ liệu bị ghi đè)
#define ERROR_ADC_NOT_INIT      0x11U  // ADC chưa được khởi tạo
#define ERROR_ADC_CHANNEL       0x12U  // Kênh ADC không hợp lệ
#define ERROR_ADC_TIMEOUT       0x13U  // Timeout khi chuyển đổi ADC
#define ERROR_ADC_CALIBRATION   0x14U  // Lỗi hiệu chuẩn ADC

// Mã lỗi cho UART/USART (0x20 - 0x2F)
#define ERROR_UART_OVERRUN      0x20U  // Lỗi overrun (dữ liệu bị ghi đè)
#define ERROR_UART_NOT_INIT     0x21U  // UART chưa được khởi tạo
#define ERROR_UART_FRAMING      0x22U  // Lỗi framing (dữ liệu không đúng định dạng)
#define ERROR_UART_PARITY       0x23U  // Lỗi parity
#define ERROR_UART_TIMEOUT      0x24U  // Timeout khi truyền/nhận
#define ERROR_UART_NOISE        0x25U  // Lỗi nhiễu trên đường truyền

// Mã lỗi cho SPI (0x30 - 0x3F)
#define ERROR_SPI_NOT_INIT      0x30U  // SPI chưa được khởi tạo
#define ERROR_SPI_TIMEOUT       0x31U  // Timeout khi truyền/nhận
#define ERROR_SPI_MODF          0x32U  // Lỗi mode fault (master/slave xung đột)
#define ERROR_SPI_OVERRUN       0x33U  // Lỗi overrun
#define ERROR_SPI_BUSY          0x34U  // SPI đang bận

// Mã lỗi cho I2C (0x40 - 0x4F)
#define ERROR_I2C_NOT_INIT      0x40U  // I2C chưa được khởi tạo
#define ERROR_I2C_TIMEOUT       0x41U  // Timeout khi truyền/nhận
#define ERROR_I2C_NACK          0x42U  // Lỗi không nhận ACK
#define ERROR_I2C_ARBITRATION   0x43U  // Lỗi mất arbitration
#define ERROR_I2C_BUS_ERROR     0x44U  // Lỗi bus I2C

// Mã lỗi cho GPIO (0x50 - 0x5F)
#define ERROR_GPIO_NOT_INIT     0x50U  // GPIO chưa được khởi tạo
#define ERROR_GPIO_INVALID_PIN  0x51U  // Chân GPIO không hợp lệ
#define ERROR_GPIO_MODE         0x52U  // Chế độ GPIO không hợp lệ

// Mã lỗi cho TIM (Timer) (0x60 - 0x6F)
#define ERROR_TIM_NOT_INIT      0x60U  // Timer chưa được khởi tạo
#define ERROR_TIM_CONFIG        0x61U  // Lỗi cấu hình timer
#define ERROR_TIM_TIMEOUT       0x62U  // Timeout khi chờ sự kiện timer

// Mã lỗi cho DMA (0x70 - 0x7F)
#define ERROR_DMA_NOT_INIT      0x70U  // DMA chưa được khởi tạo
#define ERROR_DMA_TRANSFER      0x71U  // Lỗi truyền DMA
#define ERROR_DMA_TIMEOUT       0x72U  // Timeout khi truyền DMA
#define ERROR_DMA_CONFIG        0x73U  // Lỗi cấu hình DMA



//*****************************************************************************
//
//! \addtogroup cmdline_api
//! @{
//
//*****************************************************************************
#define CMDLINE_OK        (0)
#define CMDLINE_BAD_CMD         (1)
#define CMDLINE_TOO_MANY_ARGS   (2)
#define CMDLINE_TOO_FEW_ARGS   (3)
#define CMDLINE_INVALID_ARG   (4)
#define CMDLINE_NOT_RETURN   (5)




#endif /* ERROR_CODES_H */


