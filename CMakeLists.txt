cmake_minimum_required(VERSION 3.20)

include(cmake/preproject_configuration.cmake)
include(cmake/postproject_configuration.cmake)

project(TEST_DRIVER VERSION 0.1.0 LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Define the directory where external repositories should be cloned
set(REPO_DIR "${CMAKE_SOURCE_DIR}/external")

#define the directory where the binaries should be created
set(BIN_DIR "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${BIN_DIR}")

include(cmake/GitUtils.cmake)

add_subdirectory(commons)
add_subdirectory(interfaces)
add_subdirectory(drivers)

add_executable(TEST_DRIVER TEST_DRIVER.cpp)
target_link_libraries(TEST_DRIVER drivers)
_postbuild_task(TEST_DRIVER)

include(CTest)
enable_testing()
