#pragma once
#include <stdint.h>
#include <pico/stdlib.h>
#include <lvgl.h>

typedef struct {
    void* data;
} ili9341_display;

typedef struct {
    uint16_t width, height;
    uint scl_gpio, mosi_gpio, miso_gpio, dc_gpio, cs_gpio, reset_gpio, backlight_gpio;

    uint32_t spi_clk_khz;
} ili9341_display_config;


ili9341_display* create_display(ili9341_display_config* config);  /* Creates and initializes the display */

void set_backlight_brightness(ili9341_display* display, float brightness); /* Sets the bightness of the display. 0.0f: completely dark, 1.0f: completely bright*/

// Used by LVGL
void lvgl_lcd_flash_cb(lv_display_t* disp, const lv_area_t* area, uint8_t* color_data);