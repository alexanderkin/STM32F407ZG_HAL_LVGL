#include "bsp_i2c.h"
#include "bsp_tim.h"

static void Touch_I2C_GPIO_Config(void);
static void I2C_Delay(void);
static void I2C_Start(void);
static void I2C_Stop(void);
static void I2C_Ack(void);
static void I2C_NAck(void);
static uint32_t I2C_WaitAck(void);
static void I2C_SendByte(uint8_t _data);
static uint8_t I2C_ReadByte(void);

static void Touch_I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStructure.Pin   = GTP_I2C_SCL_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GTP_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GTP_I2C_SDA_PIN;
    HAL_GPIO_Init(GTP_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin   = GTP_RST_GPIO_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GTP_RST_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GTP_INT_GPIO_PIN;
    HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
}

static void I2C_Start(void)
{
    I2C_SDA_1();
	I2C_SCL_1();
	Delay_us(5);
	I2C_SDA_0();
	Delay_us(5);
	I2C_SCL_0();
	Delay_us(5);
}

static void I2C_Stop(void)
{
	I2C_SDA_0();
	I2C_SCL_1();
	Delay_us(5);
	I2C_SDA_1();
}

static void I2C_Ack(void)
{
	I2C_SDA_0();
	Delay_us(5);
	I2C_SCL_1();
	Delay_us(5);
	I2C_SCL_0();
	Delay_us(5);
	I2C_SDA_1();
}

static void I2C_NAck(void)
{
	I2C_SDA_1();
	Delay_us(5);
	I2C_SCL_1();
	Delay_us(5);
	I2C_SCL_0();
	Delay_us(5);	
}

static uint32_t I2C_WaitAck(void)
{
	uint32_t res = 0;

	I2C_SDA_1();
	Delay_us(5);
	I2C_SCL_1();
	Delay_us(5);
	if (HAL_GPIO_ReadPin(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN))
		res = 1;
	else
		res = 0;
	I2C_SCL_0();
	Delay_us(5);
	return res;
}

static void I2C_SendByte(uint8_t _data)
{
	uint32_t i = 0;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++) {		
		if (_data & 0x80)
			I2C_SDA_1();
		else
			I2C_SDA_0();
		Delay_us(5);
		I2C_SCL_1();
		Delay_us(5);	
		I2C_SCL_0();
		if (i == 7)
			 I2C_SDA_1(); // 释放总线
		_data <<= 1;	/* 左移一个bit */
		Delay_us(5);
	}
}

static uint8_t I2C_ReadByte(void)
{
	uint32_t i = 0;
	uint8_t value = 0;

	/* 读到第1个bit为数据的bit7 */
	for (i = 0; i < 8; i++) {
		value <<= 1;
		I2C_SCL_1();
		Delay_us(5);
		if (HAL_GPIO_ReadPin(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN))
			value++;
		I2C_SCL_0();
		Delay_us(5);
	}
	return value;
}

void Touch_I2C_IRQEnable(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin   = GTP_INT_GPIO_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void Touch_I2C_IRQDisable(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin   = GTP_INT_GPIO_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
    
    HAL_NVIC_SetPriority(EXTI9_5_IRQn , 1, 1);
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
}

void Touch_I2C_ResetChip(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin   = GTP_INT_GPIO_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull  = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);
    
    HAL_GPIO_WritePin(GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GTP_RST_GPIO_PORT,GTP_RST_GPIO_PIN, GPIO_PIN_SET);
    HAL_Delay(1);

    GPIO_InitStructure.Pin   = GTP_INT_GPIO_PIN;
    GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull  = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GTP_INT_GPIO_PORT, &GPIO_InitStructure);	
}

void Touch_I2C_Init(void)
{
    Touch_I2C_GPIO_Config();
    Touch_I2C_ResetChip();
    Touch_I2C_IRQEnable();
}

#define I2C_DIR_WR	0		/* 写控制bit */
#define I2C_DIR_RD	1		/* 读控制bit */

uint32_t I2C_ReadBytes(uint8_t ClientAddr, uint8_t *pBuffer, uint16_t NumByteToRead)
{
	/* 第1步：发起I2C总线启动信号 */
	I2C_Start();
	/* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
	I2C_SendByte(ClientAddr | I2C_DIR_RD);	/* 此处是读指令 */
	/* 第3步：等待ACK */
	if (I2C_WaitAck() != 0)
		goto cmd_fail;	/* 器件无应答 */
	while(NumByteToRead) {
        if(NumByteToRead == 1) {
        	I2C_NAck();	/* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
            /* 发送I2C总线停止信号 */
            I2C_Stop();
        }
        *pBuffer = I2C_ReadByte();
        /* 读指针自增 */
        pBuffer++; 
        /*计数器自减 */
        NumByteToRead--;
        I2C_Ack();	/* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */  
    }
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 0;	/* 执行成功 */

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 1;
}

uint32_t I2C_WriteBytes(uint8_t ClientAddr, uint8_t *pBuffer,  uint8_t NumByteToWrite)
{
	uint32_t i = 0;	
    /*　第0步：发停止信号，启动内部写操作　*/
    I2C_Stop();
    /* 通过检查器件应答的方式，判断内部写操作是否完成, 一般小于 10ms 			
      CLK频率为200KHz时，查询次数为30次左右
    */
    for (i = 0; i < 1000; i++) {				
        /* 第1步：发起I2C总线启动信号 */
        I2C_Start();
        /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
        I2C_SendByte(ClientAddr | I2C_DIR_WR);	/* 此处是写指令 */
        /* 第3步：发送一个时钟，判断器件是否正确应答 */
        if (I2C_WaitAck() == 0)
            break;
    }
    if (i  == 1000)
        goto cmd_fail;	/* EEPROM器件写超时 */

    while(NumByteToWrite--) {
        /* 第4步：开始写入数据 */
        I2C_SendByte(*pBuffer);
        /* 第5步：检查ACK */
        if (I2C_WaitAck() != 0)
            goto cmd_fail;	/* 器件无应答 */
        pBuffer++;	/* 地址增1 */		
    }
	/* 命令执行成功，发送I2C总线停止信号 */
	I2C_Stop();
	return 0;

cmd_fail: /* 命令执行失败后，切记发送停止信号，避免影响I2C总线上其他设备 */
	/* 发送I2C总线停止信号 */
	I2C_Stop();
	return 1;
}
