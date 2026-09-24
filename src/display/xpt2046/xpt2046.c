#include "xpt2046.h"

#include <stdlib.h>
// TEMP
#include <stdio.h>

#include "hardware/spi.h"

// XPT2046 commands: --------------------------------------------------------------
#define XPT2046_CMD_X   0xD0
#define XPT2046_CMD_Y   0x90
#define XPT2046_CMD_Z1  0xB0
#define XPT2046_CMD_Z2  0xC0
// --------------------------------------------------------------------------------

typedef struct {
    xpt2046_touch_config* config;
    spi_inst_t* spi_inst;

} __xpt2046_touch_controller;

__xpt2046_touch_controller* g_xpt2046_touch_controller;


// Declaration of private functions: ----------------------------------------------
spi_inst_t* __get_gpio_spi_inst(uint32_t gpio); // TODO: move this to a common header.

void __write_spi_blocking(uint8_t* data);
// --------------------------------------------------------------------------------


// Public functions: --------------------------------------------------------------
void touch_controller_create(xpt2046_touch_config* config)
{
    g_xpt2046_touch_controller = malloc(sizeof(__xpt2046_touch_controller));

    g_xpt2046_touch_controller->config = config;

    // TODO: We are cooked if the SCL GPIO corresponds to spi0 and other (MOSI, MISO) corresponds to spi1
    spi_inst_t* spi_inst = __get_gpio_spi_inst(config->sck_gpio);
    if(!spi_inst)
    {
#ifdef _DEBUG
        printf("get_gpio_spi_inst(config->scl_gpio) returned NULL");
#endif
        return;
    }

    g_xpt2046_touch_controller->spi_inst = spi_inst;
    
    // Init the IRQ and CS gpio, IRQ is input, CS is output
    uint32_t cs_irq_gpio_mask = 0u | (1u << config->cs_gpio) | (1u << config->irq_gpio);
    gpio_set_dir_masked(cs_irq_gpio_mask, 0u | (1u << config->cs_gpio)); // Make only CS output
    gpio_pull_up(config->cs_gpio);
    gpio_set_function_masked(cs_irq_gpio_mask, GPIO_FUNC_SIO);

    spi_init(spi_inst, config->spi_clk_khz * 1000);
    uint32_t sck_mosi_miso_gpio_mask = 0u | (1u << config->sck_gpio) | (1u << config->mosi_gpio) | (1u << config->miso_gpio);
    gpio_set_function_masked(sck_mosi_miso_gpio_mask, GPIO_FUNC_SPI);
    spi_set_format(spi_inst, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

bool touched()
{
    return !(gpio_get(g_xpt2046_touch_controller->config->irq_gpio));
}

void read_raw(uint16_t* x, uint16_t* y)
{
    uint8_t data[] = { XPT2046_CMD_X, 0x00, 0x00 };
    uint8_t rx[3];

    // Read X
    gpio_put(g_xpt2046_touch_controller->config->cs_gpio, false);
    spi_write_read_blocking(g_xpt2046_touch_controller->spi_inst, data, rx, 3 * sizeof(uint8_t));
    gpio_put(g_xpt2046_touch_controller->config->cs_gpio, true);

    *x = (uint16_t)((uint16_t)rx[1] << 4) | ((uint16_t)rx[2] >> 4);
    data[0] = XPT2046_CMD_Y;
    
    // Read Y
    gpio_put(g_xpt2046_touch_controller->config->cs_gpio, false);
    spi_write_read_blocking(g_xpt2046_touch_controller->spi_inst, data, rx, 3 * sizeof(uint8_t));
    gpio_put(g_xpt2046_touch_controller->config->cs_gpio, true);
    
    *y = (uint16_t)((uint16_t)rx[1] << 4) | ((uint16_t)rx[2] >> 4);
}

void read_xy(uint16_t* x, uint16_t* y, bool flip_x, bool flip_y, bool rotate)
{
    
    // Intuition based calibartion
    // TODO: Add a calibration function

    // X: 90 - 1920 : 0 - 239
    // Y: 190 - 1950 : 0 - 319

    uint16_t rawX, rawY;
    read_raw(&rawX, &rawY);

    uint16_t w = rotate ? g_xpt2046_touch_controller->config->height : g_xpt2046_touch_controller->config->width;
    uint16_t h = rotate ? g_xpt2046_touch_controller->config->width : g_xpt2046_touch_controller->config->height;

    *x = (uint16_t)(((float)(rawX - 90) / (float)(1920 - 90)) * w);
    *y = (uint16_t)(((float)(rawY - 190) / (float)(1950 - 190)) * h);

    *x = flip_x ? (w - 1) - *x : *x;
    *y = flip_y ? (h - 1) - *y : *y;
}

void lvgl_touch_read_cb(lv_indev_t* indev, lv_indev_data_t* data)
{
    bool flip_x = false, flip_y = true, rotate = false;
    if (touched())
    {
        uint16_t x, y;
        read_xy(&x, &y, flip_x, flip_y, rotate);

        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else
        data->state = LV_INDEV_STATE_RELEASED;
}
// ---------------------------------------------------------------------------------