

set(ESP32_VALID_TARGETS "esp32" "esp32s2" "esp32s3" "esp32c3" "esp32c2" "esp32c6" "esp32h2" "esp32p4")

IF (DEFINED ENV{IDF_TOOLS_PATH} AND (NOT IDF_TOOLS_PATH))
  set(IDF_TOOLS_PATH $ENV{IDF_TOOLS_PATH})
  message("Using IDF_TOOLS_PATH from environment : $ENV{IDF_TOOLS_PATH}")
ELSE()
  #message(FATAL_ERROR "Enviroment variable IDF_TOOLS_PATH not defined")
ENDIF()

IF (NOT IDF_TOOLS_PATH)

  include(FetchContent)
  set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
  if (ESP_IDF_FETCH_FROM_GIT_PATH)
      get_filename_component(FETCHCONTENT_BASE_DIR "${ESP_IDF_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
  endif ()
  FetchContent_Declare(
          esp_idf
          GIT_REPOSITORY https://github.com/espressif/esp-idf.git
          GIT_TAG v5.2.2
          GIT_SUBMODULES_RECURSE TRUE)

  if (NOT esp_idf)
      message(WARNING "IDF_TOOLS_PATH environment variable not found. If build fails, please refer to esp-idf documentation on how to setup correctly.")
      message("Configure process will try to clone esp-idf from the git repository on https://github.com/espressif/esp-idf.git.\r\n\r\n")
      message("Downloading ESP-IDF")
      FetchContent_Populate(esp_idf)
      set(IDF_TOOLS_PATH ${esp_idf_SOURCE_DIR})
      message("\r\nRunning ./install.sh all")
      execute_process (COMMAND ./install.sh all WORKING_DIRECTORY ${IDF_TOOLS_PATH}/ RESULT_VARIABLE result)
      message("result: ${result}\r\n")
      # Define PICO_SDK_PATH in ~/.bashrc
      message("Adding '. ${IDF_TOOLS_PATH}/export.sh' to '~/.bashrc'")
      # execute_process (COMMAND "echo \". ${IDF_TOOLS_PATH}/export.sh\" >> ~/.bashrc" ${IDF_TOOLS_PATH}/ RESULT_VARIABLE result)
      file(APPEND $ENV{HOME}/.bashrc ". ${IDF_TOOLS_PATH}/export.sh")
  endif ()
  set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})

ENDIF()


get_filename_component(IDF_TOOLS_PATH "${IDF_TOOLS_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${IDF_TOOLS_PATH})
    message(FATAL_ERROR "Directory '${IDF_TOOLS_PATH}' not found")
endif ()
set(IDF_TOOLS_PATH ${IDF_TOOLS_PATH} CACHE PATH "Path to the ESP-IDF" FORCE)

# message("\r\nAdding installed tools to the PATH environment variable, running   . export.sh ${IDF_TOOLS_PATH}")
# execute_process (COMMAND . export.sh WORKING_DIRECTORY ${IDF_TOOLS_PATH}/ RESULT_VARIABLE result)
# message("result: ${result}\r\n")

IF(DEFINED ENV{IDF_PATH} AND (NOT IDF_PATH))
  set(IDF_PATH $ENV{IDF_PATH})
  message("Using IDF_PATH from environment : ${IDF_PATH}")
ELSE()
  message(FATAL_ERROR "Enviroment variable IDF_PATH not defined")
ENDIF()

# message(FATAL_ERROR "Intentional stop")

add_definitions(-DTARGET=esp32s3)
set(TARGET "esp32s3")
message("TARGET: ${TARGET}")
add_definitions(-DCMAKE_TOOLCHAIN_FILE=${IDF_PATH}/tools/cmake/toolchain-${TARGET}.cmake)
set(CMAKE_TOOLCHAIN_FILE "${IDF_PATH}/tools/cmake/toolchain-${TARGET}.cmake")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
