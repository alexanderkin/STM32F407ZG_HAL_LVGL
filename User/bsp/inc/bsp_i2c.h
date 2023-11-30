#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#include "main.h"

/*设定使用的电容屏IIC设备地址*/
#define GTP_ADDRESS   0xBA

/*I2C引脚*/
#define GTP_I2C_SCL_PIN         GPIO_PIN_7                 
#define GTP_I2C_SCL_GPIO_PORT   GPIOD                       

#define GTP_I2C_SDA_PIN         GPIO_PIN_3                 
#define GTP_I2C_SDA_GPIO_PORT   GPIOD                     

/*复位引脚*/
#define GTP_RST_GPIO_PORT       GPIOD
#define GTP_RST_GPIO_PIN        GPIO_PIN_6
/*中断引脚*/
#define GTP_INT_GPIO_PORT       GPIOG
#define GTP_INT_GPIO_PIN        GPIO_PIN_8

//软件IIC使用的宏
/* SCL = 1 */
#define I2C_SCL_1()  HAL_GPIO_WritePin(GTP_I2C_SCL_GPIO_PORT, GTP_I2C_SCL_PIN, GPIO_PIN_SET)
/* SCL = 0 */
#define I2C_SCL_0()  HAL_GPIO_WritePin(GTP_I2C_SCL_GPIO_PORT, GTP_I2C_SCL_PIN, GPIO_PIN_RESET)		
/* SDA = 1 */
#define I2C_SDA_1()  HAL_GPIO_WritePin(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN, GPIO_PIN_SET)		
/* SDA = 0 */
#define I2C_SDA_0()  HAL_GPIO_WritePin(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN, GPIO_PIN_RESET)		

void Touch_I2C_Init(void);
void Touch_I2C_ResetChip(void);
void Touch_I2C_IRQEnable(void);
void Touch_I2C_IRQDisable(void);
uint32_t I2C_ReadBytes(uint8_t ClientAddr, uint8_t *pBuffer, uint16_t NumByteToRead);
uint32_t I2C_WriteBytes(uint8_t ClientAddr, uint8_t *pBuffer,  uint8_t NumByteToWrite);

#endif