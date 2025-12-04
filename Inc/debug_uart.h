#ifndef DEBUG_UART_H
#define DEBUG_UART_H

#include <stdint.h>
#include "config.h"

#ifdef DEBUG_UART_ENABLE
void debug_uart_init(uint32_t baud);
void debug_uart_write(const char* msg);
#else
static inline void debug_uart_init(uint32_t baud) { (void)baud; }
static inline void debug_uart_write(const char* msg) { (void)msg; }
#endif

#endif /* DEBUG_UART_H */
