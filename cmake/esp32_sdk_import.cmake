

set(ESP32_VALID_TARGETS "esp32" "esp32s2" "esp32s3" "esp32c3" "esp32c2" "esp32c6" "esp32h2" "esp32p4")

IF (DEFINED ENV{IDF_TOOLS_PATH} AND (NOT IDF_TOOLS_PATH))
  set(IDF_TOOLS_PATH $ENV{IDF_TOOLS_PATH})
  message("Using IDF_TOOLS_PATH from environment : $ENV{IDF_TOOLS_PATH}")
ELSE()
  message("Environment variable IDF_TOOLS_PATH not defined")
ENDIF()

message("IDF_PATH = $ENV{IDF_PATH}")

IF (NOT IDF_TOOLS_PATH)

  include(FetchContent)
  set(FETCHCONTENT_BASE_DIR_SAVE ${FETCHCONTENT_BASE_DIR})
  if (ESP_IDF_FETCH_FROM_GIT_PATH)
      get_filename_component(FETCHCONTENT_BASE_DIR "${ESP_IDF_FETCH_FROM_GIT_PATH}" REALPATH BASE_DIR "${CMAKE_SOURCE_DIR}")
  endif ()
  set(ESP32_IDF_VERSION $ENV{ESP32_IDF_VERSION})
  message("\r\nIDF version ${ESP32_IDF_VERSION} was configured on preset file\r\n")
  FetchContent_Declare(
          esp_idf
          GIT_REPOSITORY https://github.com/espressif/esp-idf.git
          GIT_TAG ${ESP32_IDF_VERSION}
          GIT_SUBMODULES_RECURSE TRUE)
  message("\r\nSearching for ESP IDF on folder ${ESP_IDF_FETCH_FROM_GIT_PATH}\r\n")

  if(EXISTS ${ESP_IDF_FETCH_FROM_GIT_PATH})
      message("\r\nUsing ESP-IDF from ${ESP_IDF_FETCH_FROM_GIT_PATH}\r\n")
      FetchContent_Populate(esp_idf QUIET OFF)
      set(IDF_TOOLS_PATH ${esp_idf_SOURCE_DIR})
  else()
      message(AUTHOR_WARNING "IDF_TOOLS_PATH environment variable not found. If build fails, please refer to esp-idf documentation on how to setup correctly.")
      message("Configure process will try to clone esp-idf from the git repository on https://github.com/espressif/esp-idf.git.\r\n\r\n")
      message("\r\nDownloading ESP-IDF\r\n")
      FetchContent_Populate(esp_idf)
      set(IDF_TOOLS_PATH ${esp_idf_SOURCE_DIR})
      message("\r\nRunning ./install.sh all")
      execute_process (COMMAND ./install.sh all WORKING_DIRECTORY ${IDF_TOOLS_PATH}/ RESULT_VARIABLE result)
      message("result: ${result}\r\n")

      file(READ $ENV{HOME}/.bashrc bashrc_text)
      set(BASHRC_COMAMND ". ${IDF_TOOLS_PATH}/export.sh")
      string(FIND "${bashrc_text}" "${BASHRC_COMAMND}" result)

      if(result EQUAL -1)
        message("Adding '. ${IDF_TOOLS_PATH}/export.sh' to '~/.bashrc'")
        file(APPEND $ENV{HOME}/.bashrc "\n. ${IDF_TOOLS_PATH}/export.sh")
        file(APPEND $ENV{HOME}/.bashrc "\nclear")
      else()
        message("result = ${result}")
      endif()

      execute_process (COMMAND source ~/.bashrc RESULT_VARIABLE result)
      message("result: ${result}\r\n")

      IF(DEFINED ENV{IDF_PATH})
          message(AUTHOR_WARNING "IDF_PATH is defined in the environment path, even though we just downloaded and configured the SDK.")
      ELSE()
          message(FATAL_ERROR "Environment variable IDF_PATH not defined. Use 'source ~/.bashrc' or close the terminal, open and try again. In vscode, close and reopen the window.")
      ENDIF()
  endif ()
  set(FETCHCONTENT_BASE_DIR ${FETCHCONTENT_BASE_DIR_SAVE})

ENDIF()


get_filename_component(IDF_TOOLS_PATH "${IDF_TOOLS_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${IDF_TOOLS_PATH})
    message(FATAL_ERROR "Directory '${IDF_TOOLS_PATH}' not found")
endif ()
set(IDF_TOOLS_PATH ${IDF_TOOLS_PATH} CACHE PATH "Path to the ESP-IDF" FORCE)



IF(DEFINED ENV{IDF_PATH} AND (NOT IDF_PATH))
  set(IDF_PATH $ENV{IDF_PATH})
  message("\r\nUsing IDF_PATH from environment : ${IDF_PATH}\r\n")
ELSE()
  # message("Adding '. ${IDF_TOOLS_PATH}/export.sh' to '~/.bashrc'")
  # file(APPEND $ENV{HOME}/.bashrc "\r\n. ${IDF_TOOLS_PATH}/export.sh")
  message(FATAL_ERROR "Environment variable IDF_PATH not defined, close the terminal, open and try again.")
ENDIF()

message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
IF(NOT ESP32_TARGET)
  set(ESP32_TARGET "esp32s3")
  message("ESP32_TARGET was not defined, defaulting to ${ESP32_TARGET}")
ENDIF()
add_definitions(-DESP32_TARGET=${ESP32_TARGET})
add_definitions(-DIDF_TARGET=${ESP32_TARGET})
message("\r\nESP32_TARGET: ${ESP32_TARGET}\r\n")

set(TOOLCHAIN_FILE "${IDF_PATH}/tools/cmake/toolchain-${ESP32_TARGET}.cmake")
IF((CMAKE_TOOLCHAIN_FILE STREQUAL TOOLCHAIN_FILE) OR (NOT CMAKE_TOOLCHAIN_FILE))
  message("Target did not change since last configure phase")
ELSE()
  message(FATAL_ERROR "Target has changed since last configure phase, please delete the build folder ${CMAKE_BINARY_DIR}")
ENDIF()
add_definitions(-DCMAKE_TOOLCHAIN_FILE=${IDF_PATH}/tools/cmake/toolchain-${ESP32_TARGET}.cmake)
set(CMAKE_TOOLCHAIN_FILE "${IDF_PATH}/tools/cmake/toolchain-${ESP32_TARGET}.cmake")
message("\r\nCMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}\r\n")
