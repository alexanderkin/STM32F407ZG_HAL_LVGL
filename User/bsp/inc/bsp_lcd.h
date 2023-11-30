#ifndef __BSP_LCD_H__
#define __BSP_LCD_H__

#include "main.h"
#include "fonts.h"

#define WHITE		 		   0xFFFF
#define BLACK                  0x0000
#define GREY                   0xF7DE
#define BLUE                   0x001F
#define BLUE2                  0x051F
#define RED                    0xF800
#define MAGENTA                0xF81F
#define GREEN                  0x07E0
#define CYAN                   0x7FFF
#define YELLOW                 0xFFE0
#define BRED                   0xF81F
#define GRED                   0xFFE0
#define GBLUE                  0x07FF

#define FSMC_Addr_NT35510_CMD  ((uint32_t)0x68000000)
#define FSMC_Addr_NT35510_DATA ((uint32_t)0x68000002)

#define NT35510_LESS_PIXEL     480
#define NT35510_MORE_PIXEL     800

#define CMD_SetCoordinateX     0x2A00
#define CMD_SetCoordinateY     0x2B00
#define CMD_SetPixel           0x2C00

#define NT35510_DispWindow_X_Star 0
#define NT35510_DispWindow_Y_Star 0

extern uint16_t LCD_X_LENGTH;
extern uint16_t LCD_Y_LENGTH;

void bsp_lcd_init(void);
void NT35510_BackLed_Control(FunctionalState enumState);
void NT35510_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight, uint16_t usColor);
void NT35510_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight);
void NT35510_GramScan(uint8_t ucOption);
void NT35510_Write_Cmd(uint16_t usCmd);
void NT35510_Write_Data(uint16_t usData);
void NT35510_Rst(void);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor);
void LCD_SetTextColor(uint16_t Color);
void NT35510_FillColor(uint32_t ulAmout_Point, uint16_t usColor);
void NT35510_SetPointPixel(uint16_t usX, uint16_t usY);
void NT35510_SetPixel(uint16_t usX, uint16_t usY, uint16_t usColor);
void NT35510_ClearLine(uint16_t Line);
void NT35510_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar);
void NT35510_DispStringLine_EN(uint16_t line, char * pStr);
void NT35510_DispString_EN(uint16_t usX, uint16_t usY, char * pStr);
void NT35510_DrawLine( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2);
void NT35510_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled);
void NT35510_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled);

#endif // !__BSP_LCD_H__
