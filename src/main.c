#include <stdio.h>
#include "pico/stdlib.h"
#include "stdlib.h"

#include "config.h"
#include "display/ili9341/ili9341.h"
#include "hardware/clocks.h"

#include <lvgl.h>
#include "demos/lv_demos.h"

int main()
{
    if (!set_sys_clock_khz(SYS_CLOCK, true)) {
        return 0;
    }
    stdio_init_all();

    sleep_ms(SLEEP_ON_STARTUP);

    ili9341_display_config* disp_config = malloc(sizeof(ili9341_display_config));
    disp_config->height = DISPLAY_HOR_RES;
    disp_config->width = DISPLAY_VER_RES;

    disp_config->backlight_gpio = DISPLAY_LED;
    disp_config->mosi_gpio = DISPLAY_MOSI;
    disp_config->reset_gpio = DISPLAY_RESET;
    disp_config->miso_gpio = DISPLAY_MISO;
    disp_config->scl_gpio = DISPLAY_SCK;
    disp_config->dc_gpio = DISPLAY_DC;
    disp_config->cs_gpio = DISPLAY_CS;

    disp_config->spi_clk_khz = DISPLAY_SPI_CLOCK;

    create_display(disp_config);
    set_backlight_brightness(1.0f);

    lv_init();

    lv_display_t* disp = lv_display_create(DISPLAY_VER_RES, DISPLAY_HOR_RES);
    lv_display_set_flush_cb(disp, lvgl_lcd_flash_cb);
    lv_draw_buf_t* buf1 = lv_draw_buf_create(DISPLAY_VER_RES, DISPLAY_HOR_RES / 4, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_draw_buf_t* buf2 = lv_draw_buf_create(DISPLAY_VER_RES, DISPLAY_HOR_RES / 4, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_display_set_draw_buffers(disp, buf1, buf2);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    char* s[] = { "music" };
    printf("%d", lv_demos_create(s, 1));

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
}
