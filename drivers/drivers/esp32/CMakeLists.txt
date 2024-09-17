add_library(drv_esp32 STATIC
uart/drv_uart.cpp
uart/drv_uart.hpp
gpio/drv_gpio.cpp
gpio/drv_gpio.hpp
spt/drv_spt.cpp
spt/drv_spt.hpp
)

IF("${TARGET}" IN_LIST ESP32_VALID_TARGETS)
    # Include for ESP-IDF build system functions
    include($ENV{IDF_PATH}/tools/cmake/idf.cmake)
    # Create idf::{target} and idf::freertos static libraries
    idf_build_process("${TARGET}"
                    # try and trim the build; additional components
                    # will be included as needed based on dependency tree
                    #
                    # although esptool_py does not generate static library,
                    # processing the component is needed for flashing related
                    # targets and file generation
                    COMPONENTS freertos esptool_py driver esp_timer
                    SDKCONFIG ${CMAKE_CURRENT_LIST_DIR}/sdkconfig
                    BUILD_DIR ${CMAKE_BINARY_DIR})
ELSE()
    message(FATAL_ERROR "Unknown target ${TARGET}")
ENDIF()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# pull in common dependencies
target_link_libraries(drv_esp32 drivers idf::freertos idf::spi_flash idf::driver idf::esp_timer)