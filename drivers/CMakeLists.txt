
add_library(drivers STATIC
drivers.hpp
drivers/base/drv_comm_base.hpp
drivers/base/drv_comm_base.cpp)

target_link_libraries(drivers interfaces)

IF(DEFINED USE_ESP32)

  add_subdirectory(drivers/esp32)
  target_link_libraries(drivers drv_esp32)
  message("\r\nLinking esp32 drivers because USE_ESP32 is defined\r\n")

ELSEIF(DEFINED USE_PIPICO)

  add_subdirectory(drivers/pipico)
  target_link_libraries(drivers drv_pipico)
  message("\r\nLinking pipico drivers because USE_PIPICO is defined\r\n")

ELSEIF(DEFINED USE_LINUX)

  add_subdirectory(drivers/linux)
  target_link_libraries(drivers drv_linux)
  message("\r\nLinking linux drivers because USE_LINUX is defined\r\n")

ENDIF()

target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(examples)