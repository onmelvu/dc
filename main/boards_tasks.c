#include "drv_lcd.h"
#include "drv_pca9557.h"
#include "drv_camera.h"
#include "boards_tasks.h"

// 定义lcd显示队列句柄
static QueueHandle_t xQueueLCDFrame = NULL;

static void task_process_lcd(void *arg)
{
    camera_fb_t *frame = NULL;
    while (true)
    {
        if(xQueueReceive(xQueueLCDFrame, &frame, portMAX_DELAY))
        {
            esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, frame->width, frame->height, (uint16_t *)frame->buf);
            esp_camera_fb_return(frame);
        }
    }
    
}

static void task_process_camera(void *arg)
{
    while (true)
    {
        camera_fb_t *frame = esp_camera_fb_get();
        if(frame)
        {
            xQueueSend(xQueueLCDFrame, &frame, portMAX_DELAY);
        }
    }
    
}

void app_camera_lcd(void)
{
    xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xTaskCreatePinnedToCore(task_process_camera, "task_process_camera", 3072, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(task_process_lcd, "task_process_lcd", 4096, NULL, 5, NULL, 0);
}
