#pragma once

// The project's config as a whole --------------------------

// Clocks (in kHz)
#define SYS_CLOCK          266000  // Overclocked to 266 Mhz :P
#define DISPLAY_SPI_CLOCK  120000  // 120 MHz

// Sleep on startup, useful to connect to usb serial monitor before actual code starts executing.
#define SLEEP_ON_STARTUP 0

// Display config: 
#define DISPLAY_HOR_RES 240
#define DISPLAY_VER_RES 320

#define DISPLAY_CS      9
#define DISPLAY_RESET   15
#define DISPLAY_DC      8
#define DISPLAY_MOSI    11
#define DISPLAY_MISO    12
#define DISPLAY_SCK     10
#define DISPLAY_LED     13

