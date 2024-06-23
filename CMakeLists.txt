cmake_minimum_required(VERSION 3.0.0)
project(TEST_DRIVER VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)

message("\r\nAdding linux drivers by adding -DUSE_LINUX definition on the main CMakeLists.txt file as a temporary action\r\n")
add_definitions(-DUSE_LINUX=1) #temporary solution
set(USE_LINUX TRUE)

add_subdirectory(commons)
add_subdirectory(interfaces)
add_subdirectory(drivers)

add_executable(TEST_DRIVER TEST_DRIVER.cpp)
# target_link_libraries(TEST_DRIVER drv_base)
target_link_libraries(TEST_DRIVER drivers)

include(CTest)
enable_testing()
