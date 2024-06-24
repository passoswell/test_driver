add_library(drv_linux STATIC
std_in_out/drv_std_in_out.cpp
std_in_out/drv_std_in_out.hpp
uart/drv_uart.cpp
uart/drv_uart.hpp)

include(ExternalProject)
# Define the directory where you want to clone the repository
set(REPO_DIR "${CMAKE_SOURCE_DIR}/external/")
# Use ExternalProject_Add to clone the Git repository
ExternalProject_Add(
    lgpio_repo
    GIT_REPOSITORY https://github.com/joan2937/lg.git
    # GIT_TAG master
    PREFIX ${REPO_DIR}
    CONFIGURE_COMMAND "" # Skip the configure step
    BUILD_COMMAND ""
    # BUILD_IN_SOURCE TRUE
    INSTALL_COMMAND ""   # Skip the install step
    UPDATE_DISCONNECTED 1 # Avoid updating the repo every time CMake runs
)
ExternalProject_Get_property(lgpio_repo SOURCE_DIR)
message("Source dir of lgpio_repo = ${SOURCE_DIR}")
include_directories(${SOURCE_DIR})

file(GLOB helloworld_SRC CONFIGURE_DEPENDS "${SOURCE_DIR}/*.h" "${SOURCE_DIR}/*.c")
# message(" ${helloworld_SRC}")
add_library(lgpio_lib STATIC ${helloworld_SRC})

target_link_libraries(drv_linux drivers lgpio_lib)