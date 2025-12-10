#ifndef __CONFIG_H__
#define __CONFIG_H__

/* Main configuration */
#define ORBT_ESC_V203
#define USE_BUTTON_UI

// enable simple UART debug on PB6 (USART1 remap)
// #define DEBUG_UART_ENABLE //Blocking implementation
/* Product variant selection */
#define BU01_INT 0
#define BU01_EXT 1
#ifndef BU01_PRODUCT
#define BU01_PRODUCT (BU01_INT)
#endif

/* Debug */


#endif
