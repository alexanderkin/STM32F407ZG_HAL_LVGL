#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#include "main.h"

#define UART1_BaudRate             115200
#define UART1_WordLength           UART_WORDLENGTH_8B
#define UART1_StopBits             UART_STOPBITS_1
#define UART1_Parity               UART_PARITY_NONE
#define UART1_Mode                 UART_MODE_TX_RX
#define UART1_HwFlowCtl            UART_HWCONTROL_NONE
#define UART1_OverSampling         UART_OVERSAMPLING_16

#define UART1_GPIO_Tx_Rx_Port      GPIOA
#define UART1_GPIO_Tx_Pin          GPIO_PIN_9
#define UART1_GPIO_Rx_Pin          GPIO_PIN_10
#define UART1_GPIO_Tx_Rx_Mode      GPIO_MODE_AF_PP
#define UART1_GPIO_Tx_Rx_Pull      GPIO_NOPULL
#define UART1_GPIO_Tx_Rx_Speed     GPIO_SPEED_FREQ_VERY_HIGH
#define UART1_GPIO_Tx_Rx_Alternate GPIO_AF7_USART1

#define UART1_IRQ                  USART1_IRQn

extern UART_HandleTypeDef UART1_Handle;

void bsp_usart_init(void);

#endif // !__BSP_USART_H__
