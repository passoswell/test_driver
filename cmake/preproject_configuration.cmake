

IF(DEFINED USE_LINUX)

  message("Using GCC_LINUX")
  add_definitions(-DUSE_LINUX=1)
  set(USE_LINUX TRUE)

ELSE()

  message(FATAL_ERROR "No platform was specified, aborting configuration.")

ENDIF()