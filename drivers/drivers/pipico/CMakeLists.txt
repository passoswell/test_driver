# Initialize the SDK
pico_sdk_init()

add_library(drv_pipico STATIC
uart/drv_uart.cpp
uart/drv_uart.hpp
gpio/drv_gpio.cpp
gpio/drv_gpio.hpp
spt/drv_spt.cpp
spt/drv_spt.hpp
)

# pull in common dependencies
target_link_libraries(drv_pipico drivers pico_stdlib)