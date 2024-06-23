add_library(commons INTERFACE
com_status.hpp
commons.hpp)

target_include_directories(commons INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
