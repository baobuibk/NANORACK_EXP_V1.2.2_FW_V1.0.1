/*
 * uart_dbg.h
 *
 *  Created on: May 22, 2025
 *      Author: Admin
 */

#ifndef UART_DBG_H_
#define UART_DBG_H_


#include "uart_stdio.h"
#include "configs.h"
#include <stdint.h>

/* Debug Levels */
typedef enum {
    DBG_LEVEL_INFO,    // Informational messages
    DBG_LEVEL_WARNING, // Warning messages
    DBG_LEVEL_ERROR    // Error messages
} debug_level_t;

/* External UART instance for debug */
extern UART_stdio_t uart_stdio;

/* Debug Macro */

#ifdef DEBUG_ENABLE
    #define DBG(level, format, ...) do { \
        const char* prefix; \
        switch (level) { \
            case DBG_LEVEL_INFO:    prefix = "[INFO]"; break; \
            case DBG_LEVEL_WARNING: prefix = "[WARNING]"; break; \
            case DBG_LEVEL_ERROR:   prefix = "[ERROR]"; break; \
            default:                prefix = "[UNKNOWN]"; break; \
        } \
        uart_stdio_printf(&uart_stdio, "%s %s:%d: " format, \
                          prefix, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
    #define DBG(level, format, ...) ((void)0) // No-op if debug disabled
#endif

/* Convenience Macro for Simple Printing */
//#define Printf(format, ...) uart_stdio_printf(&min_shell_uart, format, ##__VA_ARGS__)

#endif /* STDIO_UART_DBG_H_ */
