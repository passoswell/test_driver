# Initialize the SDK
pico_sdk_init()

add_library(drv_pipico STATIC
uart/drv_uart.cpp
uart/drv_uart.hpp
dio/drv_dio.cpp
dio/drv_dio.hpp
spt/drv_spt.cpp
spt/drv_spt.hpp
)

# pull in common dependencies
target_link_libraries(drv_pipico drivers pico_stdlib)