#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/clocks.h>

#include "config.h"
#include "display/ili9341/ili9341.h"
#include "display/xpt2046/xpt2046.h"

#include <stdlib.h>

#include <lvgl.h>

// Defined in ui/entry_point.c, runs in core0
void lvgl_ui_entry_point();
// Defined in audio/entry_point.c, runs in core1
void audio_processing_thread_entry_point();

void hardware_init()
{
    ili9341_display_config* disp_config = malloc(sizeof(ili9341_display_config));
    disp_config->width = DISPLAY_HOR_RES;
    disp_config->height = DISPLAY_VER_RES;

    disp_config->backlight_gpio = DISPLAY_LED;
    disp_config->mosi_gpio = DISPLAY_MOSI;
    disp_config->reset_gpio = DISPLAY_RESET;
    disp_config->miso_gpio = DISPLAY_MISO;
    disp_config->sck_gpio = DISPLAY_SCK;
    disp_config->dc_gpio = DISPLAY_DC;
    disp_config->cs_gpio = DISPLAY_CS;

    disp_config->spi_clk_khz = DISPLAY_SPI_CLOCK;

    xpt2046_touch_config* touch_config = malloc(sizeof(xpt2046_touch_config));
    touch_config->width = DISPLAY_HOR_RES;
    touch_config->height = DISPLAY_VER_RES;

    touch_config->mosi_gpio = TOUCH_MOSI;
    touch_config->miso_gpio = TOUCH_MISO;
    touch_config->sck_gpio = TOUCH_SCK;
    touch_config->cs_gpio = TOUCH_CS;
    touch_config->irq_gpio = TOUCH_IRQ;
    touch_config->spi_clk_khz = TOUCH_SPI_CLOCK;

    create_display(disp_config);
    set_backlight_brightness(1.0f);

    touch_controller_create(touch_config);
}

void lvgl_init()
{
    lv_init();

    lv_display_t* disp = lv_display_create(DISPLAY_HOR_RES, DISPLAY_VER_RES);
    lv_display_set_flush_cb(disp, lvgl_lcd_flash_cb);
    lv_draw_buf_t* buf1 = lv_draw_buf_create(DISPLAY_HOR_RES, DISPLAY_VER_RES / 4, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_draw_buf_t* buf2 = lv_draw_buf_create(DISPLAY_HOR_RES, DISPLAY_VER_RES / 4, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_display_set_draw_buffers(disp, buf1, buf2);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvgl_touch_read_cb);
}

int main()
{
    if (!set_sys_clock_khz(SYS_CLOCK, true)) {
        return 0;
    }
    stdio_init_all();

    sleep_ms(SLEEP_ON_STARTUP);

    hardware_init();
    lvgl_init();

    // Launch the audio processing in a different core
    multicore_launch_core1(audio_processing_thread_entry_point);

    lvgl_ui_entry_point();

    // Display update loop ------------------------------------------
    absolute_time_t last = get_absolute_time();
    while (true)
    {
        absolute_time_t now = get_absolute_time();
        int64_t elapsed_us = absolute_time_diff_us(last, now);

        if (elapsed_us >= 1000) {
            uint32_t ms = elapsed_us / 1000;
            lv_tick_inc(ms);
            last = delayed_by_us(last, ms * 1000);
        }
        lv_timer_handler();
    }
    // --------------------------------------------------------------
}
