#ifndef __GT917S_H__
#define __GT917S_H__

#include "main.h"

/* 表示读数据 */ 
#define I2C_M_RD        0x0001

#define UPDATE_CONFIG   0    // 1 ：更新配置    0 ：不更新配置

typedef enum 
{
	GT9157=0,
	GT911=1,
    GT5688=2,
}TOUCH_IC;

struct i2c_msg {
	uint8_t addr;		/* 从设备的I2C设备地址 */
	uint16_t flags;     /* 控制标志*/
	uint16_t len;		/* 读写数据的长度 */
	uint8_t *buf;		/* 存储读写数据的指针 */
};

// STEP_3(optional): Specify your special config info if needed
#define GTP_MAX_HEIGHT   480
#define GTP_MAX_WIDTH    800
#define GTP_INT_TRIGGER  0
#define GTP_MAX_TOUCH    5

//***************************PART3:OTHER define*********************************
#define GTP_DRIVER_VERSION        "V2.2<2014/01/14>"
#define GTP_I2C_NAME              "Goodix-TS"
#define GT91XX_CONFIG_PROC_FILE   "gt9xx_config"
#define GTP_POLL_TIME             10    
#define GTP_ADDR_LENGTH           2
#define GTP_CONFIG_MIN_LENGTH     186
#define GTP_CONFIG_MAX_LENGTH     256
#define FAIL                      0
#define SUCCESS                   1
#define SWITCH_OFF                0
#define SWITCH_ON                 1

//******************** For GT9XXF Start **********************//
#define GTP_REG_BAK_REF          0x99EC
#define GTP_REG_MAIN_CLK         0x8020
#define GTP_REG_CHIP_TYPE        0x8000
#define GTP_REG_HAVE_KEY         0x8057
#define GTP_REG_MATRIX_DRVNUM    0x8069     
#define GTP_REG_MATRIX_SENNUM    0x806A
#define GTP_REG_COMMAND          0x8040

#define GTP_COMMAND_READSTATUS   0
#define GTP_COMMAND_DIFFERENCE   1
#define GTP_COMMAND_SOFTRESET    2
#define GTP_COMMAND_UPDATE       3
#define GTP_COMMAND_CALCULATE    4
#define GTP_COMMAND_TURNOFF      5


#define GTP_FL_FW_BURN        0x00
#define GTP_FL_ESD_RECOVERY   0x01
#define GTP_FL_READ_REPAIR    0x02

#define GTP_BAK_REF_SEND    0
#define GTP_BAK_REF_STORE   1
#define CFG_LOC_DRVA_NUM    29
#define CFG_LOC_DRVB_NUM    30
#define CFG_LOC_SENS_NUM    31

#define GTP_CHK_FW_MAX        40
#define GTP_CHK_FS_MNT_MAX    300
#define GTP_BAK_REF_PATH      "/data/gtp_ref.bin"
#define GTP_MAIN_CLK_PATH     "/data/gtp_clk.bin"
#define GTP_RQST_CONFIG       0x01
#define GTP_RQST_BAK_REF      0x02
#define GTP_RQST_RESET        0x03
#define GTP_RQST_MAIN_CLOCK   0x04
#define GTP_RQST_RESPONDED    0x00
#define GTP_RQST_IDLE         0xFF

//******************** For GT9XXF End **********************//
// Registers define
#define GTP_READ_COOR_ADDR    0x814E
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814A
#define GTP_REG_CONFIG_DATA   0x8050
#define GTP_REG_VERSION       0x8140
#define RESOLUTION_LOC        3
#define TRIGGER_LOC           8
#define X2Y_LOC               (1<<3)

#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))
	

//***************************PART1:ON/OFF define*******************************

#define GTP_DEBUG_ON         0
#define GTP_DEBUG_ARRAY_ON   0
#define GTP_DEBUG_FUNC_ON    0
// Log define
#define GTP_INFO(fmt,arg...)    printf("<<-GTP-INFO->> "fmt"\n",##arg)
#define GTP_ERROR(fmt,arg...)   printf("<<-GTP-ERROR->> "fmt"\n",##arg)
#define GTP_DEBUG(fmt,arg...)   do{\
                                    if(GTP_DEBUG_ON)\
                                        printf("<<-GTP-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
								}while(0)

#define GTP_DEBUG_ARRAY(array, num)     do{\
                                            int32_t i;\
                                            uint8_t* a = array;\
                                            if(GTP_DEBUG_ARRAY_ON){\
                                                printf("<<-GTP-DEBUG-ARRAY->>\n");\
                                                for (i = 0; i < (num); i++){\
                                                    printf("%02x   ", (a)[i]);\
                                                    if ((i + 1 ) %10 == 0)\
                                                        printf("\n");\
                                                }\
                                                printf("\n");\
                                            }\
                                        }while(0)

#define GTP_DEBUG_FUNC()    do{\
                                if(GTP_DEBUG_FUNC_ON)\
                                    printf("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                            }while(0)

																			 
																			 
#define GTP_SWAP(x, y)  do{\
                            typeof(x) z = x;\
                            x = y;\
                            y = z;\
                        }while (0)

int32_t GTP_Init_Panel(void);
int32_t GTP_Reset_Guitar(void);
int32_t GTP_Read_Version(void);
int32_t GTP_Send_Command(uint8_t command);
void GTP_IRQ_Enable(void);
void GTP_IRQ_Disable(void);

#endif