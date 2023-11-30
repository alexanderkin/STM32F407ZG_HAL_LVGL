#include "bsp_usart.h"

UART_HandleTypeDef UART1_Handle;

#if !defined(__MICROLIB)

__asm(".global __use_no_semihosting");
void _sys_exit(int x)
{
  x = x;
}
void _ttywrch(int ch)
{
  ch = ch;
}
FILE __stdout;

#endif

#if defined (__GNUC__) && !defined (__clang__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&UART1_Handle, (uint8_t *)&ch, 1 , 0xFFFF);
    return ch;
}

void bsp_usart_init(void)
{
    UART1_Handle.Instance          = USART1;
    UART1_Handle.Init.BaudRate     = UART1_BaudRate;
    UART1_Handle.Init.WordLength   = UART1_WordLength;
    UART1_Handle.Init.StopBits     = UART1_StopBits;
    UART1_Handle.Init.Parity       = UART1_Parity;
    UART1_Handle.Init.Mode         = UART1_Mode;
    UART1_Handle.Init.HwFlowCtl    = UART1_HwFlowCtl;
    UART1_Handle.Init.OverSampling = UART1_OverSampling;
    HAL_UART_Init(&UART1_Handle);

    __HAL_UART_ENABLE_IT(&UART1_Handle, UART_IT_RXNE);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitStruct.Pin       = UART1_GPIO_Tx_Pin | UART1_GPIO_Rx_Pin;
        GPIO_InitStruct.Mode      = UART1_GPIO_Tx_Rx_Mode;
        GPIO_InitStruct.Pull      = UART1_GPIO_Tx_Rx_Pull;
        GPIO_InitStruct.Speed     = UART1_GPIO_Tx_Rx_Speed;
        GPIO_InitStruct.Alternate = UART1_GPIO_Tx_Rx_Alternate;
        HAL_GPIO_Init(UART1_GPIO_Tx_Rx_Port, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(UART1_IRQ, 0, 1);
        HAL_NVIC_EnableIRQ(UART1_IRQ);
    }
}

void USART1_IRQHandler(void)
{
    uint8_t ch = 0;

    if(__HAL_UART_GET_FLAG(&UART1_Handle, UART_FLAG_RXNE) != RESET)
    {
        ch = (uint16_t)READ_REG(UART1_Handle.Instance->DR);
        WRITE_REG(UART1_Handle.Instance->DR, ch);
    }
}
