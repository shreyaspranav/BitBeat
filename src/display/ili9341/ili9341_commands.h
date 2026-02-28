#pragma once

#define ILI9341_NOP                    0x00  // No operation
#define ILI9341_SWRESET                0x01  // Software reset
#define ILI9341_RDDID                  0x04  // Read display ID
#define ILI9341_RDDST                  0x09  // Read display status
#define ILI9341_RDDPM                  0x0A  // Read display power mode
#define ILI9341_RDDMADCTL              0x0B  // Read MADCTL register
#define ILI9341_RDDCOLMOD              0x0C  // Read pixel format
#define ILI9341_RDDIM                  0x0D  // Read image format
#define ILI9341_RDDSM                  0x0E  // Read signal mode
#define ILI9341_RDDSDR                 0x0F  // Read self diagnostic result

#define ILI9341_SLPIN                  0x10  // Enter sleep mode
#define ILI9341_SLPOUT                 0x11  // Exit sleep mode
#define ILI9341_PTLON                  0x12  // Partial mode ON
#define ILI9341_NORON                  0x13  // Normal display mode ON

#define ILI9341_INVOFF                 0x20  // Display inversion OFF
#define ILI9341_INVON                  0x21  // Display inversion ON
#define ILI9341_GAMMASET               0x26  // Gamma curve select

#define ILI9341_DISPOFF                0x28  // Display OFF
#define ILI9341_DISPON                 0x29  // Display ON

#define ILI9341_CASET                  0x2A  // Column address set (X start/end)
#define ILI9341_PASET                  0x2B  // Page address set (Y start/end)
#define ILI9341_RAMWR                  0x2C  // Memory write (send pixel data)
#define ILI9341_RGBSET                 0x2D  // Color lookup table write
#define ILI9341_RAMRD                  0x2E  // Memory read

#define ILI9341_PTLAR                  0x30  // Partial area definition
#define ILI9341_VSCRDEF                0x33  // Vertical scrolling definition

#define ILI9341_TEOFF                  0x34  // Tearing effect OFF
#define ILI9341_TEON                   0x35  // Tearing effect ON

#define ILI9341_MADCTL                 0x36  // Memory access control (rotation)
#define ILI9341_VSCRSADD               0x37  // Vertical scroll start address

#define ILI9341_IDMOFF                 0x38  // Idle mode OFF
#define ILI9341_IDMON                  0x39  // Idle mode ON

#define ILI9341_PIXFMT                 0x3A  // Pixel format set (16bit,18bit)

#define ILI9341_RAMWRC                 0x3C  // Memory write continue
#define ILI9341_RAMRDC                 0x3E  // Memory read continue

#define ILI9341_SETSCANLINE            0x44  // Set tear scanline
#define ILI9341_GETSCANLINE            0x45  // Get scanline

#define ILI9341_WRDISBV                0x51  // Write display brightness
#define ILI9341_RDDISBV                0x52  // Read display brightness

#define ILI9341_WRCTRLD                0x53  // Write control display
#define ILI9341_RDCTRLD                0x54  // Read control display

#define ILI9341_WRCABC                 0x55  // Write adaptive brightness control
#define ILI9341_RDCABC                 0x56  // Read adaptive brightness control

#define ILI9341_WRCABCMB               0x5E  // Write minimum brightness
#define ILI9341_RDCABCMB               0x5F  // Read minimum brightness

#define ILI9341_RDID1                  0xDA  // Read ID1
#define ILI9341_RDID2                  0xDB  // Read ID2
#define ILI9341_RDID3                  0xDC  // Read ID3

#define ILI9341_RGBCTRL                0xB0  // RGB interface signal control
#define ILI9341_FRMCTR1                0xB1  // Frame rate control (normal)
#define ILI9341_FRMCTR2                0xB2  // Frame rate control (idle)
#define ILI9341_FRMCTR3                0xB3  // Frame rate control (partial)
#define ILI9341_INVCTR                 0xB4  // Display inversion control
#define ILI9341_BLANKING               0xB5  // Blanking porch control
#define ILI9341_DISCTRL                0xB6  // Display function control
#define ILI9341_ENTRYMODE              0xB7  // Entry mode set

#define ILI9341_BACKLIGHT1             0xB8  // Backlight control 1
#define ILI9341_BACKLIGHT2             0xB9  // Backlight control 2
#define ILI9341_BACKLIGHT3             0xBA  // Backlight control 3
#define ILI9341_BACKLIGHT4             0xBB  // Backlight control 4
#define ILI9341_BACKLIGHT5             0xBC  // Backlight control 5
#define ILI9341_BACKLIGHT7             0xBE  // Backlight control 7
#define ILI9341_BACKLIGHT8             0xBF  // Backlight control 8

#define ILI9341_PWCTRL1                0xC0  // Power control 1
#define ILI9341_PWCTRL2                0xC1  // Power control 2

#define ILI9341_VMCTRL1                0xC5  // VCOM control 1
#define ILI9341_VMCTRL2                0xC7  // VCOM control 2

#define ILI9341_NVMWR                  0xD0  // NV memory write
#define ILI9341_NVMPKEY                0xD1  // NV memory protection key
#define ILI9341_RDNVM                  0xD2  // NV memory status read

#define ILI9341_RDID4                  0xD3  // Read ID4

#define ILI9341_GMCTRP1                0xE0  // Positive gamma correction
#define ILI9341_GMCTRN1                0xE1  // Negative gamma correction

#define ILI9341_DGAMCTRL1              0xE2  // Digital gamma control 1
#define ILI9341_DGAMCTRL2              0xE3  // Digital gamma control 2

#define ILI9341_IFCTRL                 0xF6  // Interface control

#define ILI9341_PWCTRLA                0xCB  // Power control A
#define ILI9341_PWCTRLB                0xCF  // Power control B

#define ILI9341_DTCTRLA                0xE8  // Driver timing control A
#define ILI9341_DTCTRLB                0xEA  // Driver timing control B
#define ILI9341_DTCTRLC                0xE9  // Driver timing control C

#define ILI9341_PWONSEQ                0xED  // Power on sequence control

#define ILI9341_ENABLE3G               0xF2  // Enable 3 gamma control
#define ILI9341_PUMPCTRL               0xF7  // Pump ratio control