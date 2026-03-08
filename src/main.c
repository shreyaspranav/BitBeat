#include <stdio.h>
#include "pico/stdlib.h"
#include "stdlib.h"

#include "config.h"
#include "display/ili9341/ili9341.h"

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

    disp_config->spi_clk_khz = 2000;

    ili9341_display* disp = create_display(disp_config);
    set_backlight_brightness(disp, 1.0f);

    for(;;);
}
