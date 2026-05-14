#include "drv_lcd_touch.h"

#include "boards.h"
#include "drv_pca9557.h"
#include "esp_check.h"
#include "esp_lcd_touch_ft6x36.h"
#include "esp_log.h"

static const char *TAG = "ft6336";

static esp_lcd_touch_handle_t s_tp = NULL;
static esp_lcd_panel_io_handle_t s_tp_io_handle = NULL;

esp_err_t bsp_touch_init(void)
{
    esp_err_t ret = bsp_i2c_init();
    if (ret != ESP_OK) {
        return ret;
    }

    if (s_tp) {
        return ESP_OK;
    }

    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    ESP_RETURN_ON_ERROR(bsp_i2c_get_bus_handle(&i2c_bus_handle), TAG, "get i2c bus handle failed");

    esp_lcd_touch_config_t tp_cfg = {
        .x_max = BSP_LCD_V_RES,
        .y_max = BSP_LCD_H_RES,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 1,
            .mirror_x = 1,
            .mirror_y = 0,
        },
    };

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT6x36_CONFIG();
    // The FT5x06 helper macro doesn't set scl_speed_hz, but i2c_master (new driver) requires it.
    tp_io_config.scl_speed_hz = BSP_I2C_CLK_SPEED;

    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_bus_handle, &tp_io_config, &s_tp_io_handle), TAG, "new touch panel io failed");
    ESP_RETURN_ON_ERROR(esp_lcd_touch_new_i2c_ft6x36(s_tp_io_handle, &tp_cfg, &s_tp), TAG, "new touch failed");

    return ESP_OK;
}

esp_err_t bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed)
{
    if (!x || !y || !pressed) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!s_tp) {
        *pressed = false;
        return ESP_ERR_INVALID_STATE;
    }

    esp_lcd_touch_read_data(s_tp);

    uint16_t touch_x[1] = {0};
    uint16_t touch_y[1] = {0};
    uint16_t touch_strength[1] = {0};
    uint8_t touch_cnt = 0;

    bool is_pressed = esp_lcd_touch_get_coordinates(s_tp, touch_x, touch_y, touch_strength, &touch_cnt, 1);
    if (is_pressed && touch_cnt > 0) {
        *x = touch_x[0];
        *y = touch_y[0];
        *pressed = true;
    } else {
        *pressed = false;
    }

    return ESP_OK;
}


