
add_library(drivers INTERFACE
drivers.hpp
)

target_link_libraries(drivers INTERFACE interfaces)

IF(DEFINED USE_LINUX)

  add_subdirectory(linux)
  target_link_libraries(drivers INTERFACE drv_linux)
  message("\r\nLinking linux drivers because USE_LINUX is defined\r\n")

ENDIF()

target_include_directories(drivers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(examples)