add_library(drv_linux STATIC
std_in_out/drv_std_in_out.cpp
std_in_out/drv_std_in_out.hpp)

target_link_libraries(drv_linux drivers)