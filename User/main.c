#include "main.h"
#include "../lvgl.h"
#include "bsp_init.h"
#include "clock_init.h"
#include "../../lv_demo.h"
#include "../examples/porting/lv_port_disp.h"
#include "../examples/porting/lv_port_indev.h"

typedef struct {
    lv_obj_t *btn;
    lv_obj_t *label;
} led_btn_t;

typedef struct {
    led_btn_t btn1;
    led_btn_t btn2;
    led_btn_t btn3;
    led_btn_t btn4;
}led_obj_t;

led_obj_t LED_BTN;

static void btn1_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LED_RGBOFF;
        LED1(ON);
        LV_LOG_USER("Clicked");
    }
}

static void btn2_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LED_RGBOFF;
        LED2(ON);
        LV_LOG_USER("Clicked");
    }
}

static void btn3_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LED_RGBOFF;
        LED3(ON);
        LV_LOG_USER("Clicked");
    }
}

static void btn4_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LED_RGBOFF;
        LV_LOG_USER("Clicked");
    }
}

led_btn_t *btn_arr[] = { &LED_BTN.btn1, &LED_BTN.btn2, &LED_BTN.btn3, &LED_BTN.btn4 };
const char *text_buf[] = { "Red", "Green", "Blue", "Close" };
void *event[] = { &btn1_event_handler, &btn2_event_handler, &btn3_event_handler, &btn4_event_handler };

void test_led(void)
{
    uint32_t i = 0;
    for (i = 0; i < 4; i++) {
        btn_arr[i]->btn = lv_btn_create(lv_scr_act());
        lv_obj_set_size(btn_arr[i]->btn, 160, 80);
        lv_obj_set_pos(btn_arr[i]->btn, 32 + i * 192, 200);
        lv_obj_add_event_cb(btn_arr[i]->btn, event[i], LV_EVENT_ALL, NULL);

        btn_arr[i]->label = lv_label_create(btn_arr[i]->btn);
        lv_label_set_text(btn_arr[i]->label, text_buf[i]);
        lv_obj_center(btn_arr[i]->label);
    }
}


int main(void)
{
    HAL_Init();
    SystemClock_Config();
    bsp_Init();
	
	lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
	// lv_demo_widgets();
    test_led();

    for (;;)
    {
        lv_task_handler();
        HAL_Delay(1);
    }
}
