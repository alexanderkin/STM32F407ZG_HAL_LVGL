#include "bsp_lcd.h"

uint8_t LCD_SCAN_MODE = 3;
uint16_t LCD_X_LENGTH = NT35510_MORE_PIXEL;
uint16_t LCD_Y_LENGTH = NT35510_LESS_PIXEL;
static uint16_t CurrentTextColor = WHITE;
static uint16_t CurrentBackColor = BLACK;
static sFONT *LCD_Currentfonts = &Font16x32;

SRAM_HandleTypeDef  SRAM_Handler;
FMC_NORSRAM_TimingTypeDef Timing;

static void NT35510_Delay ( __IO uint32_t nCount )
{
    for ( ; nCount != 0; nCount -- );
}

static void NT35510_GPIO_Config(void)
{
    GPIO_InitTypeDef  GPIO_Initure;

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_FSMC_CLK_ENABLE();

    GPIO_Initure.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull      = GPIO_PULLUP;
    GPIO_Initure.Speed     = GPIO_SPEED_HIGH;

    GPIO_Initure.Pin       = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);

    GPIO_Initure.Pin       = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);

    GPIO_Initure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Initure.Alternate = GPIO_AF12_FSMC;

    GPIO_Initure.Pin       = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_4  | GPIO_PIN_5 | GPIO_PIN_8 | \
                             GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    GPIO_Initure.Pin       = GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
                             GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);

    GPIO_Initure.Pin       = GPIO_PIN_0 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

static void NT35510_FSMC_Config(void)
{
    SRAM_Handler.Instance = FMC_NORSRAM_DEVICE;
    SRAM_Handler.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

    __HAL_RCC_FSMC_CLK_ENABLE();

    Timing.AddressSetupTime              = 0x04;
    Timing.AddressHoldTime               = 0x00;
    Timing.DataSetupTime                 = 0x04;
    Timing.BusTurnAroundDuration         = 0x00;
    Timing.CLKDivision                   = 0x00;
    Timing.DataLatency                   = 0x00;
    Timing.AccessMode                    = FSMC_ACCESS_MODE_B;

    SRAM_Handler.Init.NSBank             = FSMC_NORSRAM_BANK3;
    SRAM_Handler.Init.DataAddressMux     = FSMC_DATA_ADDRESS_MUX_DISABLE;
    SRAM_Handler.Init.MemoryType         = FSMC_MEMORY_TYPE_NOR;
    SRAM_Handler.Init.MemoryDataWidth    = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    SRAM_Handler.Init.BurstAccessMode    = FSMC_BURST_ACCESS_MODE_DISABLE;
    SRAM_Handler.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    SRAM_Handler.Init.WaitSignalActive   = FSMC_WAIT_TIMING_BEFORE_WS;
    SRAM_Handler.Init.WriteOperation     = FSMC_WRITE_OPERATION_ENABLE;
    SRAM_Handler.Init.WaitSignal         = FSMC_WAIT_SIGNAL_DISABLE;
    SRAM_Handler.Init.ExtendedMode       = FSMC_EXTENDED_MODE_DISABLE;
    SRAM_Handler.Init.AsynchronousWait   = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    SRAM_Handler.Init.WriteBurst         = FSMC_WRITE_BURST_DISABLE;
    SRAM_Handler.Init.ContinuousClock    = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;

    HAL_SRAM_Init(&SRAM_Handler, &Timing, &Timing);
}

void NT35510_Write_Cmd(uint16_t usCmd)
{
    *(__IO uint16_t *)(FSMC_Addr_NT35510_CMD) = usCmd;
}

void NT35510_Write_Data(uint16_t usData)
{
	*(__IO uint16_t *)(FSMC_Addr_NT35510_DATA) = usData;
}

void NT35510_Rst(void)
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_RESET);
    NT35510_Delay(0xAFF);
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_SET);
    NT35510_Delay(0xAFF);
}

static void NT35510_REG_Config ( void )
{	
    //NT35510-HSD43
    //PAGE1
    NT35510_Write_Cmd(0xF000);    NT35510_Write_Data(0x0055);
    NT35510_Write_Cmd(0xF001);    NT35510_Write_Data(0x00AA);
    NT35510_Write_Cmd(0xF002);    NT35510_Write_Data(0x0052);
    NT35510_Write_Cmd(0xF003);    NT35510_Write_Data(0x0008);
    NT35510_Write_Cmd(0xF004);    NT35510_Write_Data(0x0001);

    //Set AVDD 5.2V
    NT35510_Write_Cmd(0xB000);    NT35510_Write_Data(0x000D);
    NT35510_Write_Cmd(0xB001);    NT35510_Write_Data(0x000D);
    NT35510_Write_Cmd(0xB002);    NT35510_Write_Data(0x000D);

    //Set AVEE 5.2V
    NT35510_Write_Cmd(0xB100);    NT35510_Write_Data(0x000D);
    NT35510_Write_Cmd(0xB101);    NT35510_Write_Data(0x000D);
    NT35510_Write_Cmd(0xB102);    NT35510_Write_Data(0x000D);

    //Set VCL -2.5V
    NT35510_Write_Cmd(0xB200);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xB201);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xB202);    NT35510_Write_Data(0x0000);

    //Set AVDD Ratio
    NT35510_Write_Cmd(0xB600);    NT35510_Write_Data(0x0044);
    NT35510_Write_Cmd(0xB601);    NT35510_Write_Data(0x0044);
    NT35510_Write_Cmd(0xB602);    NT35510_Write_Data(0x0044);

    //Set AVEE Ratio
    NT35510_Write_Cmd(0xB700);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xB701);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xB702);    NT35510_Write_Data(0x0034);

    //Set VCL -2.5V
    NT35510_Write_Cmd(0xB800);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xB801);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xB802);    NT35510_Write_Data(0x0034);

    //Control VGH booster voltage rang
    NT35510_Write_Cmd(0xBF00);    NT35510_Write_Data(0x0001);

    //VGH=15V(1V/step)	Free pump
    NT35510_Write_Cmd(0xB300);    NT35510_Write_Data(0x000f);
    NT35510_Write_Cmd(0xB301);    NT35510_Write_Data(0x000f);
    NT35510_Write_Cmd(0xB302);    NT35510_Write_Data(0x000f);

    //VGH Ratio
    NT35510_Write_Cmd(0xB900);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xB901);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xB902);    NT35510_Write_Data(0x0034);

    //VGL_REG=-10(1V/step)
    NT35510_Write_Cmd(0xB500);    NT35510_Write_Data(0x0008);
    NT35510_Write_Cmd(0xB501);    NT35510_Write_Data(0x0008);
    NT35510_Write_Cmd(0xB502);    NT35510_Write_Data(0x0008);

    NT35510_Write_Cmd(0xC200);    NT35510_Write_Data(0x0003);

    //VGLX Ratio
    NT35510_Write_Cmd(0xBA00);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xBA01);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xBA02);    NT35510_Write_Data(0x0034);

    //VGMP/VGSP=4.5V/0V
    NT35510_Write_Cmd(0xBC00);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xBC01);    NT35510_Write_Data(0x0078);
    NT35510_Write_Cmd(0xBC02);    NT35510_Write_Data(0x0000);

    //VGMN/VGSN=-4.5V/0V
    NT35510_Write_Cmd(0xBD00);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xBD01);    NT35510_Write_Data(0x0078);
    NT35510_Write_Cmd(0xBD02);    NT35510_Write_Data(0x0000);

    //Vcom=-1.4V(12.5mV/step)
    NT35510_Write_Cmd(0xBE00);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xBE01);    NT35510_Write_Data(0x0064);

    //Gamma (R+)
    NT35510_Write_Cmd(0xD100);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD101);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD102);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD103);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD104);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD105);    NT35510_Write_Data(0x003A);
    NT35510_Write_Cmd(0xD106);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD107);    NT35510_Write_Data(0x004A);
    NT35510_Write_Cmd(0xD108);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD109);    NT35510_Write_Data(0x005C);
    NT35510_Write_Cmd(0xD10A);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD10B);    NT35510_Write_Data(0x0081);
    NT35510_Write_Cmd(0xD10C);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD10D);    NT35510_Write_Data(0x00A6);
    NT35510_Write_Cmd(0xD10E);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD10F);    NT35510_Write_Data(0x00E5);
    NT35510_Write_Cmd(0xD110);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD111);    NT35510_Write_Data(0x0013);
    NT35510_Write_Cmd(0xD112);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD113);    NT35510_Write_Data(0x0054);
    NT35510_Write_Cmd(0xD114);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD115);    NT35510_Write_Data(0x0082);
    NT35510_Write_Cmd(0xD116);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD117);    NT35510_Write_Data(0x00CA);
    NT35510_Write_Cmd(0xD118);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD119);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD11A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD11B);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD11C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD11D);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD11E);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD11F);    NT35510_Write_Data(0x0067);
    NT35510_Write_Cmd(0xD120);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD121);    NT35510_Write_Data(0x0084);
    NT35510_Write_Cmd(0xD122);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD123);    NT35510_Write_Data(0x00A4);
    NT35510_Write_Cmd(0xD124);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD125);    NT35510_Write_Data(0x00B7);
    NT35510_Write_Cmd(0xD126);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD127);    NT35510_Write_Data(0x00CF);
    NT35510_Write_Cmd(0xD128);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD129);    NT35510_Write_Data(0x00DE);
    NT35510_Write_Cmd(0xD12A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD12B);    NT35510_Write_Data(0x00F2);
    NT35510_Write_Cmd(0xD12C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD12D);    NT35510_Write_Data(0x00FE);
    NT35510_Write_Cmd(0xD12E);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD12F);    NT35510_Write_Data(0x0010);
    NT35510_Write_Cmd(0xD130);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD131);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD132);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD133);    NT35510_Write_Data(0x006D);

    //Gamma (G+)
    NT35510_Write_Cmd(0xD200);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD201);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD202);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD203);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD204);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD205);    NT35510_Write_Data(0x003A);
    NT35510_Write_Cmd(0xD206);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD207);    NT35510_Write_Data(0x004A);
    NT35510_Write_Cmd(0xD208);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD209);    NT35510_Write_Data(0x005C);
    NT35510_Write_Cmd(0xD20A);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD20B);    NT35510_Write_Data(0x0081);
    NT35510_Write_Cmd(0xD20C);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD20D);    NT35510_Write_Data(0x00A6);
    NT35510_Write_Cmd(0xD20E);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD20F);    NT35510_Write_Data(0x00E5);
    NT35510_Write_Cmd(0xD210);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD211);    NT35510_Write_Data(0x0013);
    NT35510_Write_Cmd(0xD212);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD213);    NT35510_Write_Data(0x0054);
    NT35510_Write_Cmd(0xD214);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD215);    NT35510_Write_Data(0x0082);
    NT35510_Write_Cmd(0xD216);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD217);    NT35510_Write_Data(0x00CA);
    NT35510_Write_Cmd(0xD218);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD219);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD21A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD21B);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD21C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD21D);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD21E);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD21F);    NT35510_Write_Data(0x0067);
    NT35510_Write_Cmd(0xD220);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD221);    NT35510_Write_Data(0x0084);
    NT35510_Write_Cmd(0xD222);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD223);    NT35510_Write_Data(0x00A4);
    NT35510_Write_Cmd(0xD224);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD225);    NT35510_Write_Data(0x00B7);
    NT35510_Write_Cmd(0xD226);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD227);    NT35510_Write_Data(0x00CF);
    NT35510_Write_Cmd(0xD228);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD229);    NT35510_Write_Data(0x00DE);
    NT35510_Write_Cmd(0xD22A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD22B);    NT35510_Write_Data(0x00F2);
    NT35510_Write_Cmd(0xD22C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD22D);    NT35510_Write_Data(0x00FE);
    NT35510_Write_Cmd(0xD22E);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD22F);    NT35510_Write_Data(0x0010);
    NT35510_Write_Cmd(0xD230);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD231);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD232);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD233);    NT35510_Write_Data(0x006D);

    //Gamma (B+)
    NT35510_Write_Cmd(0xD300);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD301);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD302);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD303);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD304);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD305);    NT35510_Write_Data(0x003A);
    NT35510_Write_Cmd(0xD306);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD307);    NT35510_Write_Data(0x004A);
    NT35510_Write_Cmd(0xD308);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD309);    NT35510_Write_Data(0x005C);
    NT35510_Write_Cmd(0xD30A);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD30B);    NT35510_Write_Data(0x0081);
    NT35510_Write_Cmd(0xD30C);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD30D);    NT35510_Write_Data(0x00A6);
    NT35510_Write_Cmd(0xD30E);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD30F);    NT35510_Write_Data(0x00E5);
    NT35510_Write_Cmd(0xD310);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD311);    NT35510_Write_Data(0x0013);
    NT35510_Write_Cmd(0xD312);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD313);    NT35510_Write_Data(0x0054);
    NT35510_Write_Cmd(0xD314);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD315);    NT35510_Write_Data(0x0082);
    NT35510_Write_Cmd(0xD316);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD317);    NT35510_Write_Data(0x00CA);
    NT35510_Write_Cmd(0xD318);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD319);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD31A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD31B);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD31C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD31D);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD31E);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD31F);    NT35510_Write_Data(0x0067);
    NT35510_Write_Cmd(0xD320);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD321);    NT35510_Write_Data(0x0084);
    NT35510_Write_Cmd(0xD322);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD323);    NT35510_Write_Data(0x00A4);
    NT35510_Write_Cmd(0xD324);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD325);    NT35510_Write_Data(0x00B7);
    NT35510_Write_Cmd(0xD326);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD327);    NT35510_Write_Data(0x00CF);
    NT35510_Write_Cmd(0xD328);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD329);    NT35510_Write_Data(0x00DE);
    NT35510_Write_Cmd(0xD32A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD32B);    NT35510_Write_Data(0x00F2);
    NT35510_Write_Cmd(0xD32C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD32D);    NT35510_Write_Data(0x00FE);
    NT35510_Write_Cmd(0xD32E);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD32F);    NT35510_Write_Data(0x0010);
    NT35510_Write_Cmd(0xD330);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD331);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD332);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD333);    NT35510_Write_Data(0x006D);

    //Gamma (R-)
    NT35510_Write_Cmd(0xD400);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD401);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD402);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD403);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD404);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD405);    NT35510_Write_Data(0x003A);
    NT35510_Write_Cmd(0xD406);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD407);    NT35510_Write_Data(0x004A);
    NT35510_Write_Cmd(0xD408);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD409);    NT35510_Write_Data(0x005C);
    NT35510_Write_Cmd(0xD40A);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD40B);    NT35510_Write_Data(0x0081);
    NT35510_Write_Cmd(0xD40C);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD40D);    NT35510_Write_Data(0x00A6);
    NT35510_Write_Cmd(0xD40E);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD40F);    NT35510_Write_Data(0x00E5);
    NT35510_Write_Cmd(0xD410);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD411);    NT35510_Write_Data(0x0013);
    NT35510_Write_Cmd(0xD412);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD413);    NT35510_Write_Data(0x0054);
    NT35510_Write_Cmd(0xD414);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD415);    NT35510_Write_Data(0x0082);
    NT35510_Write_Cmd(0xD416);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD417);    NT35510_Write_Data(0x00CA);
    NT35510_Write_Cmd(0xD418);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD419);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD41A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD41B);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD41C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD41D);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD41E);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD41F);    NT35510_Write_Data(0x0067);
    NT35510_Write_Cmd(0xD420);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD421);    NT35510_Write_Data(0x0084);
    NT35510_Write_Cmd(0xD422);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD423);    NT35510_Write_Data(0x00A4);
    NT35510_Write_Cmd(0xD424);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD425);    NT35510_Write_Data(0x00B7);
    NT35510_Write_Cmd(0xD426);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD427);    NT35510_Write_Data(0x00CF);
    NT35510_Write_Cmd(0xD428);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD429);    NT35510_Write_Data(0x00DE);
    NT35510_Write_Cmd(0xD42A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD42B);    NT35510_Write_Data(0x00F2);
    NT35510_Write_Cmd(0xD42C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD42D);    NT35510_Write_Data(0x00FE);
    NT35510_Write_Cmd(0xD42E);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD42F);    NT35510_Write_Data(0x0010);
    NT35510_Write_Cmd(0xD430);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD431);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD432);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD433);    NT35510_Write_Data(0x006D);

    //Gamma (G-)
    NT35510_Write_Cmd(0xD500);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD501);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD502);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD503);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD504);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD505);    NT35510_Write_Data(0x003A);
    NT35510_Write_Cmd(0xD506);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD507);    NT35510_Write_Data(0x004A);
    NT35510_Write_Cmd(0xD508);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD509);    NT35510_Write_Data(0x005C);
    NT35510_Write_Cmd(0xD50A);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD50B);    NT35510_Write_Data(0x0081);
    NT35510_Write_Cmd(0xD50C);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD50D);    NT35510_Write_Data(0x00A6);
    NT35510_Write_Cmd(0xD50E);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD50F);    NT35510_Write_Data(0x00E5);
    NT35510_Write_Cmd(0xD510);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD511);    NT35510_Write_Data(0x0013);
    NT35510_Write_Cmd(0xD512);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD513);    NT35510_Write_Data(0x0054);
    NT35510_Write_Cmd(0xD514);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD515);    NT35510_Write_Data(0x0082);
    NT35510_Write_Cmd(0xD516);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD517);    NT35510_Write_Data(0x00CA);
    NT35510_Write_Cmd(0xD518);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD519);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD51A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD51B);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD51C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD51D);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD51E);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD51F);    NT35510_Write_Data(0x0067);
    NT35510_Write_Cmd(0xD520);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD521);    NT35510_Write_Data(0x0084);
    NT35510_Write_Cmd(0xD522);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD523);    NT35510_Write_Data(0x00A4);
    NT35510_Write_Cmd(0xD524);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD525);    NT35510_Write_Data(0x00B7);
    NT35510_Write_Cmd(0xD526);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD527);    NT35510_Write_Data(0x00CF);
    NT35510_Write_Cmd(0xD528);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD529);    NT35510_Write_Data(0x00DE);
    NT35510_Write_Cmd(0xD52A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD52B);    NT35510_Write_Data(0x00F2);
    NT35510_Write_Cmd(0xD52C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD52D);    NT35510_Write_Data(0x00FE);
    NT35510_Write_Cmd(0xD52E);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD52F);    NT35510_Write_Data(0x0010);
    NT35510_Write_Cmd(0xD530);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD531);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD532);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD533);    NT35510_Write_Data(0x006D);

    //Gamma (B-)
    NT35510_Write_Cmd(0xD600);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD601);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD602);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD603);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD604);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD605);    NT35510_Write_Data(0x003A);
    NT35510_Write_Cmd(0xD606);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD607);    NT35510_Write_Data(0x004A);
    NT35510_Write_Cmd(0xD608);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD609);    NT35510_Write_Data(0x005C);
    NT35510_Write_Cmd(0xD60A);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD60B);    NT35510_Write_Data(0x0081);
    NT35510_Write_Cmd(0xD60C);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD60D);    NT35510_Write_Data(0x00A6);
    NT35510_Write_Cmd(0xD60E);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD60F);    NT35510_Write_Data(0x00E5);
    NT35510_Write_Cmd(0xD610);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD611);    NT35510_Write_Data(0x0013);
    NT35510_Write_Cmd(0xD612);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD613);    NT35510_Write_Data(0x0054);
    NT35510_Write_Cmd(0xD614);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD615);    NT35510_Write_Data(0x0082);
    NT35510_Write_Cmd(0xD616);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD617);    NT35510_Write_Data(0x00CA);
    NT35510_Write_Cmd(0xD618);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD619);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xD61A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD61B);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xD61C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD61D);    NT35510_Write_Data(0x0034);
    NT35510_Write_Cmd(0xD61E);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD61F);    NT35510_Write_Data(0x0067);
    NT35510_Write_Cmd(0xD620);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD621);    NT35510_Write_Data(0x0084);
    NT35510_Write_Cmd(0xD622);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD623);    NT35510_Write_Data(0x00A4);
    NT35510_Write_Cmd(0xD624);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD625);    NT35510_Write_Data(0x00B7);
    NT35510_Write_Cmd(0xD626);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD627);    NT35510_Write_Data(0x00CF);
    NT35510_Write_Cmd(0xD628);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD629);    NT35510_Write_Data(0x00DE);
    NT35510_Write_Cmd(0xD62A);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD62B);    NT35510_Write_Data(0x00F2);
    NT35510_Write_Cmd(0xD62C);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xD62D);    NT35510_Write_Data(0x00FE);
    NT35510_Write_Cmd(0xD62E);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD62F);    NT35510_Write_Data(0x0010);
    NT35510_Write_Cmd(0xD630);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD631);    NT35510_Write_Data(0x0033);
    NT35510_Write_Cmd(0xD632);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xD633);    NT35510_Write_Data(0x006D);

    //PAGE0
    NT35510_Write_Cmd(0xF000);    NT35510_Write_Data(0x0055);
    NT35510_Write_Cmd(0xF001);    NT35510_Write_Data(0x00AA);
    NT35510_Write_Cmd(0xF002);    NT35510_Write_Data(0x0052);
    NT35510_Write_Cmd(0xF003);    NT35510_Write_Data(0x0008);
    NT35510_Write_Cmd(0xF004);    NT35510_Write_Data(0x0000);

    //480x800
    NT35510_Write_Cmd(0xB500);    NT35510_Write_Data(0x0050);

    //Dispay control
    NT35510_Write_Cmd(0xB100);    NT35510_Write_Data(0x00CC);
    NT35510_Write_Cmd(0xB101);    NT35510_Write_Data(0x0000);

    //Source hold time (Nova non-used)
    NT35510_Write_Cmd(0xB600);    NT35510_Write_Data(0x0005);

    //Gate EQ control	 (Nova non-used)
    NT35510_Write_Cmd(0xB700);    NT35510_Write_Data(0x0077);
    NT35510_Write_Cmd(0xB701);    NT35510_Write_Data(0x0077);

    //Source EQ control (Nova non-used)
    NT35510_Write_Cmd(0xB800);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xB801);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xB802);    NT35510_Write_Data(0x0003);
    NT35510_Write_Cmd(0xB803);    NT35510_Write_Data(0x0003);

    //Inversion mode: column
    NT35510_Write_Cmd(0xBC00);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xBC01);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0xBC02);    NT35510_Write_Data(0x0000);

    //Frame rate	(Nova non-used)
    NT35510_Write_Cmd(0xBD00);    NT35510_Write_Data(0x0001);
    NT35510_Write_Cmd(0xBD01);    NT35510_Write_Data(0x0084);
    NT35510_Write_Cmd(0xBD02);    NT35510_Write_Data(0x001c);
    NT35510_Write_Cmd(0xBD03);    NT35510_Write_Data(0x001c);
    NT35510_Write_Cmd(0xBD04);    NT35510_Write_Data(0x0000);

    //LGD timing control(4H/4-delay_ms)
    NT35510_Write_Cmd(0xC900);    NT35510_Write_Data(0x00D0);
    NT35510_Write_Cmd(0xC901);    NT35510_Write_Data(0x0002);
    NT35510_Write_Cmd(0xC902);    NT35510_Write_Data(0x0050);
    NT35510_Write_Cmd(0xC903);    NT35510_Write_Data(0x0050);
    NT35510_Write_Cmd(0xC904);    NT35510_Write_Data(0x0050);

    NT35510_Write_Cmd(0x3600);    NT35510_Write_Data(0x0000);
    NT35510_Write_Cmd(0x3500);    NT35510_Write_Data(0x0000);

    NT35510_Write_Cmd(0xFF00);    NT35510_Write_Data(0x00AA);
    NT35510_Write_Cmd(0xFF01);    NT35510_Write_Data(0x0055);
    NT35510_Write_Cmd(0xFF02);    NT35510_Write_Data(0x0025);
    NT35510_Write_Cmd(0xFF03);    NT35510_Write_Data(0x0001);

    NT35510_Write_Cmd(0xFC00);    NT35510_Write_Data(0x0016);
    NT35510_Write_Cmd(0xFC01);    NT35510_Write_Data(0x00A2);
    NT35510_Write_Cmd(0xFC02);    NT35510_Write_Data(0x0026);
    NT35510_Write_Cmd(0x3A00);    NT35510_Write_Data(0x0006);

    NT35510_Write_Cmd(0x3A00);    NT35510_Write_Data(0x0055);
    //Sleep out
    NT35510_Write_Cmd(0x1100);
    NT35510_Delay(0xFFFFFF);

    //Display on
    NT35510_Write_Cmd(0x2900);
}

void NT35510_GramScan(uint8_t ucOption)
{
    if(ucOption > 7)
        return;

    LCD_SCAN_MODE = ucOption;

    if(ucOption % 2 == 0)	
    {
        LCD_X_LENGTH = NT35510_LESS_PIXEL;
        LCD_Y_LENGTH = NT35510_MORE_PIXEL;
    }
    else				
    {
        LCD_X_LENGTH = NT35510_MORE_PIXEL;
        LCD_Y_LENGTH = NT35510_LESS_PIXEL; 
    }

    NT35510_Write_Cmd(0x3600);
    NT35510_Write_Data(0x00 | (ucOption << 5));
    NT35510_Write_Cmd(CMD_SetCoordinateX);
    NT35510_Write_Data(0x00);
    NT35510_Write_Cmd(CMD_SetCoordinateX + 1);
    NT35510_Write_Data(0x00);
    NT35510_Write_Cmd(CMD_SetCoordinateX + 2);
    NT35510_Write_Data(((LCD_X_LENGTH - 1) >> 8) & 0xFF);
    NT35510_Write_Cmd(CMD_SetCoordinateX + 3);
    NT35510_Write_Data((LCD_X_LENGTH - 1) & 0xFF);

    NT35510_Write_Cmd(CMD_SetCoordinateY);
    NT35510_Write_Data(0x00);
    NT35510_Write_Cmd(CMD_SetCoordinateY + 1);
    NT35510_Write_Data(0x00);
    NT35510_Write_Cmd(CMD_SetCoordinateY + 2);
    NT35510_Write_Data(((LCD_Y_LENGTH - 1) >> 8) & 0xFF);
    NT35510_Write_Cmd(CMD_SetCoordinateY + 3);
    NT35510_Write_Data((LCD_Y_LENGTH - 1) & 0xFF);

    NT35510_Write_Cmd(CMD_SetPixel);	
}

void NT35510_OpenWindow(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight)
{	
    NT35510_Write_Cmd(CMD_SetCoordinateX);
    NT35510_Write_Data(usX >> 8);
    NT35510_Write_Cmd(CMD_SetCoordinateX + 1);
    NT35510_Write_Data(usX & 0xff);
    NT35510_Write_Cmd(CMD_SetCoordinateX + 2);
    NT35510_Write_Data((usX + usWidth - 1) >> 8);
    NT35510_Write_Cmd(CMD_SetCoordinateX + 3);
    NT35510_Write_Data((usX + usWidth - 1) & 0xff);

    NT35510_Write_Cmd(CMD_SetCoordinateY);
    NT35510_Write_Data(usY >> 8);
    NT35510_Write_Cmd(CMD_SetCoordinateY + 1);
    NT35510_Write_Data(usY & 0xff);
    NT35510_Write_Cmd(CMD_SetCoordinateY + 2);
    NT35510_Write_Data((usY + usHeight - 1 ) >> 8);
    NT35510_Write_Cmd(CMD_SetCoordinateY + 3);
    NT35510_Write_Data((usY + usHeight - 1) & 0xff);
}

void NT35510_FillColor(uint32_t ulAmout_Point, uint16_t usColor)
{
	uint32_t i = 0;
	NT35510_Write_Cmd(CMD_SetPixel);
	for (i = 0; i < ulAmout_Point; i ++)
	NT35510_Write_Data(usColor);
}

void NT35510_Clear(uint16_t usX, uint16_t usY, uint16_t usWidth, uint16_t usHeight, uint16_t usColor)
{
    NT35510_OpenWindow(usX, usY, usWidth, usHeight);
    NT35510_FillColor(usWidth * usHeight, usColor);
}

void NT35510_BackLed_Control(FunctionalState enumState)
{
    if(enumState)
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
}

void bsp_lcd_init(void)
{
	NT35510_GPIO_Config();
	NT35510_FSMC_Config();

	NT35510_Rst();
	NT35510_REG_Config();

	//设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向  
	NT35510_GramScan(LCD_SCAN_MODE);

	NT35510_Clear(0, 0, LCD_X_LENGTH, LCD_Y_LENGTH, CurrentBackColor);
	NT35510_BackLed_Control(ENABLE);
}

void LCD_SetFont(sFONT *fonts)
{
    LCD_Currentfonts = fonts;
}

sFONT *LCD_GetFont(void)
{
    return LCD_Currentfonts;
}

void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) 
{
    CurrentTextColor = TextColor; 
    CurrentBackColor = BackColor;
}

void LCD_SetTextColor(uint16_t Color)
{
    CurrentTextColor = Color;
}

void NT35510_ClearLine(uint16_t Line)
{
    NT35510_Clear(0, Line, LCD_X_LENGTH, ((sFONT *)LCD_GetFont())->Height, CurrentBackColor);
}

void NT35510_DispChar_EN(uint16_t usX, uint16_t usY, const char cChar)
{
    uint8_t  byteCount, bitCount, fontLength;	
    uint16_t ucRelativePositon;
    uint8_t* Pfont;

    ucRelativePositon = cChar - ' ';
    fontLength = (LCD_Currentfonts->Width * LCD_Currentfonts->Height) / 8;
    Pfont = (uint8_t *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];

    NT35510_OpenWindow(usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);
    NT35510_Write_Cmd(CMD_SetPixel);		

    for (byteCount = 0; byteCount < fontLength; byteCount++)
    {
        for (bitCount = 0; bitCount < 8; bitCount++)
        {
            if (Pfont[byteCount] & (0x80 >> bitCount))
                NT35510_Write_Data (CurrentTextColor);			
            else
                NT35510_Write_Data (CurrentBackColor);
        }	
    }	
}

void NT35510_DispStringLine_EN(uint16_t line, char * pStr)
{
    uint16_t usX = 0;
    while (* pStr != '\0')
    {
        if ((usX - NT35510_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH)
        {
            usX = NT35510_DispWindow_X_Star;
            line += LCD_Currentfonts->Height;
        }

        if ((line - NT35510_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH)
        {
            usX = NT35510_DispWindow_X_Star;
            line = NT35510_DispWindow_Y_Star;
        }

        NT35510_DispChar_EN(usX, line, * pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;
    }
}

void NT35510_DispString_EN(uint16_t usX, uint16_t usY, char * pStr)
{
    while (* pStr != '\0')
    {
        if ((usX - NT35510_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH)
        {
            usX = NT35510_DispWindow_X_Star;
            usY += LCD_Currentfonts->Height;
        }

        if ((usY - NT35510_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH)
        {
            usX = NT35510_DispWindow_X_Star;
            usY = NT35510_DispWindow_Y_Star;
        }

        NT35510_DispChar_EN(usX, usY, * pStr);
        pStr ++;
        usX += LCD_Currentfonts->Width;
    }
}

static void NT35510_SetCursor(uint16_t usX, uint16_t usY)	
{
	NT35510_OpenWindow(usX, usY, 1, 1);
}

void NT35510_SetPointPixel(uint16_t usX, uint16_t usY)
{
    if ((usX < LCD_X_LENGTH) && (usY < LCD_Y_LENGTH))
    {
        NT35510_SetCursor(usX, usY);
        NT35510_FillColor(1, CurrentTextColor);
    }
}

void NT35510_SetPixel(uint16_t usX, uint16_t usY, uint16_t usColor)
{
    NT35510_SetCursor(usX, usY);
	NT35510_Write_Cmd(CMD_SetPixel);
	NT35510_Write_Data(usColor);
}

void NT35510_DrawLine( uint16_t usX1, uint16_t usY1, uint16_t usX2, uint16_t usY2)
{
	uint16_t us;
	uint16_t usX_Current, usY_Current;
	int32_t lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance;
	int32_t lIncrease_X, lIncrease_Y;

	lDelta_X = usX2 - usX1;
	lDelta_Y = usY2 - usY1;
	usX_Current = usX1;
	usY_Current = usY1;

	if (lDelta_X > 0)
		lIncrease_X = 1;
	else if (lDelta_X == 0)
		lIncrease_X = 0;
	else
    {
        lIncrease_X = -1;
        lDelta_X    = -lDelta_X;
    }

	if (lDelta_Y > 0)
		lIncrease_Y = 1;
	else if (lDelta_Y == 0)
		lIncrease_Y = 0;
	else
    {
        lIncrease_Y = -1;
        lDelta_Y = - lDelta_Y;
    }

	if (lDelta_X > lDelta_Y)
		lDistance = lDelta_X;
	else 
		lDistance = lDelta_Y; 

	for (us = 0; us <= lDistance + 1; us ++)
	{
		NT35510_SetPointPixel(usX_Current, usY_Current);
		lError_X += lDelta_X;
		lError_Y += lDelta_Y;
		if (lError_X > lDistance)
		{
			lError_X    -= lDistance;
			usX_Current += lIncrease_X;
		}
		if (lError_Y > lDistance)
		{
			lError_Y    -= lDistance;
			usY_Current += lIncrease_Y;
		}
	}
}

void NT35510_DrawRectangle(uint16_t usX_Start, uint16_t usY_Start, uint16_t usWidth, uint16_t usHeight, uint8_t ucFilled)
{
	if (ucFilled)
	{
		NT35510_OpenWindow(usX_Start, usY_Start, usWidth, usHeight);
		NT35510_FillColor(usWidth * usHeight , CurrentTextColor);
	}
	else
	{
		NT35510_DrawLine(usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start);
		NT35510_DrawLine(usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1);
		NT35510_DrawLine(usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1);
		NT35510_DrawLine(usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1);		
	}
}

void NT35510_DrawCircle(uint16_t usX_Center, uint16_t usY_Center, uint16_t usRadius, uint8_t ucFilled)
{
	int16_t sCurrentX, sCurrentY;
	int16_t sError;

	sCurrentX = 0; sCurrentY = usRadius;	  
	sError = 3 - (usRadius << 1);

	while (sCurrentX <= sCurrentY)
	{
		int16_t sCountY;
		if (ucFilled)
        {
            for ( sCountY = sCurrentX; sCountY <= sCurrentY; sCountY ++ ) 
			{                      
				NT35510_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCountY  );
				NT35510_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCountY  );
				NT35510_SetPointPixel(usX_Center - sCountY,   usY_Center + sCurrentX);
				NT35510_SetPointPixel(usX_Center - sCountY,   usY_Center - sCurrentX);
				NT35510_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCountY  );
                NT35510_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCountY  );
				NT35510_SetPointPixel(usX_Center + sCountY,   usY_Center - sCurrentX);
                NT35510_SetPointPixel(usX_Center + sCountY,   usY_Center + sCurrentX);
			}
        } 			
		else
		{          
			NT35510_SetPointPixel(usX_Center + sCurrentX, usY_Center + sCurrentY);
			NT35510_SetPointPixel(usX_Center - sCurrentX, usY_Center + sCurrentY);
			NT35510_SetPointPixel(usX_Center - sCurrentY, usY_Center + sCurrentX);
			NT35510_SetPointPixel(usX_Center - sCurrentY, usY_Center - sCurrentX);
			NT35510_SetPointPixel(usX_Center - sCurrentX, usY_Center - sCurrentY);
			NT35510_SetPointPixel(usX_Center + sCurrentX, usY_Center - sCurrentY);
			NT35510_SetPointPixel(usX_Center + sCurrentY, usY_Center - sCurrentX);
			NT35510_SetPointPixel(usX_Center + sCurrentY, usY_Center + sCurrentX);
        }	
        sCurrentX ++;
        if ( sError < 0 ) 
            sError += 4 * sCurrentX + 6;	  
        else
        {
            sError += 10 + 4 * ( sCurrentX - sCurrentY );
            sCurrentY --;
        }
    }
}
