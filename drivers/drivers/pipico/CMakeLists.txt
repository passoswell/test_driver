# Initialize the SDK
pico_sdk_init()

add_library(drv_pipico STATIC
uart/drv_uart.cpp
uart/drv_uart.hpp)

# pull in common dependencies
target_link_libraries(drv_pipico drivers pico_stdlib)