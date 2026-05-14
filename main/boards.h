#ifndef _BOARDS_H_
#define _BOARDS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_camera.h"

// I2C
#define BSP_I2C_SDA         (GPIO_NUM_1)
#define BSP_I2C_SCL         (GPIO_NUM_2)

#define BSP_I2C_NUMBER      (0)
#define BSP_I2C_CLK_SPEED   (100000)

esp_err_t bsp_i2c_init(void);
esp_err_t bsp_i2c_get_bus_handle(i2c_master_bus_handle_t *ret_bus_handle);


// IO EXPANDER
#define PCA9557_INPUT_PORT          0x00
#define PCA9557_OUTPUT_PORT         0x01
#define PCA9557_POLARITY_INVERSION  0x02
#define PCA9557_CONFIGURATION_PORT  0x03

#define LCD_CS_GPIO                 BIT(0)   // PCA9557_GPIO_NUM_1
#define PA_EN_GPIO                  BIT(1)    // PCA9557_GPIO_NUM_2
#define DVP_PWDN_GPIO               BIT(2) // PCA9557_GPIO_NUM_3

#define PCA9557_SENSOR_ADDR         0x19 /*!< Slave address of the MPU9250 sensor */

#define SET_BITS(_m, _s, _v)        ((_v) ? (_m) | ((_s)) : (_m) & ~((_s)))

// LCD
#define BSP_LCD_PIXEL_CLOCK_HZ (80 * 1000 * 1000)
#define BSP_LCD_SPI_NUM (SPI3_HOST)
#define LCD_CMD_BITS (8)
#define LCD_PARAM_BITS (8)
#define BSP_LCD_BITS_PER_PIXEL (16)
#define LCD_LEDC_CH LEDC_CHANNEL_0

#define BSP_LCD_H_RES (320)
#define BSP_LCD_V_RES (240)

#define BSP_LCD_SPI_MOSI (GPIO_NUM_40)
#define BSP_LCD_SPI_CLK (GPIO_NUM_41)
#define BSP_LCD_SPI_CS (GPIO_NUM_NC)
#define BSP_LCD_DC (GPIO_NUM_39)
#define BSP_LCD_RST (GPIO_NUM_NC)
#define BSP_LCD_BACKLIGHT (GPIO_NUM_42)

#define LCD_TOUCH_ADDR      0x38

// CAMERA
#define CAMERA_PIN_PWDN -1
#define CAMERA_PIN_RESET -1
#define CAMERA_PIN_XCLK 5
#define CAMERA_PIN_SIOD 1
#define CAMERA_PIN_SIOC 2

#define CAMERA_PIN_D7 9
#define CAMERA_PIN_D6 4
#define CAMERA_PIN_D5 6
#define CAMERA_PIN_D4 15
#define CAMERA_PIN_D3 17
#define CAMERA_PIN_D2 8
#define CAMERA_PIN_D1 18
#define CAMERA_PIN_D0 16
#define CAMERA_PIN_VSYNC 3
#define CAMERA_PIN_HREF 46
#define CAMERA_PIN_PCLK 7

#define XCLK_FREQ_HZ 24000000

#endif
