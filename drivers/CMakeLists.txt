
add_library(drivers STATIC
drivers.hpp
drivers/base/drv_comm_base.hpp
drivers/base/drv_comm_base.cpp)

target_link_libraries(drivers interfaces)

if(DEFINED USE_LINUX)
  add_subdirectory(drivers/linux)
  target_link_libraries(drivers drv_linux)
  message("\r\nLinking linux drivers because USE_LINUX is defined\r\n")
endif()

target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})