add_library(interfaces INTERFACE
interfaces.hpp
inoutstream/inoutstream.hpp)

target_link_libraries(interfaces INTERFACE commons)

target_include_directories(interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})