find_package(Threads REQUIRED)

add_library(drv_linux STATIC
utils/linux_types.hpp
utils/linux_io.hpp
utils/linux_io.cpp
std_in_out/drv_std_in_out.cpp
std_in_out/drv_std_in_out.hpp
uart/drv_uart.cpp
uart/drv_uart.hpp
i2c/drv_i2c.cpp
i2c/drv_i2c.hpp)

git_clone(
  PROJECT_NAME                    lg
  GIT_URL                         https://github.com/joan2937/lg.git
  GIT_BRANCH                      master
  DIRECTORY                       ${REPO_DIR}
)

set(lgpio_src_dir "${REPO_DIR}/lg")
message("Source dir of lgpio_repo = ${lgpio_src_dir}")
include_directories(${lgpio_src_dir})

file(GLOB helloworld_SRC CONFIGURE_DEPENDS "${lgpio_src_dir}/*.h" "${lgpio_src_dir}/*.c")
# message(" ${helloworld_SRC}")
add_library(lgpio_lib STATIC ${helloworld_SRC})

target_link_libraries(drv_linux drivers ${CMAKE_THREAD_LIBS_INIT} lgpio_lib)