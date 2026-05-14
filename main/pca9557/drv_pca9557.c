#include "drv_pca9557.h"

static const char *TAG = "PCA9557";
static i2c_master_bus_handle_t s_i2c_bus_handle = NULL;
static i2c_master_dev_handle_t s_pca9557_dev_handle = NULL;

esp_err_t bsp_i2c_init(void)
{
    if (s_i2c_bus_handle && s_pca9557_dev_handle) {
        return ESP_OK;
    }

    esp_err_t ret = ESP_OK;
    i2c_master_bus_config_t bsp_i2c_conf = {
        .i2c_port = BSP_I2C_NUMBER,
        .sda_io_num = BSP_I2C_SDA,
        .scl_io_num = BSP_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags.enable_internal_pullup = true,
    };
    ret = i2c_new_master_bus(&bsp_i2c_conf, &s_i2c_bus_handle);
    if (ret == ESP_ERR_INVALID_STATE || ret == ESP_ERR_NOT_FOUND) {
        // Bus already exists on this port, get its handle.
        ret = i2c_master_get_bus_handle(BSP_I2C_NUMBER, &s_i2c_bus_handle);
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "i2c bus init failed");

    if (!s_pca9557_dev_handle) {
        i2c_device_config_t pca9557_dev_conf = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = PCA9557_SENSOR_ADDR,
            .scl_speed_hz = BSP_I2C_CLK_SPEED,
            .scl_wait_us = 0,
            .flags.disable_ack_check = 0,
        };
        ret = i2c_master_bus_add_device(s_i2c_bus_handle, &pca9557_dev_conf, &s_pca9557_dev_handle);
        ESP_RETURN_ON_ERROR(ret, TAG, "pca9557 i2c device add failed");
    }

    return ESP_OK;
}

esp_err_t bsp_i2c_get_bus_handle(i2c_master_bus_handle_t *ret_bus_handle)
{
    if (!ret_bus_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "i2c init failed");
    *ret_bus_handle = s_i2c_bus_handle;
    return ESP_OK;
}

esp_err_t pca9557_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    ESP_RETURN_ON_FALSE(s_pca9557_dev_handle != NULL, ESP_ERR_INVALID_STATE, TAG, "pca9557 device not initialized");
    return i2c_master_transmit_receive(s_pca9557_dev_handle, &reg_addr, 1, data, len, 1000);
}

esp_err_t pca9557_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    ESP_RETURN_ON_FALSE(s_pca9557_dev_handle != NULL, ESP_ERR_INVALID_STATE, TAG, "pca9557 device not initialized");
    return i2c_master_transmit(s_pca9557_dev_handle, write_buf, sizeof(write_buf), 1000);
}

void pca9557_init(void)
{
    // 写入控制引脚默认值 DVP_PWDN=1  PA_EN = 0  LCD_CS = 1
    pca9557_register_write_byte(PCA9557_OUTPUT_PORT, 0x05);
    // 把PCA9557芯片的IO1 IO1 IO2设置为输出 其它引脚保持默认的输入
    pca9557_register_write_byte(PCA9557_CONFIGURATION_PORT, 0xf8);
}

// 设置PCA9557芯片的某个IO引脚输出高低电平
esp_err_t pca9557_set_output_state(uint8_t gpio_bit, uint8_t level)
{
    uint8_t data;
    esp_err_t res = ESP_FAIL;

    pca9557_register_read(PCA9557_OUTPUT_PORT, &data, 1);
    res = pca9557_register_write_byte(PCA9557_OUTPUT_PORT, SET_BITS(data, gpio_bit, level));

    return res;
}

void lcd_cs(uint8_t level)
{
    pca9557_set_output_state(LCD_CS_GPIO, level);
}

void pa_en(uint8_t level)
{
    pca9557_set_output_state(PA_EN_GPIO, level);
}

void dvp_pwdn(uint8_t level)
{
    pca9557_set_output_state(DVP_PWDN_GPIO, level);
}
