#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "main.h"

#define ON 1
#define OFF 0 

#define LED_GPIO_Port GPIOF
#define LED1_GPIO_Pin  GPIO_PIN_6
#define LED2_GPIO_Pin  GPIO_PIN_7
#define LED3_GPIO_Pin  GPIO_PIN_8

#define LED_GPIO_Mode  GPIO_MODE_OUTPUT_PP
#define LED_GPIO_Pull  GPIO_PULLUP
#define LED_GPIO_Speed GPIO_SPEED_FREQ_HIGH

#define LED_RED        LED1(ON); LED2(OFF);LED3(OFF)
#define LED_GREEN	   LED1(OFF);LED2(ON); LED3(OFF)
#define LED_BLUE	   LED1(OFF);LED2(OFF);LED3(ON)
#define LED_YELLOW	   LED1(ON); LED2(ON); LED3(OFF)
#define LED_PURPLE	   LED1(ON); LED2(OFF);LED3(ON)
#define LED_CYAN       LED1(OFF);LED2(ON); LED3(ON)
#define LED_WHITE	   LED1(ON); LED2(ON); LED3(ON)
#define LED_RGBOFF	   LED1(OFF);LED2(OFF);LED3(OFF)

void bsp_led_init(void);
void LED1(int status);
void LED2(int status);
void LED3(int status);
void LED1_Toggle(void);
void LED2_Toggle(void);
void LED3_Toggle(void);

#endif // !__BSP_LED_H__
