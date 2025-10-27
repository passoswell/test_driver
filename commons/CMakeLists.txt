add_library(commons INTERFACE
com_error_code.hpp
com_types.hpp
commons.hpp
)

add_subdirectory(examples)

target_include_directories(commons INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
