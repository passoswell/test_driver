# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/user/esp-idf/esp_idf-src/components/bootloader/subproject"
  "/home/user/vscode/projects/test_driver/build/esp32/bootloader"
  "/home/user/vscode/projects/test_driver/build/esp32/drivers/esp32/bootloader-prefix"
  "/home/user/vscode/projects/test_driver/build/esp32/drivers/esp32/bootloader-prefix/tmp"
  "/home/user/vscode/projects/test_driver/build/esp32/drivers/esp32/bootloader-prefix/src/bootloader-stamp"
  "/home/user/vscode/projects/test_driver/build/esp32/drivers/esp32/bootloader-prefix/src"
  "/home/user/vscode/projects/test_driver/build/esp32/drivers/esp32/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/user/vscode/projects/test_driver/build/esp32/drivers/esp32/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/user/vscode/projects/test_driver/build/esp32/drivers/esp32/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
