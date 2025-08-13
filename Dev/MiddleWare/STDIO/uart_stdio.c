#include "uart_stdio.h"
#include <stdio.h>
#include <string.h>
#include "error_codes.h"
#include "atomic.h"
// Kích thước bộ đệm tạm cho printf
#define PRINTF_BUFFER_SIZE 128

// Khởi tạo UART_stdio_t
void uart_stdio_init(UART_stdio_t* me, USART_TypeDef* uart_x, circular_char_buffer_t* rx_buffer, circular_char_buffer_t* tx_buffer) {
    me->uart_x = uart_x;
    me->rx_buffer = rx_buffer;
    me->tx_buffer = tx_buffer;
    me->tx_busy = false;
    me->is_active = false; // UART chưa được kích hoạt
}

// Kích hoạt UART
void uart_stdio_active(UART_stdio_t* me) {
    if (me->uart_x != NULL && !me->is_active) {
        // Bật UART
        LL_USART_Enable(me->uart_x);
        // Bật ngắt nhận (RXNE)
        LL_USART_EnableIT_RXNE(me->uart_x);
        me->is_active = true;
    }
}

// Hủy kích hoạt UART
void uart_stdio_deactive(UART_stdio_t* me) {
    if (me->uart_x != NULL && me->is_active) {
        // Tắt ngắt TXE và RXNE
        LL_USART_DisableIT_TXE(me->uart_x);
        LL_USART_DisableIT_RXNE(me->uart_x);
        // Tắt UART
        LL_USART_Disable(me->uart_x);
        me->tx_busy = false; // Đặt lại trạng thái truyền
        me->is_active = false;
    }
}

// Kiểm tra trạng thái hoạt động
bool uart_stdio_is_active(UART_stdio_t* me) {
    return me->is_active;
}

// Đọc dữ liệu từ bộ đệm nhận
bool uart_stdio_read(UART_stdio_t* me, uint8_t* buffer, uint32_t* rec_num) {
    if (!me->is_active) {
        *rec_num = 0;
        return false;
    }

    uint32_t count = 0;
    uint32_t result;

    while (count < *rec_num) {
        result = circular_char_buffer_pop(me->rx_buffer, &buffer[count]);
        if (result) {
            break;
        }
        count++;
    }
    *rec_num = count;
    return count > 0;
}

// Ghi dữ liệu vào bộ đệm phát
uint32_t uart_stdio_write(UART_stdio_t* me, const uint8_t * buffer, uint32_t num_data) {
	uint8_t data;
    uint32_t count = 0;
    uint32_t ui32Ret = ERROR_OK;
	if (!me->is_active) {
        return ERROR_NOT_READY;
    }
    // Nếu không có truyền đang diễn ra, kích hoạt truyền
//	 __disable_irq();
	ENTER_CRITICAL();
    if (!me->tx_busy) {

        data = buffer[0];
        LL_USART_TransmitData8(me->uart_x, data);
        me->tx_busy = true;
        LL_USART_EnableIT_TXE(me->uart_x); // Bật ngắt TXE

        count ++;
    }
    // Thêm dữ liệu  vào bộ đệm
    while (count < num_data ) {
    	ui32Ret = circular_char_buffer_push(me->tx_buffer, buffer[count]);
        if (ui32Ret) {
            break;
        }
        count++;
    }
//    __enable_irq();
    EXIT_CRITICAL();
    return ui32Ret;
}


// Ghi dữ liệu vào bộ đệm phát
uint32_t uart_stdio_write_char(UART_stdio_t* me, const uint8_t character) {
uint32_t result = ERROR_OK;
	if (!me->is_active) {
        return ERROR_NOT_READY;
    }
    // Nếu không có truyền đang diễn ra, kích hoạt truyền
	ENTER_CRITICAL();
    if (!me->tx_busy) {

        LL_USART_TransmitData8(me->uart_x, character);
        me->tx_busy = true;
        LL_USART_EnableIT_TXE(me->uart_x); // Bật ngắt TXE
    }
    else
    // Thêm dữ liệu  vào bộ đệm
     {
        result = circular_char_buffer_push(me->tx_buffer, character);
        if (result) {
            result = ERROR_OUT_OF_MEMORY;
        }
    }
    EXIT_CRITICAL();
    return result;
}

// In chuỗi định dạng qua UART
uint32_t uart_stdio_printf(UART_stdio_t* me, const char * format, ...) {
    if (!me->is_active) {
        return 0;
    }

    char temp_buffer[PRINTF_BUFFER_SIZE];
    va_list args;

    // Khởi tạo danh sách tham số biến đổi
    va_start(args, format);

    // Định dạng chuỗi vào bộ đệm tạm
    int len = vsnprintf(temp_buffer, PRINTF_BUFFER_SIZE, format, args);

    // Kết thúc danh sách tham số
    va_end(args);

    // Kiểm tra độ dài hợp lệ
    if (len < 0 || len >= PRINTF_BUFFER_SIZE) {
        return 0;
    }

    // Gửi chuỗi đã định dạng qua UART
    return uart_stdio_write(me, (uint8_t *)temp_buffer, (uint32_t)len);
}

// Callback xử lý ngắt nhận
void uart_stdio_rx_callback(UART_stdio_t* me) {
    if (LL_USART_IsActiveFlag_RXNE(me->uart_x) && me->is_active) {
        uint8_t received_data = LL_USART_ReceiveData8(me->uart_x);
        circular_char_buffer_push(me->rx_buffer, received_data);
    }
//    LL_USART_ClearFlag_ORE(me->uart_x);
    if (LL_USART_IsActiveFlag_ORE(me->uart_x))
    	LL_USART_ClearFlag_ORE(me->uart_x);
}

// Callback xử lý ngắt phát
uint32_t uart_stdio_tx_callback(UART_stdio_t* me) {
    uint8_t data;
    uint32_t result = ERROR_OK;

    if (LL_USART_IsActiveFlag_TXE(me->uart_x) && me->is_active) {

        result = circular_char_buffer_pop(me->tx_buffer, &data);
        if (!result) {
            // Tiếp tục truyền byte tiếp theo
            LL_USART_TransmitData8(me->uart_x, data);
        } else {
            // Không còn dữ liệu, tắt ngắt TXE và đặt trạng thái không bận
            LL_USART_DisableIT_TXE(me->uart_x);
            me->tx_busy = false;
        }
    }
    return result;
}
