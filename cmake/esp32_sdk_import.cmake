

set(ESP32_VALID_TARGETS "esp32" "esp32s2" "esp32s3" "esp32c3" "esp32c2" "esp32c6" "esp32h2" "esp32p4")

IF (DEFINED ENV{IDF_TOOLS_PATH} AND (NOT IDF_TOOLS_PATH))
  set(IDF_TOOLS_PATH $ENV{IDF_TOOLS_PATH})
  message("Using IDF_TOOLS_PATH from environment : $ENV{IDF_TOOLS_PATH}")
ELSE()
  message(FATAL_ERROR "Enviroment variable IDF_TOOLS_PATH not defined")
ENDIF()

IF(DEFINED ENV{IDF_PATH} AND (NOT IDF_PATH))
  set(IDF_PATH $ENV{IDF_PATH})
  message("Using IDF_PATH from environment : ${IDF_PATH}")
  #add_definitions(-PYTHON=${IDF_PATH}/python_env/idf5.2_py3.10_env/bin/python)
  #include(${IDF_PATH}/tools/cmake/project.cmake)
ELSE()
  message(FATAL_ERROR "Enviroment variable IDF_PATH not defined")
ENDIF()

add_definitions(-DTARGET=esp32s3)
set(TARGET "esp32s3")
message("TARGET: ${TARGET}")
add_definitions(-DCMAKE_TOOLCHAIN_FILE=${IDF_PATH}/tools/cmake/toolchain-${TARGET}.cmake)
set(CMAKE_TOOLCHAIN_FILE "${IDF_PATH}/tools/cmake/toolchain-${TARGET}.cmake")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
