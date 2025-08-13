/*
 * shell.c
 *
 *  Created on: Jun 9, 2025
 *      Author: Admin
 */
#include "board.h"
#include "shell.h"
#include "stddef.h"
#include "app_signals.h"
#include "cli_command.h"
#include "circular_buffer.h"
#include "error_codes.h"
#include "experiment_task.h"

#include "dbc_assert.h"
// Expand cli implementation here (must be in one file only)
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

DBC_MODULE_NAME("shell")

#define SHELL_TASK_NUM_EVENTS		30
shell_task_t shell_task_inst ;

static shell_evt_t shell_current_event = {0}; // Current event being processed
static shell_evt_t shell_task_event_buffer[SHELL_TASK_NUM_EVENTS] = {0}; // Array to hold shell events
static shell_evt_t const entry_evt = {.super = {.sig = SIG_ENTRY} };
static shell_evt_t const exit_evt = {.super = {.sig = SIG_EXIT} };
static shell_evt_t const uart_empty_evt = {.super = {.sig = EVT_SHELL_UART_EMPTY},									};
static shell_evt_t const uart_send_buffer_evt = {.super = {.sig = EVT_SHELL_SEND_BUFFER},};
static shell_evt_t const uart_send_chunk_evt = {.super = {.sig = EVT_SHELL_SEND_CHUNK},};
static shell_evt_t const uart_send_buffer_bin_evt = {.super = {.sig = EVT_SHELL_SEND_BUFFER_BINARY},};
static shell_evt_t const uart_send_crc_evt = {.super = {.sig = EVT_SHELL_SEND_CRC},};

circular_buffer_t shell_task_event_queue = {0}; // Circular buffer to hold shell events

extern experiment_task_t experiment_task_inst;
static experiment_task_t *p_experiment_task = &experiment_task_inst;

// UART_STDIO
#define SHELL_UART_RX_BUFFER_SIZE 1024
#define SHELL_UART_TX_BUFFER_SIZE 4*1024

UART_stdio_t uart_stdio;
circular_char_buffer_t rx_buffer;
circular_char_buffer_t tx_buffer;
uint8_t rx_static_buffer[SHELL_UART_RX_BUFFER_SIZE];
uint8_t tx_static_buffer[SHELL_UART_TX_BUFFER_SIZE];

static bool enable_shell_empty = false;

#define SHELL_UART_CLI_BUFFER_SIZE			32768

//#define SHELL_UART_CLI_BUFFER_SIZE			4096
#define SHELL_UART_CLI_RX_BUFFER_SIZE		128
#define SHELL_UART_CLI_CMD_BUFFER_SIZE		64
#define SHELL_UART_CLI_HISTORY_BUFFER_SIZE	256

#define SHELL_POLL_INTERVAL 				10

#define SHELL_UART_INITATION		    	"EXP $ "

EmbeddedCli * shell_uart_cli = NULL;

//static EmbeddedCli * shell_uart_cli = NULL;
static CLI_UINT  shell_buffer[BYTES_TO_CLI_UINTS(SHELL_UART_CLI_BUFFER_SIZE * sizeof(char))];

static uint32_t shell_cli_init(void);
static void shell_cli_write_char(EmbeddedCli *embeddedCli, char c) ;
static void shell_task_init(shell_task_t * const me, shell_evt_t const * const e) ;
static void shell_task_dispatch(shell_task_t * const me, shell_evt_t const * const e);

static state_t shell_state_process_handler(shell_task_t * const me, shell_evt_t const * const e);
static state_t shell_state_send_long_buffer_handler(shell_task_t * const me, shell_evt_t const * const e);
static state_t shell_state_send_long_buffer_binary_handler(shell_task_t * const me, shell_evt_t const * const e);
static void shell_htoa(shell_task_t * const me);
static void shell_binary(shell_task_t * const me);
static void crc16_CCITT_update(uint16_t *crc, uint16_t data);

// Khởi tạo bộ đệm vòng (item_size = 1 byte)
void shell_stdio_init(void)
{
	circular_char_buffer_init(&rx_buffer, rx_static_buffer, SHELL_UART_RX_BUFFER_SIZE);
	circular_char_buffer_init(&tx_buffer, tx_static_buffer,  SHELL_UART_TX_BUFFER_SIZE);

    // Khởi tạo UART_Stdio
    uart_stdio_init(&uart_stdio, CLI_UART, &rx_buffer, &tx_buffer);
}

static uint32_t shell_cli_init(void) {
    // Initialize the CLI UART with the specified parameters

    EmbeddedCliConfig cli_config = {
        .invitation = SHELL_UART_INITATION,
        .rxBufferSize = SHELL_UART_RX_BUFFER_SIZE,
        .cmdBufferSize = SHELL_UART_CLI_CMD_BUFFER_SIZE,
        .historyBufferSize = SHELL_UART_CLI_HISTORY_BUFFER_SIZE,
        .maxBindingCount = 0,
        .cliBuffer = shell_buffer,
        .cliBufferSize = sizeof(shell_buffer),
        .enableAutoComplete = 1,
        .staticBindings = NULL, // No static bindings for now
        .staticBindingCount = 0
    };
    cli_config.staticBindings = getCliStaticBindings();
    cli_config.staticBindingCount = getCliStaticBindingCount();
    shell_uart_cli = embeddedCliNew(&cli_config);
    if (shell_uart_cli == NULL) {
        DBC_ASSERT(1u, 0); // Initialization failed
        return ERROR_FAIL; // Indicate failure
    }
    shell_uart_cli->writeChar = shell_cli_write_char;
    cli_printf(shell_uart_cli,"required size = %d \n\r", embeddedCliRequiredSize(&cli_config));
    return ERROR_OK; // Indicate success
}
static void shell_cli_write_char(EmbeddedCli *embeddedCli, char c) {
	uart_stdio_write_char(&uart_stdio, (uint8_t)c);
}
void shell_task_ctor(shell_task_t * const me, shell_task_init_t * const init) {
    DBC_ASSERT(0u, me != NULL);
    SST_Task_ctor(&me->super, (SST_Handler) shell_task_init, (SST_Handler)shell_task_dispatch, \
                                (SST_Evt *)init->current_evt, init->shell_task_event_buffer);
    SST_TimeEvt_ctor(&me->shell_task_timeout_timer, EVT_SHELL_POLL_TIME, &(me->super));
    me->shell_uart_cli = init->shell_uart_cli; // Initialize to NULL or a valid pointer if needed
    me->shell_uart_stdio = init->shell_uart_stdio; // Initialize to NULL or a valid pointer if needed
    me->state = init->init_state; // Set the initial state to process handler
    SST_TimeEvt_disarm(&me->shell_task_timeout_timer); // Disarm the timeout timer
}
void shell_task_ctor_singleton() {
	shell_stdio_init();
	circular_buffer_init(&shell_task_event_queue,(uint8_t *)&shell_task_event_buffer,sizeof(shell_task_event_buffer),SHELL_TASK_NUM_EVENTS,sizeof(shell_evt_t));
	shell_cli_init();
	shell_task_init_t shell_task_init = {
        .init_state = shell_state_process_handler, // Set the initial state to process handler
        .shell_uart_cli = shell_uart_cli, // Initialize to NULL or a valid pointer if needed
        .shell_uart_stdio = &uart_stdio, // Initialize to NULL or a valid pointer if needed
        .current_evt = &shell_current_event, // Pointer to the current event being processed
        .shell_task_event_buffer = &shell_task_event_queue // Pointer to the circular buffer for events
    };
    shell_task_ctor(&shell_task_inst, &shell_task_init);

}
void shell_task_start(uint8_t priority)
{
	SST_Task_start(&shell_task_inst.super,priority);
}
void shell_task_init(shell_task_t * const me, shell_evt_t const * const e) {
    DBC_ASSERT(7u, me != NULL);
    uart_stdio_active(me->shell_uart_stdio);
    SST_TimeEvt_arm(&me->shell_task_timeout_timer, SHELL_POLL_INTERVAL, SHELL_POLL_INTERVAL);

    // Initialize the shell task
    // Set the initial state to process handler
    // Optionally, you can initialize other components or bindings here

    cli_printf(shell_uart_cli, "\r\n========================================\r\n");
    cli_printf(shell_uart_cli, "          EXPERIMENT BOARD \r\n");
    cli_printf(shell_uart_cli, "                 V122 \r\n");
    cli_printf(shell_uart_cli, "========================================\r\n");
}

static void shell_task_dispatch(shell_task_t * const me, shell_evt_t const * const e)
{
	shell_state_handler_t prev_state = me->state; /* save for later */
    state_t status = (me->state)(me, e);

    if (status == TRAN_STATUS) { /* transition taken? */
        (prev_state)(me, &exit_evt);
        (me->state)(me, &entry_evt);
    }
}
static state_t shell_state_process_handler(shell_task_t * const me, shell_evt_t const * const e) {
    DBC_ASSERT(3u,me != NULL);
    DBC_ASSERT(4u,e != NULL);

    switch (e->super.sig) {
    	case SIG_ENTRY:
    	    SST_TimeEvt_arm(&me->shell_task_timeout_timer, SHELL_POLL_INTERVAL, SHELL_POLL_INTERVAL);
    	    return HANDLED_STATUS;
    	case SIG_EXIT:
    		SST_TimeEvt_disarm(&me->shell_task_timeout_timer);
    		return HANDLED_STATUS;
    	case EVT_SHELL_POLL_TIME:
    	{
            // Handle initialization event
        	embeddedCliProcess(shell_uart_cli);
            return HANDLED_STATUS;
        }
        case EVT_SHELL_SEND_BUFFER: {
            // Handle initialization event
        	me->state = shell_state_send_long_buffer_handler;
            return TRAN_STATUS;
        }
        case EVT_SHELL_SEND_BUFFER_BINARY: {
			// Handle initialization event
			me->state = shell_state_send_long_buffer_binary_handler;
			return TRAN_STATUS;
		}

        default: {
            // Handle other events if necessary
            return IGNORED_STATUS;
        }
    }
}


// Callback xử lý ngắt nhận
void CLI_UART_stdio_rx_callback() {
    if (LL_USART_IsActiveFlag_RXNE(CLI_UART)) {
        uint8_t received_data = LL_USART_ReceiveData8(CLI_UART);
        embeddedCliReceiveChar(shell_uart_cli, received_data);
     }
}

void CLI_UART_stdio_tx_callback(shell_task_t * const me) {

	uint32_t ret;
	ret = uart_stdio_tx_callback(me->shell_uart_stdio);
//	ret = uart_stdio_tx_callback(&uart_stdio);

	if (ret && enable_shell_empty) SST_Task_post(&me->super, (SST_Evt *)&uart_empty_evt);
}


// USART6_IRQHandler
void USART6_IRQHandler(void)
{
	CLI_UART_stdio_rx_callback();
	CLI_UART_stdio_tx_callback(&shell_task_inst);
	USART6->ICR = 0xFFFFFFFF;
}

static state_t shell_state_send_long_buffer_handler(shell_task_t * const me, shell_evt_t const * const e) {

	uint32_t ret = 0;
    switch (e->super.sig) {
        case SIG_ENTRY: {
            // send first chunk of data
        	while (me->remain_word > 0)
        	{
        		shell_htoa(me);
        		while (me->htoa_buffer_index < 5)
        		{
        			ret = uart_stdio_write_char(me->shell_uart_stdio, me->htoa_buffer[me->htoa_buffer_index]);
        			if (ret)  //buffer full, need to wait
        			{
        				return HANDLED_STATUS;
        			}
        			me->htoa_buffer_index++;
        		}
        	}
        }
        case EVT_SHELL_UART_EMPTY:
    		while (me->htoa_buffer_index < 5)	//print the rest of previous htoa buffer
    		{
    			ret = uart_stdio_write_char(me->shell_uart_stdio, me->htoa_buffer[me->htoa_buffer_index]);
    			if (ret)  //buffer full, need to wait
    			{
    				return HANDLED_STATUS;
    			}
    			me->htoa_buffer_index++;
    		}
        	while (me->remain_word > 0)
        	{
        		shell_htoa(me);
        		while (me->htoa_buffer_index < 5)
        		{
        			ret = uart_stdio_write_char(me->shell_uart_stdio, me->htoa_buffer[me->htoa_buffer_index]);
        			if (ret)  //buffer full, need to wait
        			{
        				return HANDLED_STATUS;
        			}
        			me->htoa_buffer_index++;
        		}
        	}

        default:
            // Handle other events if necessary
            return IGNORED_STATUS;
    }
}


static state_t shell_state_send_long_buffer_binary_handler(shell_task_t * const me, shell_evt_t const * const e)
{
	uint8_t bytes_temp[3];
    switch (e->super.sig)
    {
        case SIG_ENTRY:
        {
        	// Send 3 byte header
        	uint32_t header = (0x003FFFFF & (experiment_task_inst.data_profile.num_data * 2)) | 0xFFC00000;
			bytes_temp[0] = (uint8_t)(header >> 16);
			bytes_temp[1] = (uint8_t)(header >> 8);
			bytes_temp[2] = (uint8_t)header;
			for(uint8_t i = 0; i < 3; i++)
			{
				uart_stdio_write_char(me->shell_uart_stdio, bytes_temp[i]);
			}

			// Create event if done send evt
			experiment_task_done_send_header_evt(p_experiment_task);
			return HANDLED_STATUS;
        }


        case EVT_SHELL_SEND_CHUNK:
        {
        	while (me->remain_word > 0)
			{
				shell_binary(me);
				while (me->bin_buffer_index < 2)
				{
					if(uart_stdio_write_char(me->shell_uart_stdio, me->bin_buffer[me->bin_buffer_index]))	//buffer full, need to wait
					{
						enable_shell_empty = true;
						return HANDLED_STATUS;
					}

					me->bin_buffer_index++;
				}
			}
			// Create event if done send chunk and have other chunk
			experiment_task_done_send_chunk(p_experiment_task);
			return HANDLED_STATUS;
        }


        case EVT_SHELL_UART_EMPTY:
        {
        	// print pre buffer due to full
        	while (me->bin_buffer_index < 2)
			{
				if(uart_stdio_write_char(me->shell_uart_stdio, me->bin_buffer[me->bin_buffer_index]))		// if buffer full
				{
					return HANDLED_STATUS;
				}
				me->bin_buffer_index++;

				// Create event if done send chunk
				if(!me->remain_word)
				{
					enable_shell_empty = false;
					experiment_task_done_send_chunk(p_experiment_task);
				}
			}

        	// print continue word of chunk
        	while (me->remain_word > 0)
			{
				shell_binary(me);
				while (me->bin_buffer_index < 2)
				{
					if(uart_stdio_write_char(me->shell_uart_stdio, me->bin_buffer[me->bin_buffer_index]))	//buffer full, need to wait
					{
						return HANDLED_STATUS;
					}
					me->bin_buffer_index++;
				}

	        	// Create event if done send chunk
				if(!me->remain_word)
				{
					enable_shell_empty = false;
					experiment_task_done_send_chunk(p_experiment_task);
				}
			}


			return HANDLED_STATUS;
        }

        case EVT_SHELL_SEND_CRC:
        {
        	bytes_temp[0] = (uint8_t)(me->crc >> 8);
        	bytes_temp[1] = (uint8_t)me->crc;
        	for(uint8_t i = 0 ; i < 2; i++)
			{
				uart_stdio_write_char(me->shell_uart_stdio, bytes_temp[i]);
			}
			me->state = shell_state_process_handler;
			return TRAN_STATUS;
        }


        default:
            return IGNORED_STATUS;
    }
}

static void crc16_CCITT_update(uint16_t *crc, uint16_t data)
{
    uint8_t bytes[2] = {data >> 8, data & 0xFF};
    for (uint8_t i = 0; i < 2; i++)
    {
        *crc ^= bytes[i] << 8;
        for (uint8_t j = 0; j < 8; j++)
            *crc = (*crc & 0x8000) ? (*crc << 1) ^ 0x1021 : *crc << 1;
    }
}

static void shell_binary(shell_task_t * const me)
{
	uint16_t data = *me->buffer_to_send++;
	me->remain_word --;
	crc16_CCITT_update(&me->crc, data);
	me->bin_buffer[0] = data >> 8;
	me->bin_buffer[1] = data & 0xFF;
	me->bin_buffer_index = 0;
}

static void shell_htoa(shell_task_t * const me)
{
    const char hex_chars[] = "0123456789ABCDEF";
    uint16_t data = *me->buffer_to_send++;
    me->remain_word --;
    me->htoa_buffer[0] = hex_chars[(data >> 12) & 0x0F]; // Nibble cao nhất (bit 15-12)
	me->htoa_buffer[1] = hex_chars[(data >> 8) & 0x0F];  // Nibble thứ hai (bit 11-8)
	me->htoa_buffer[2] = hex_chars[(data >> 4) & 0x0F];  // Nibble thứ ba (bit 7-4)
	me->htoa_buffer[3] = hex_chars[data & 0x0F];         // Nibble thấp nhất (bit 3-0)
	me->htoa_buffer[4] = ' ';
    me->htoa_buffer_index = 0;
    if (!me->remain_word)
    	experiment_task_done_send_chunk(p_experiment_task);
}

void shell_send_buffer(shell_task_t * const me, uint16_t *buffer, uint32_t size, uint8_t mode)
{
	me->buffer_to_send = buffer;
	me->remain_word = size;

	if(mode)
	{
		SST_Task_post(&me->super, (SST_Evt *)&uart_send_chunk_evt);
	}
	else SST_Task_post(&me->super, (SST_Evt *)&uart_send_buffer_evt);
}

void shell_uart_send_buffer_bin_evt(shell_task_t * const me)
{
	SST_Task_post(&me->super, (SST_Evt *)&uart_send_buffer_bin_evt);
}

void shell_uart_send_crc_evt(shell_task_t * const me)
{
	SST_Task_post(&me->super, (SST_Evt *)&uart_send_crc_evt);
}

