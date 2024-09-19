add_library(interfaces INTERFACE
interfaces.hpp
inoutstream/inoutstream.hpp
software_timer/software_timer_interface.hpp
dio/dio_interface_types.hpp
dio/dio_interface.hpp
)

target_link_libraries(interfaces INTERFACE commons)

target_include_directories(interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})