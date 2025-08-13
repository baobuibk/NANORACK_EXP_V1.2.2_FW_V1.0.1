/*
 * uart.c
 *
 *  Created on: Feb 25, 2025
 *      Author: CAO HIEU
 */

#include "uart_driver.h"
#include "stdbool.h"


#ifdef USE_NODMA_UART
// #define ATOMIC_BLOCK_START(uart) LL_USART_DisableIT_RXNE(uart)
// #define ATOMIC_BLOCK_END(uart)   LL_USART_EnableIT_RXNE(uart)

static RingBufElement uart6_rx_data[UART6_BUFFER_SIZE];
static RingBufElement uart6_tx_data[UART6_BUFFER_SIZE];

static UART_Driver_t uart_drivers[UART_DRIVER_COUNT] = {
    {USART6, {0}, {0}}
};

/*************************************************
 *                  HELPER                       *
 *************************************************/

static UART_Driver_t *UART_Driver_Get(USART_TypeDef *uart)
{
    for (int i = 0; i < UART_DRIVER_COUNT; i++)
    {
        if (uart_drivers[i].uart == uart)
        {
            return &uart_drivers[i];
        }
    }
    return NULL;
}

/*************************************************
 *                    Init                       *
 *************************************************/

Std_ReturnType UART_Driver_Init(void)
{
    RingBuffer_Create(&uart_drivers[0].rx_buffer, 1, "UART6_RX", uart6_rx_data, UART6_BUFFER_SIZE);
    RingBuffer_Create(&uart_drivers[0].tx_buffer, 2, "UART6_TX", uart6_tx_data, UART6_BUFFER_SIZE);

//    LL_USART_EnableIT_ERROR(USART6);
    LL_USART_EnableIT_RXNE(USART6);

    return E_OK;
}

/*************************************************
 *                     API                       *
 *************************************************/

int UART_Driver_Read(USART_TypeDef *uart)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return -1;

    RingBufElement data;
    if (RingBuffer_Get(&driver->rx_buffer, &data))
    {
        return data;
    }
    return -1;
}

void UART_Driver_Write(USART_TypeDef *uart, uint8_t data)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return;

    while (!RingBuffer_Put(&driver->tx_buffer, data))
    {
        // vTaskDelay(pdMS_TO_TICKS(1));
    }
    LL_USART_EnableIT_TXE(uart);
}

void UART_Driver_SendString(USART_TypeDef *uart, const char *str)
{
    while (*str)
    {
        UART_Driver_Write(uart, (uint8_t)(*str));
        str++;
    }
}

_Bool UART_Driver_IsDataAvailable(USART_TypeDef *uart)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return false;

    return RingBuffer_IsDataAvailable(&driver->rx_buffer);
}

uint16_t UART_Driver_TXNumFreeSlots(USART_TypeDef *uart)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return 0;

    return (uint16_t)RingBuffer_NumFreeSlots(&driver->tx_buffer);
}

uint16_t UART_Driver_RXNumFreeSlots(USART_TypeDef *uart)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return 0;

    return (uint16_t)RingBuffer_NumFreeSlots(&driver->rx_buffer);
}

void UART_Driver_FlushRx(USART_TypeDef *uart)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return;

    memset(driver->rx_buffer.buffer, 0, driver->rx_buffer.max_size);
    atomic_store_explicit(&driver->rx_buffer.head, 0U, memory_order_release);
    atomic_store_explicit(&driver->rx_buffer.tail, 0U, memory_order_release);
}

void UART_Driver_FlushTx(USART_TypeDef *uart)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return;

    memset(driver->tx_buffer.buffer, 0, driver->tx_buffer.max_size);
    atomic_store_explicit(&driver->tx_buffer.head, 0U, memory_order_release);
    atomic_store_explicit(&driver->tx_buffer.tail, 0U, memory_order_release);
}

void UART_Driver_Flush(USART_TypeDef *uart)
{
    UART_Driver_FlushRx(uart);
    UART_Driver_FlushTx(uart);
}

void UART_Driver_ISR(USART_TypeDef *uart)
{
    UART_Driver_t *driver = UART_Driver_Get(uart);
    if (driver == NULL)
        return;

    if ((LL_USART_IsActiveFlag_RXNE(uart) != RESET) &&
        (LL_USART_IsEnabledIT_RXNE(uart) != RESET))
    {
        uint8_t data = LL_USART_ReceiveData8(uart);

        if (LL_USART_IsActiveFlag_ORE(uart))
        {
            LL_USART_ClearFlag_ORE(uart);
        }
        if (LL_USART_IsActiveFlag_FE(uart))
        {
            LL_USART_ClearFlag_FE(uart);
        }
        if (LL_USART_IsActiveFlag_NE(uart))
        {
            LL_USART_ClearFlag_NE(uart);
        }
        RingBuffer_Put(&driver->rx_buffer, data);
    }

    if ((LL_USART_IsActiveFlag_TXE(uart) != RESET) &&
        (LL_USART_IsEnabledIT_TXE(uart) != RESET))
    {
        uint8_t tx_data;
        if (RingBuffer_Get(&driver->tx_buffer, &tx_data))
        {
            LL_USART_TransmitData8(uart, tx_data);
        }
        else
        {
            LL_USART_DisableIT_TXE(uart);
        }
    }
}
#endif


