idf_component_register(SRCS "main.c" "lcd/drv_lcd.c"  "lcd/drv_lcd_touch.c" "pca9557/drv_pca9557.c" "camera/drv_camera.c" "boards_tasks.c" "lv_port/lv_port_disp.c" "lv_port/lv_port_indev.c"
                    INCLUDE_DIRS "." "lcd" "pca9557" "camera" "lv_port" 
                    REQUIRES esp_driver_i2c esp_lcd esp_driver_spi esp_driver_ledc lvgl
                    )

