/*
 * shell.h
 *
 *  Created on: Jun 9, 2025
 *      Author: Admin
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "sst.h"
#include "fsm.h"
#include "embedded_cli.h"
#include "uart_stdio.h"


typedef struct shell_task_t shell_task_t;
typedef struct shell_evt_t shell_evt_t;
typedef struct shell_task_init_t shell_task_init_t;

typedef state_t (*shell_state_handler_t)(struct shell_task_t * const me, shell_evt_t const * const e);
struct shell_task_t {
    SST_Task super;
    shell_state_handler_t state; /* the "state variable" */
    UART_stdio_t * shell_uart_stdio;
    EmbeddedCli * shell_uart_cli ;
    SST_TimeEvt shell_task_timeout_timer;
    uint16_t * buffer_to_send;
    uint32_t remain_word;
    uint8_t  htoa_buffer[5];
    uint8_t  htoa_buffer_index;
    uint8_t  bin_buffer[2];
	uint8_t  bin_buffer_index;
    uint16_t crc;
} ;

struct shell_task_init_t{
	shell_state_handler_t init_state; /* the "init state variable" */
    EmbeddedCli * shell_uart_cli;
    UART_stdio_t * shell_uart_stdio;
    shell_evt_t const * current_evt;
    circular_buffer_t * shell_task_event_buffer;
} ;

struct shell_evt_t {
    SST_Evt super;
} ;

void shell_task_ctor_singleton(void);
void shell_task_start(uint8_t priority);
void shell_task_ctor(shell_task_t * const me, shell_task_init_t * const init) ;
void CLI_UART_stdio_rx_callback();

void shell_send_buffer(shell_task_t * const me, uint16_t *buffer, uint32_t size, uint8_t mode);

void shell_uart_send_buffer_bin_evt(shell_task_t * const me);
void shell_uart_send_crc_evt(shell_task_t * const me);

#endif /* APP_SHELL_SHELL_H_ */
