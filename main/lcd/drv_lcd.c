#include "drv_lcd.h"
#include "drv_pca9557.h"

static const char *TAG = "lcd";


// 背光PWM初始化
esp_err_t bsp_display_brightness_init(void)
{
    // Setup LEDC peripheral for PWM backlight control
    const ledc_channel_config_t LCD_backlight_channel = {
        .gpio_num = BSP_LCD_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LCD_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = 0,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = true
    };
    const ledc_timer_config_t LCD_backlight_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = 0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(ledc_timer_config(&LCD_backlight_timer));
    ESP_ERROR_CHECK(ledc_channel_config(&LCD_backlight_channel));

    return ESP_OK;
}

// 背光亮度设置
esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (brightness_percent > 100)
    {
        brightness_percent = 100;
    }
    else if (brightness_percent < 0)
    {
        brightness_percent = 0;
    }

    ESP_LOGI(TAG, "Setting LCD backlight: %d%%", brightness_percent);
    // LEDC resolution set to 10bits, thus: 100% = 1023
    uint32_t duty_cycle = (1023 * brightness_percent) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH, duty_cycle));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH));

    return ESP_OK;
}

// 关闭背光
esp_err_t bsp_display_backlight_off(void)
{
    return bsp_display_brightness_set(0);
}

// 打开背光 最亮
esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(100);
}

esp_lcd_panel_handle_t panel_handle = NULL;
esp_lcd_panel_io_handle_t io_handle = NULL;

esp_err_t bsp_display_new(void)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Backlight init failed");
    const spi_bus_config_t spi_bus_conf = {
        .miso_io_num = -1,
        .mosi_io_num = BSP_LCD_SPI_MOSI,
        .sclk_io_num = BSP_LCD_SPI_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0,
        .max_transfer_sz = BSP_LCD_H_RES * BSP_LCD_V_RES * sizeof(uint16_t),
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_LCD_SPI_NUM, &spi_bus_conf, SPI_DMA_CH_AUTO), TAG, "SPI bus initialization failed");
    const esp_lcd_panel_io_spi_config_t io_conf = {
        .dc_gpio_num = BSP_LCD_DC,
        .cs_gpio_num = BSP_LCD_SPI_CS,
        .pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 2,
        .trans_queue_depth = 10,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM, &io_conf, &io_handle), err, TAG, "LCD panel IO init failed");
    const esp_lcd_panel_dev_config_t panel_conf = {
        .reset_gpio_num = BSP_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = BSP_LCD_BITS_PER_PIXEL,
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7789(io_handle, &panel_conf, &panel_handle), err, TAG, "LCD panel init failed");

    esp_lcd_panel_reset(panel_handle); // 液晶屏复位
    lcd_cs(0);                         // 拉低CS引脚
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, true, false);
    esp_lcd_panel_swap_xy(panel_handle, true);

    return ret;
err:
    if (panel_handle)
    {
        esp_lcd_panel_del(panel_handle);
    }
    if (io_handle)
    {
        esp_lcd_panel_io_del(io_handle);
    }
    spi_bus_free(BSP_LCD_SPI_NUM);
    return ret;
}

esp_err_t bsp_lcd_init(void)
{
    esp_err_t ret = ESP_OK;
    ret = bsp_display_new();                             // 液晶屏驱动初始化
    lcd_set_color(0x0000);                               // 设置整屏背景黑色
    ret = esp_lcd_panel_disp_on_off(panel_handle, true); // 打开液晶屏显示
    ret = bsp_display_backlight_on();                    // 打开背光显示

    return ret;
}

void lcd_set_color(uint16_t color)
{
    uint16_t *buffer = (uint16_t *)heap_caps_malloc(BSP_LCD_H_RES * sizeof(uint16_t), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if(NULL == buffer)
    {
        ESP_LOGE(TAG, "malloc buffer failed");
    }
    else
    {
        for(size_t i = 0; i < BSP_LCD_H_RES; i++)
        {
            buffer[i] = color;
        }
        for (int y = 0; y < BSP_LCD_V_RES; y++)
        {
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, BSP_LCD_H_RES, y+1, buffer);
        }
        free(buffer);
    }    
    
}

void lcd_draw_pictrue(int x_start, int y_start, int x_end, int y_end, const unsigned char *gImage)
{
    size_t pixels_byte_size = (x_end - x_start) * (y_end - y_start) * sizeof(uint16_t);
    uint16_t *pixels = (uint16_t *)heap_caps_malloc(pixels_byte_size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    if(NULL == pixels)
    {
        ESP_LOGE(TAG, "malloc buffer failed");
        return;
    }
    // 拷贝图像数据到内存
    memcpy(pixels, gImage, pixels_byte_size);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_end, y_end, (uint16_t *)pixels);
    heap_caps_free(pixels);
}
