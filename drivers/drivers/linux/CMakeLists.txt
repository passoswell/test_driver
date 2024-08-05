find_package(Threads REQUIRED)

add_library(drv_linux STATIC
utils/linux_types.hpp
utils/linux_io.hpp
utils/linux_io.cpp
std_in_out/drv_std_in_out.cpp
std_in_out/drv_std_in_out.hpp
uart/drv_uart.cpp
uart/drv_uart.hpp
i2c/drv_i2c.cpp
i2c/drv_i2c.hpp
gpio/drv_gpio.cpp
gpio/drv_gpio.hpp)

include(FetchContent)

message("Downloading lgpio")
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external/lg")
FetchContent_Declare(
    lg
    GIT_REPOSITORY https://github.com/joan2937/lg.git
    GIT_TAG master
    GIT_SUBMODULES_RECURSE FALSE)
FetchContent_Populate(lg)
include_directories(${lg_SOURCE_DIR})
message("lgpio downloaded into ${lg_SOURCE_DIR}")
file(GLOB lg_SRC CONFIGURE_DEPENDS "${lg_SOURCE_DIR}/*.h" "${lg_SOURCE_DIR}/*.c")
add_library(lgpio_lib STATIC ${lg_SRC})

target_link_libraries(drv_linux drivers ${CMAKE_THREAD_LIBS_INIT} gpiod)