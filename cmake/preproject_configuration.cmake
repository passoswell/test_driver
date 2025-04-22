

set(FETCHCONTENT_QUIET OFF)

IF(DEFINED USE_ESP32)

  message("Using ESP32")
  include(cmake/esp32_sdk_import.cmake)
  add_definitions(-DUSE_ESP32=1)
  set(USE_ESP32 $ENV{USE_ESP32})
  set(ESP_PLATFORM $ENV{ESP_PLATFORM})

ELSEIF(DEFINED USE_PIPICO)

  message("Using PIPICO")
  # Pull in SDK (must be before project)
  include(cmake/pico_sdk_import.cmake)
  include(cmake/pico_extras_import_optional.cmake)
  add_definitions(-DUSE_PIPICO=1)
  set(DUSE_PIPICO TRUE)

ELSEIF(DEFINED USE_LINUX)

  message("Using GCC_LINUX")
  add_definitions(-DUSE_LINUX=1)
  set(USE_LINUX TRUE)

ELSE()

  message(FATAL_ERROR "No platform was specified, aborting configuration.")

ENDIF()