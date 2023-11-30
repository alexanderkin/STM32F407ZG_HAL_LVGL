#include "bsp_led.h"

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOF_CLK_ENABLE();

    GPIO_InitStruct.Pin   = LED1_GPIO_Pin | LED2_GPIO_Pin | LED3_GPIO_Pin;
    GPIO_InitStruct.Mode  = LED_GPIO_Mode;
    GPIO_InitStruct.Pull  = LED_GPIO_Pull;
    GPIO_InitStruct.Speed = LED_GPIO_Speed;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LED_GPIO_Port, LED1_GPIO_Pin | LED2_GPIO_Pin | LED3_GPIO_Pin, GPIO_PIN_SET);
}

void LED1(int status)
{
    if(status == 0)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_SET);
    }
    else if(status == 1)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED1_GPIO_Pin, GPIO_PIN_RESET);
    }
}

void LED2(int status)
{
    if(status == 0)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED2_GPIO_Pin, GPIO_PIN_SET);
    }
    else if(status == 1)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED2_GPIO_Pin, GPIO_PIN_RESET);
    }
}

void LED3(int status)
{
    if(status == 0)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED3_GPIO_Pin, GPIO_PIN_SET);
    }
    else if(status == 1)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED3_GPIO_Pin, GPIO_PIN_RESET);
    }
}

void LED1_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED1_GPIO_Pin);
}

void LED2_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED2_GPIO_Pin);
}

void LED3_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED3_GPIO_Pin);
}
