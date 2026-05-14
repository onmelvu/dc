#include <stdio.h>
#include "drv_pca9557.h"
#include "drv_camera.h"
#include "drv_lcd.h"
#include "yingwu.h"
#include "boards_tasks.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl.h"
#include "esp_timer.h"
#include "lv_conf.h"

static esp_timer_handle_t lvgl_tick_timer = NULL;

// 定时回调函数，每 1ms 触发
static void lv_tick_task(void *arg)
{
    lv_tick_inc(1);
}

// 初始化 LVGL Tick 定时器
void lvgl_tick_timer_init(void)
{
    const esp_timer_create_args_t timer_args = {
        .callback = &lv_tick_task,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lv_tick_timer"};

    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 1000); // 1ms 触发
}

 void app_main(void)
{
    bsp_i2c_init();                   // I2C初始化
    pca9557_init();                                  // IO扩展芯片初始化
    // bsp_lcd_init();                                  // 液晶屏初始化
    lvgl_tick_timer_init();

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    // bsp_camera_init();
    // app_camera_lcd();

    lv_obj_t *switch_obj = lv_switch_create(lv_scr_act());
    lv_obj_set_size(switch_obj, 120, 50);
    lv_obj_align(switch_obj, LV_ALIGN_CENTER, 0, 0);

    while (1)
    {
        lv_task_handler();             // LVGL 任务管理
        vTaskDelay(pdMS_TO_TICKS(10)); // 延迟 10ms
    }
}
