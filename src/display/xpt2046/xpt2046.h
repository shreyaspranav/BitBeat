#pragma once

#include <stdint.h>
#include <pico/stdlib.h>
#include <lvgl.h>

typedef struct {
    uint16_t width, height;
    uint sck_gpio, mosi_gpio, miso_gpio, cs_gpio, irq_gpio;

    uint32_t spi_clk_khz;
} xpt2046_touch_config;

void touch_controller_create(xpt2046_touch_config* config);

bool touched();
void read_raw(uint16_t* x, uint16_t* y);
void read_xy(uint16_t* x, uint16_t* y, bool flip_x, bool flip_y, bool rotate);

// LVGL polls this function
void lvgl_touch_read_cb(lv_indev_t* indev, lv_indev_data_t* data);
