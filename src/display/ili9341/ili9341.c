#include "ili9341.h"
#include "ili9341_commands.h"

#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "stdio.h"
#include "stdlib.h"

// Internal Configuration: -------------------------------------------------------------
const uint16_t BACKLIGHT_PWM_WRAP  = 25000;
const float BACKLIGHT_CLOCK_DIV    = 1.0f;

// -------------------------------------------------------------------------------------

typedef struct {
    uint32_t disp_spi_dma_channel;
    dma_channel_config_t disp_spi_dma_channel_config;

    ili9341_display_config* config;
} __ili9341_display;

// Declaration of private functions: ---------------------------------------------------
spi_inst_t* __get_gpio_spi_inst(uint32_t gpio);

// Public Functions: -------------------------------------------------------------------
ili9341_display* create_display(ili9341_display_config* config)
{
    __ili9341_display* display = malloc(sizeof(__ili9341_display));

    spi_inst_t* spi_inst = __get_gpio_spi_inst(config->scl_gpio);
    if(!spi_inst)
    {
#ifdef _DEBUG
        printf("get_gpio_spi_inst(config->scl_gpio) returned NULL");
#endif
        return NULL;
    }

    // Setup the SPI bus for the display including the CS, RESET and the DC pin. 
    uint32_t cs_dc_reset_gpio_mask = 0u | (1u << config->cs_gpio) | (1u << config->dc_gpio) | (1u << config->reset_gpio);
    gpio_set_dir_masked(cs_dc_reset_gpio_mask, GPIO_OUT);
    gpio_put_masked(cs_dc_reset_gpio_mask, true);   // All the pins are active low signals.
    gpio_set_function_masked(cs_dc_reset_gpio_mask, GPIO_FUNC_SIO);

    spi_init(spi_inst, config->spi_clk_khz * 1000);
    uint32_t scl_mosi_miso_gpio_mask = 0u | (1u << config->scl_gpio) | (1u << config->mosi_gpio) | (1u << config->miso_gpio);
    gpio_set_function_masked(scl_mosi_miso_gpio_mask, GPIO_FUNC_SPI);
    spi_set_format(spi_inst, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // Setup DMA from the memory to the SPI peripheral.
    // Panic if the there are no unused DMA channel
    int disp_spi_dma_channel = dma_claim_unused_channel(true);
    dma_channel_config_t dma_config = dma_channel_get_default_config(disp_spi_dma_channel);
    channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_8);
    channel_config_set_dreq(&dma_config, (spi_inst == spi0) ? DREQ_SPI0_TX : DREQ_SPI1_TX);

    // Setup PWM for the display backlight. Used to set the brightness of the display.
    gpio_set_function(config->backlight_gpio, GPIO_FUNC_PWM);
    pwm_config backlight_pwm_config = pwm_get_default_config();
    pwm_config_set_clkdiv(&backlight_pwm_config, BACKLIGHT_CLOCK_DIV);
    pwm_config_set_wrap(&backlight_pwm_config, BACKLIGHT_PWM_WRAP);
    pwm_set_gpio_level(config->backlight_gpio, BACKLIGHT_PWM_WRAP / 2);  // Initially set the brightness to half
    pwm_init(PWM_GPIO_SLICE_NUM(config->backlight_gpio), &backlight_pwm_config, true);

    display->config = config;
    display->disp_spi_dma_channel = disp_spi_dma_channel;
    display->disp_spi_dma_channel_config = dma_config;

    return (ili9341_display*)display;
}

void set_backlight_brightness(ili9341_display* display, float brightness)
{
    __ili9341_display* disp = (__ili9341_display*)display;
    pwm_set_gpio_level(disp->config->backlight_gpio, (uint16_t)(brightness * BACKLIGHT_PWM_WRAP));
}

// Private Functions: ------------------------------------------------------------------

spi_inst_t* __get_gpio_spi_inst(uint32_t gpio)
{
    switch(gpio)
    {
        case 0: case 1: case 2: case 3:
        case 4: case 5: case 6: case 7:
        case 16: case 17: case 18: case 19:
        case 20: case 21: case 22: case 23:
            return spi0;

        case 8: case 9: case 10: case 11:
        case 12: case 13: case 14: case 15:
        case 24: case 25: case 26: case 27:
            return spi1;

        default:
            return NULL;
    }
}