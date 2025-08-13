#ifndef UART_STDIO_H
#define UART_STDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "board.h"
#include "circular_char_buffer.h"

// Cấu trúc class UART_stdio_t
typedef struct {
    USART_TypeDef* uart_x;       // Con trỏ tới UART hardware (LL)
    circular_char_buffer_t* rx_buffer; // Con trỏ tới bộ đệm vòng nhận
    circular_char_buffer_t* tx_buffer; // Con trỏ tới bộ đệm vòng phát
    bool tx_busy;                // Trạng thái đang truyền
    bool is_active;              // Trạng thái hoạt động của UART
} UART_stdio_t;

// Hàm khởi tạo
void uart_stdio_init(UART_stdio_t* me, USART_TypeDef* uart_x, circular_char_buffer_t* rx_buffer, circular_char_buffer_t* tx_buffer);

// Hàm kích hoạt UART
void uart_stdio_active(UART_stdio_t* me);

// Hàm hủy kích hoạt UART
void uart_stdio_deactive(UART_stdio_t* me);

// Hàm kiểm tra trạng thái hoạt động
bool uart_stdio_is_active(UART_stdio_t* me);

// Hàm đọc dữ liệu từ bộ đệm nhận
bool uart_stdio_read(UART_stdio_t* me, uint8_t* buffer, uint32_t* rec_num);

uint32_t uart_stdio_write_char(UART_stdio_t* me, const uint8_t character) ;
// Hàm ghi dữ liệu vào bộ đệm phát
uint32_t uart_stdio_write(UART_stdio_t* me, const uint8_t * buffer, uint32_t num_data);

// Hàm in chuỗi định dạng qua UART
uint32_t uart_stdio_printf(UART_stdio_t* me, const char * format, ...);

// Callback cho ngắt nhận (gọi từ UART ISR)
void uart_stdio_rx_callback(UART_stdio_t* me);

// Callback cho ngắt phát (gọi từ UART ISR)
uint32_t uart_stdio_tx_callback(UART_stdio_t* me);

#endif
