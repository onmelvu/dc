#ifndef _DRV_LCD_TOUCH_H_
#define _DRV_LCD_TOUCH_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

esp_err_t bsp_touch_init(void);
esp_err_t bsp_touch_read(uint16_t *x, uint16_t *y, bool *pressed);

#endif