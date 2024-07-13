IF(DEFINED USE_PIPICO)

  message("Using PIPICO")
  # Pull in SDK (must be before project)
  include(cmake/pico_sdk_import.cmake)
  add_definitions(-DUSE_PIPICO=1)
  set(DUSE_PIPICO TRUE)

ELSEIF(DEFINED USE_LINUX)

  message("Using GCC_LINUX")
  add_definitions(-DUSE_LINUX=1)
  set(USE_LINUX TRUE)

ELSE()

  message(FATAL_ERROR "No platform was specified, aborting configuration.")

ENDIF()