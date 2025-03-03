find_package(Threads REQUIRED)

add_library(drv_linux STATIC
utils/linux_types.hpp
utils/linux_io.hpp
utils/linux_io.cpp
utils/linux_threads.hpp
utils/linux_threads.tpp
utils/linux_queue.hpp
utils/linux_serial_file.hpp
utils/linux_serial_file.cpp

dio/dio.cpp
dio/dio.hpp
iic/iic.cpp
iic/iic.hpp
iic/iic_types.hpp
spi/spi.cpp
spi/spi.hpp
spt/drv_spt.cpp
spt/drv_spt.hpp
std_in_out/std_in_out.cpp
std_in_out/std_in_out.hpp
uart/uart.cpp
uart/uart.hpp
)

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

target_link_libraries(drv_linux interfaces drivers ${CMAKE_THREAD_LIBS_INIT} gpiod)