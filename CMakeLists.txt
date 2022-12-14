cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(PicoBoi C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(PicoBoi)

pico_enable_stdio_usb(PicoBoi 1)
pico_enable_stdio_uart(PicoBoi 0)
pico_generate_pio_header(PicoBoi ${CMAKE_CURRENT_LIST_DIR}/TFT/SPIPIO.pio)

add_compile_options(-Ofast)

# must match with executable name and source file names
target_sources(PicoBoi PRIVATE PicoBoi.c TFT/TFTMaster.c TFT/glcdfont.c  menu.c snake.c GPS.c music.c )

# Add pico_multicore which is required for multicore functionality
target_link_libraries(PicoBoi pico_stdlib pico_multicore pico_bootsel_via_double_reset hardware_pwm hardware_dma hardware_irq hardware_adc hardware_pio hardware_i2c hardware_uart hardware_regs)

# create map/bin/hex file etc.
pico_add_extra_outputs(PicoBoi)

add_compile_options(-Wall
        -Wno-format # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)
