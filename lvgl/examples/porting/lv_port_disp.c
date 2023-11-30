/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "../../lvgl.h"
#include "main.h"
#include "bsp_lcd.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
DMA_HandleTypeDef LCD_DMA_Handler;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

static void DMA_TransferComplete(DMA_HandleTypeDef *dma);

//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_disp_drv_t disp_drv;
static int32_t x1_flush;
static int32_t y1_flush;
static int32_t x2_flush;
static int32_t y2_fill;
static int32_t y_fill_act;
static const lv_color_t * buf_to_flush;
static __IO uint16_t * my_fb = (__IO uint16_t*) (FSMC_Addr_NT35510_DATA);


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

//    /* Example for 2) */
//    static lv_disp_draw_buf_t draw_buf_dsc_2;
//    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
//    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

//    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
//    static lv_disp_draw_buf_t draw_buf_dsc_3;
//    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
//    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*An other screen sized buffer*/
//    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2, MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_1;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /*You code here*/
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	/*Return if the area is out the screen*/
	if(area->x2 < 0) return;
	if(area->y2 < 0) return;
	if(area->x1 > MY_DISP_HOR_RES - 1) return;
	if(area->y1 > MY_DISP_VER_RES - 1) return;

	/*Truncate the area to the screen*/
	int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
	int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
	int32_t act_x2 = area->x2 > MY_DISP_HOR_RES - 1 ? MY_DISP_HOR_RES - 1 : area->x2;
	int32_t act_y2 = area->y2 > MY_DISP_VER_RES - 1 ? MY_DISP_VER_RES - 1 : area->y2;

	x1_flush = act_x1;
	y1_flush = act_y1;
	x2_flush = act_x2;
	y2_fill = act_y2;
	y_fill_act = act_y1;
	buf_to_flush = color_p;
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateX;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_x1 >> 8;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateX + 1;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_x1 & 0xff;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateX + 2;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_x2 >> 8;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateX + 3;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_x2 & 0xff;

    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateY;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_y1 >> 8;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateY + 1;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_y1 & 0xff;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateY + 2;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_y2 >> 8;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetCoordinateY + 3;
    *(volatile uint16_t *)(FSMC_Addr_NT35510_DATA) = act_y2 & 0xff;

    *(volatile uint16_t *)(FSMC_Addr_NT35510_CMD) = CMD_SetPixel;

    HAL_StatusTypeDef err = HAL_DMA_Start_IT(&LCD_DMA_Handler,(uint32_t)buf_to_flush, (uint32_t)&my_fb[y_fill_act * MY_DISP_HOR_RES + x1_flush], (x2_flush - x1_flush + 1));
	if(err != HAL_OK)
	{
		while(1);	/*Halt on error*/
	}
}

void LCD_DMA_Init(void)
{
    __HAL_RCC_DMA2_CLK_ENABLE(); 

    LCD_DMA_Handler.Instance = DMA2_Stream1;
    LCD_DMA_Handler.Init.Channel = DMA_CHANNEL_0;
    LCD_DMA_Handler.Init.Direction = DMA_MEMORY_TO_MEMORY;
    LCD_DMA_Handler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    LCD_DMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_1QUARTERFULL;
    LCD_DMA_Handler.Init.MemBurst = DMA_MBURST_SINGLE;
    LCD_DMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    LCD_DMA_Handler.Init.MemInc = DMA_MINC_DISABLE;
    LCD_DMA_Handler.Init.Mode = DMA_NORMAL;
    LCD_DMA_Handler.Init.PeriphBurst = DMA_PBURST_SINGLE;
    LCD_DMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    LCD_DMA_Handler.Init.PeriphInc = DMA_PINC_ENABLE;
    LCD_DMA_Handler.Init.Priority = DMA_PRIORITY_HIGH;

    HAL_DMA_DeInit(&LCD_DMA_Handler);
    HAL_DMA_Init(&LCD_DMA_Handler);

    HAL_DMA_RegisterCallback(&LCD_DMA_Handler, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);

    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}

void DMA2_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&LCD_DMA_Handler);
}

static void DMA_TransferComplete(DMA_HandleTypeDef *dma)
{
	y_fill_act++;

	if(y_fill_act > y2_fill) {
        lv_disp_flush_ready(&disp_drv);
	} else {
	    buf_to_flush += x2_flush - x1_flush + 1;
	    /*##-7- Start the DMA transfer using the interrupt mode ####################*/
	    /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
	    /* Enable All the DMA interrupts */

	    if(HAL_DMA_Start_IT(dma, (uint32_t)buf_to_flush, (uint32_t)&my_fb[y_fill_act * MY_DISP_HOR_RES + x1_flush], (x2_flush - x1_flush + 1)) != HAL_OK)
	    {
	        while(1);	/*Halt on error*/
	    }
	}
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
