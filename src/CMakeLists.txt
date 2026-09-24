# Specify the list of sources here:
set(SOURCES
    "display/ili9341/ili9341.c"
    "display/xpt2046/xpt2046.c"

    "ui/entry_point.c"

    "audio/entry_point.c"

    "main.c"
)

add_executable(BitBeat ${SOURCES})

target_include_directories(BitBeat PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(BitBeat
    pico_stdlib
    pico_multicore
    
    hardware_spi
    hardware_dma
    hardware_pwm

    lvgl
)

target_compile_definitions(BitBeat PUBLIC "$<$<CONFIG:Debug>:_DEBUG>")

target_include_directories(BitBeat PUBLIC
    "${CMAKE_SOURCE_DIR}/libs/lvgl/src"
)

pico_set_program_name(BitBeat "BitBeat")
pico_set_program_version(BitBeat "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(BitBeat 0)
pico_enable_stdio_usb(BitBeat 1)

pico_add_extra_outputs(BitBeat)