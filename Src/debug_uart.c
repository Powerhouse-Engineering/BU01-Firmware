#include "debug_uart.h"

#ifdef DEBUG_UART_ENABLE

#include "main.h"

void debug_uart_init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Default to USART3 TX on PB10 to free PB6 for its original use.
       Define DEBUG_UART_USE_USART1 to keep the older PB6 (USART1 remap) path. */
#ifdef DEBUG_UART_USE_USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE); // TX on PB6
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    USART_TypeDef* uart = USART1;
#else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, DISABLE); // default PB10/11 mapping
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // TX
    USART_TypeDef* uart = USART3;
#endif

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    USART_Init(uart, &USART_InitStructure);
    USART_Cmd(uart, ENABLE);
}

void debug_uart_write(const char* msg)
{
#ifdef DEBUG_UART_USE_USART1
    USART_TypeDef* uart = USART1;
#else
    USART_TypeDef* uart = USART3;
#endif
    if (!msg) {
        return;
    }
    while (*msg) {
        while (USART_GetFlagStatus(uart, USART_FLAG_TC) == RESET) {
        }
        USART_SendData(uart, (uint8_t)*msg++);
    }
}

#endif
