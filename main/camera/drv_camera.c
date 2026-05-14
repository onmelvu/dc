#include "drv_camera.h"
#include "drv_pca9557.h"

static const char *TAG = "CAMERA";

void bsp_camera_init(void)
{
    dvp_pwdn(0);// 打开摄像头
    camera_config_t camera_conf = {
        .ledc_channel = LEDC_CHANNEL_1,
        .ledc_timer = LEDC_TIMER_1,
        .pin_d0 = CAMERA_PIN_D0,
        .pin_d1 = CAMERA_PIN_D1,
        .pin_d2 = CAMERA_PIN_D2,
        .pin_d3 = CAMERA_PIN_D3,
        .pin_d4 = CAMERA_PIN_D4,
        .pin_d5 = CAMERA_PIN_D5,
        .pin_d6 = CAMERA_PIN_D6,
        .pin_d7 = CAMERA_PIN_D7,
        .pin_xclk = CAMERA_PIN_XCLK,
        .pin_pclk = CAMERA_PIN_PCLK,
        .pin_vsync = CAMERA_PIN_VSYNC,
        .pin_href = CAMERA_PIN_HREF,
        .pin_sccb_sda = -1, // 这里写-1 表示使用已经初始化的I2C接口
        .pin_sccb_scl = CAMERA_PIN_SIOC,
        .sccb_i2c_port = 0,
        .pin_pwdn = CAMERA_PIN_PWDN,
        .pin_reset = CAMERA_PIN_RESET,
        .xclk_freq_hz = XCLK_FREQ_HZ,
        .pixel_format = PIXFORMAT_RGB565,
        .frame_size = FRAMESIZE_QVGA,
        .jpeg_quality = 12,
        .fb_count = 2,
        .fb_location = CAMERA_FB_IN_PSRAM,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
    };
    esp_err_t ret = esp_camera_init(&camera_conf);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return;
    }
    sensor_t *sensor = esp_camera_sensor_get();// 获取摄像头型号
    if (sensor->id.PID == GC0308_PID)
    {
        sensor->set_hmirror(sensor, 1); // 控制摄像头镜像 写1镜像 写0不镜像
    }
}

