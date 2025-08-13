/*
 * uart.h
 *
 *  Created on: Feb 25, 2025
 *      Author: CAO HIEU
 */

#define USE_NODMA_UART
#ifdef USE_NODMA_UART

#ifndef M1_DRIVERS_UART_UART_DRIVER_H_
#define M1_DRIVERS_UART_UART_DRIVER_H_

#include "stm32f7xx_ll_usart.h"
#include "RingBuffer/ring_buffer.h"
#include "basetypedef.h"
#include <stdint.h>

#define UART_DRIVER_COUNT    1

#define UART6_BUFFER_SIZE    1024

// Driver UART-Ringbuffer Struct
typedef struct {
    USART_TypeDef *uart;
    s_RingBufferType rx_buffer;  // Ringbuffer Rx
    s_RingBufferType tx_buffer;  // Ringbuffer Tx
} UART_Driver_t;

Std_ReturnType UART_Driver_Init(void);
void UART_Driver_ISR(USART_TypeDef *uart);
int  UART_Driver_Read(USART_TypeDef *uart);
void UART_Driver_Write(USART_TypeDef *uart, uint8_t data);
void UART_Driver_SendString(USART_TypeDef *uart, const char *str);
_Bool UART_Driver_IsDataAvailable(USART_TypeDef *uart);
uint16_t UART_Driver_TXNumFreeSlots(USART_TypeDef *uart);
uint16_t UART_Driver_RXNumFreeSlots(USART_TypeDef *uart);
// --- API: Flush Rx/Tx or Both ---
void UART_Driver_FlushRx(USART_TypeDef *uart);
void UART_Driver_FlushTx(USART_TypeDef *uart);
void UART_Driver_Flush(USART_TypeDef *uart);

#endif

#endif /* M1_DRIVERS_UART_UART_DRIVER_H_ */
