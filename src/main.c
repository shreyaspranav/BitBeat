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
    stdio_init_all();

    ili9341_display_config* disp_config = malloc(sizeof(ili9341_display_config));
    disp_config->width = DISPLAY_HOR_RES;
    disp_config->height = DISPLAY_VER_RES;

    disp_config->backlight_gpio = DISPLAY_LED;
    disp_config->mosi_gpio = DISPLAY_MOSI;
    disp_config->reset_gpio = DISPLAY_RESET;
    disp_config->miso_gpio = DISPLAY_MISO;
    disp_config->scl_gpio = DISPLAY_SCK;
    disp_config->dc_gpio = DISPLAY_DC;
    disp_config->cs_gpio = DISPLAY_CS;

    disp_config->spi_clk_khz = 60000;

    ili9341_display* ili9341_disp = create_display(disp_config);
    set_backlight_brightness(ili9341_disp, 1.0f);

    lv_init();

    lv_display_t* disp = lv_display_create(DISPLAY_HOR_RES, DISPLAY_VER_RES);
    lv_display_set_driver_data(disp, ili9341_disp);
    lv_display_set_flush_cb(disp, lvgl_lcd_flash_cb);
    lv_draw_buf_t* buf1 = lv_draw_buf_create(DISPLAY_HOR_RES, DISPLAY_VER_RES / 2, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_draw_buf_t* buf2 = lv_draw_buf_create(DISPLAY_HOR_RES, DISPLAY_VER_RES / 2, LV_COLOR_FORMAT_RGB565, LV_STRIDE_AUTO);
    lv_display_set_draw_buffers(disp, buf1, buf2);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_obj_t* screen = lv_obj_create(NULL);
    lv_screen_load(screen);

    char* s[] = { "benchmark" };
    printf("%d", lv_demos_create(s, 1));

    absolute_time_t last = get_absolute_time();

    while (1)
    {
        if (absolute_time_diff_us(last, get_absolute_time()) >= 500) {
            lv_tick_inc(1);
            last = get_absolute_time();
        }

        lv_timer_handler();
    }
}
