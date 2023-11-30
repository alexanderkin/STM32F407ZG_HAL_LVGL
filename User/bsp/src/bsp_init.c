#include "bsp_init.h"

void bsp_Init(void)
{
    bsp_led_init();
    bsp_usart_init();
    LCD_DMA_Init();
    bsp_lcd_init();
    GTP_Init_Panel();
    bsp_Tim7_Init(1000);
}
