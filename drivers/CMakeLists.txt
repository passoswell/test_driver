
add_library(drivers INTERFACE
drivers.hpp
)

target_link_libraries(drivers INTERFACE interfaces)

IF(DEFINED USE_ESP32)

  add_subdirectory(esp32)
  target_link_libraries(drivers INTERFACE drv_esp32)
  message("\r\nLinking esp32 drivers because USE_ESP32 is defined\r\n")

ELSEIF(DEFINED USE_PIPICO)

  add_subdirectory(pipico)
  target_link_libraries(drivers drv_pipico)
  message("\r\nLinking pipico drivers because USE_PIPICO is defined\r\n")

ELSEIF(DEFINED USE_LINUX)

  add_subdirectory(linux)
  target_link_libraries(drivers INTERFACE drv_linux)
  message("\r\nLinking linux drivers because USE_LINUX is defined\r\n")

ENDIF()

target_include_directories(drivers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(examples)