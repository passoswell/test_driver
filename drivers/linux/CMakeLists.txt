find_package(Threads REQUIRED)

add_library(drv_linux STATIC
utils/linux_types.hpp
utils/linux_io.hpp
utils/linux_io.cpp
utils/linux_queue.hpp
utils/linux_serial_file.hpp
utils/linux_serial_file.cpp

dio/dio.cpp
dio/dio.hpp
iic/iic.cpp
iic/iic.hpp
iic/iic_types.hpp
spi/spi.cpp
spi/spi.hpp
spt/spt.cpp
spt/spt.hpp
std_in_out/std_in_out.cpp
std_in_out/std_in_out.hpp
uart/uart.cpp
uart/uart.hpp

task_system/stl_queue.hpp
task_system/stl_queue.tpp
task_system/stl_worker_task.hpp
task_system/stl_worker_task.tpp
task_system/task_system.hpp
task_system/stl_mutex.hpp
task_system/stl_atomic_mutex.hpp
task_system/stl_critical_mutex.hpp
)

target_link_libraries(drv_linux interfaces drivers ${CMAKE_THREAD_LIBS_INIT} gpiod)