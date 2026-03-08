#include "ili9341.h"
#include "ili9341_commands.h"
#include "test_image.h"

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

#define DC_COMMAND  false
#define DC_DATA     true

typedef struct {
    uint32_t disp_spi_dma_channel;
    dma_channel_config_t disp_spi_dma_channel_config;
    spi_inst_t* disp_spi_inst;

    ili9341_display_config* config;
} __ili9341_display;

// Declaration of private functions: ---------------------------------------------------
spi_inst_t* __get_gpio_spi_inst(uint32_t gpio);
void __send_init_seq(__ili9341_display* display);

void __write_spi_cmd_param_blocking(__ili9341_display* display, uint8_t* buffer, size_t len);
void __write_spi_disp_data_dma(__ili9341_display* display, const uint8_t* data, size_t length);

void __test__(__ili9341_display* display);

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
    uint32_t cs_dc_reset_gpio_mask = (1u << config->cs_gpio) | (1u << config->dc_gpio) | (1u << config->reset_gpio);
    gpio_init_mask(cs_dc_reset_gpio_mask);

    // Learned the hard way, the second parameter is not supposed to be 'true' -> it becomes 0x00000001 -> only SETS GPIO 0. 
    gpio_set_dir_masked(cs_dc_reset_gpio_mask, cs_dc_reset_gpio_mask);
    gpio_put_masked(cs_dc_reset_gpio_mask, cs_dc_reset_gpio_mask);   // All the pins are active low signals.
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
    channel_config_set_read_increment(&dma_config, true);
    channel_config_set_write_increment(&dma_config, false);

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
    display->disp_spi_inst = spi_inst;

    gpio_put(config->reset_gpio, 0);
    sleep_ms(50);
    gpio_put(config->reset_gpio, 1);
    sleep_ms(150);

    __send_init_seq(display);

    // TEMP: send a sample image.
    __test__(display);

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

void __send_init_seq(__ili9341_display* display)
{
    // Format: <cmd length> <sleep in ms> <cmds>
    // Format: <total bytes including command> <sleep in ms> <cmd + args>
    uint8_t init_seq[] =
    {
        1,  20, ILI9341_SWRESET,                                        // Software reset
        1, 120, ILI9341_SLPOUT,                                         // Exit sleep mode (required 120ms)
        6,   0, ILI9341_PWCTRLA, 0x39, 0x2C, 0x00, 0x34, 0x02,          // Power control A
        4,   0, ILI9341_PWCTRLB, 0x00, 0xA2, 0xF0,                      // Power control B
        2,   0, ILI9341_PWCTRL1, 0x21,                                  // Power control 1
        2,   0, ILI9341_PWCTRL2, 0x00,                                  // Power control 2
        2,   0, ILI9341_PIXFMT,  0x66,                                  // Pixel format: 18 bit both on RGB and MCU interface
        2,   0, ILI9341_MADCTL,  0x48,
        
        // Frame rate & display function control
        3,   0, ILI9341_FRMCTR1, 0x00, 0x18,   // Frame rate control (normal mode)
        4,   0, ILI9341_DISCTRL, 0x08, 0x82, 0x27,  // Display function control
        
        // Gamma correction
        2,   0, ILI9341_ENABLE3G, 0x00,        // Gamma function disable (no partial mode)
        2,   0, ILI9341_GAMMASET, 0x01,        // Gamma curve select (1 = default)
        16,  0, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Positive gamma
        16,  0, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Negative gamma
        
        // Final display enable
        1,   0, ILI9341_DISPON,                // Display ON
        0
    };

    uint8_t* seq_ptr = init_seq;
    while(*seq_ptr)
    {
        uint8_t* count_ptr = seq_ptr;
        uint8_t* delay_ptr = seq_ptr + (1 * sizeof(uint8_t));
        uint8_t* cmd_ptr   = seq_ptr + (2 * sizeof(uint8_t));

        __write_spi_cmd_param_blocking(display, cmd_ptr, *count_ptr);
        sleep_ms((uint32_t)(*delay_ptr));

        seq_ptr += (2 + *count_ptr) * sizeof(uint8_t);
    }

    printf("Init seq completed\n");
}

void __write_spi_cmd_param_blocking(__ili9341_display* display, uint8_t* data, size_t len)
{
    gpio_put(display->config->cs_gpio, false);

    // First byte is a command
    gpio_put(display->config->dc_gpio, DC_COMMAND);
    spi_write_blocking(display->disp_spi_inst, data, 1 * sizeof(uint8_t));

    // Next seqence of bytes are parameters, treat them as data
    if(len > 1)
    {
        gpio_put(display->config->dc_gpio, DC_DATA);
        spi_write_blocking(display->disp_spi_inst, data + 1, (len - 1) * sizeof(uint8_t));
    }
    gpio_put(display->config->cs_gpio, true);

}

void __write_spi_disp_data_dma(__ili9341_display* display, const uint8_t* data, size_t length)
{
    gpio_put(display->config->cs_gpio, false);
    gpio_put(display->config->dc_gpio, DC_COMMAND);
    uint8_t ramwr = ILI9341_RAMWR;
    spi_write_blocking(display->disp_spi_inst, &ramwr, 1);

    gpio_put(display->config->dc_gpio, DC_DATA);

    dma_channel_configure(
        display->disp_spi_dma_channel,
        &display->disp_spi_dma_channel_config,
        &spi_get_hw(display->disp_spi_inst)->dr,
        (void*)data,
        (uint32_t)length,
        true
    );

    dma_channel_wait_for_finish_blocking(display->disp_spi_dma_channel);
    while (spi_get_hw(display->disp_spi_inst)->sr & SPI_SSPSR_BSY_BITS) { tight_loop_contents(); }

    // deassert CS now that the transfer is complete
    gpio_put(display->config->cs_gpio, true);
}

void __test__(__ili9341_display* display)
{
    // Set row and column start and end to cover the entire screen
    uint16_t start_column = 0;
    uint16_t end_column   = display->config->width - 1;
    uint16_t start_row    = 0;
    uint16_t end_row      = display->config->height - 1;

    uint8_t caset_data[] = 
    {
        ILI9341_CASET,
        start_column >> 8, start_column & 0xFF,
        end_column   >> 8, end_column   & 0xFF,
    };

    uint8_t paset_data[] = 
    {
        ILI9341_PASET,
        start_row >> 8, start_row & 0xFF,
        end_row   >> 8, end_row   & 0xFF,
    };

    __write_spi_cmd_param_blocking(display, caset_data, 5);
    __write_spi_cmd_param_blocking(display, paset_data, 5);

    __write_spi_disp_data_dma(display, image_data_rgb666, 240 * 320 * 3);
}