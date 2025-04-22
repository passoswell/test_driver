cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

include(cmake/preproject_configuration.cmake)
include(cmake/postproject_configuration.cmake)

project(TEST_DRIVER VERSION 0.1.0 LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE})
# set(CMAKE_CXX_FLAGS_DEBUG "-Og") # Minimal optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast") # Optimize for speed on release configuration

# Define the directory where external repositories should be cloned
set(REPO_DIR "${CMAKE_SOURCE_DIR}/external")

include(cmake/GitUtils.cmake)

add_subdirectory(commons)
add_subdirectory(interfaces)
add_subdirectory(drivers)

add_executable(TEST_DRIVER TEST_DRIVER.cpp)
target_link_libraries(TEST_DRIVER drivers)
_postbuild_task(TEST_DRIVER)

include(CTest)
enable_testing()
