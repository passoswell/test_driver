# Initialize the SDK
pico_sdk_init()

add_library(drv_pipico STATIC
dio/dio.cpp
dio/dio.hpp
spt/spt.cpp
spt/spt.hpp
)

# pull in common dependencies
target_link_libraries(drv_pipico drivers pico_stdlib)