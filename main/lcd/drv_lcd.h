#ifndef DRV_LVD_H_
#define DRV_LVD_H_
#include "boards.h"

extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t io_handle;

esp_err_t bsp_display_brightness_init(void);
esp_err_t bsp_display_brightness_set(int brightness_percent);
esp_err_t bsp_display_backlight_off(void);
esp_err_t bsp_display_backlight_on(void);
esp_err_t bsp_lcd_init(void);
void lcd_set_color(uint16_t color);
void lcd_draw_pictrue(int x_start, int y_start, int x_end, int y_end, const unsigned char *gImage);

#endif
