add_library(interfaces STATIC
interfaces.hpp
inoutstream/inoutstream_interface_types.hpp
inoutstream/inoutstream_interface.hpp
software_timer_interface/software_timer_interface.hpp
task_interface/task_interface.hpp

driver_base/driver_base.hpp
driver_base/driver_base.cpp
driver_base/driver_in_base.hpp
driver_base/driver_in_base.cpp
driver_base/driver_out_base.hpp
driver_base/driver_out_base.cpp

peripherals_base/dio_base.hpp
peripherals_base/iic_base.hpp
peripherals_base/spi_base.hpp
peripherals_base/spt_base.hpp
peripherals_base/spt_base.cpp
peripherals_base/uart_base.hpp
)

target_link_libraries(interfaces PUBLIC commons)

target_include_directories(interfaces PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})