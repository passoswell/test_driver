message("Building error_code examples and saving binary on ${CMAKE_BINARY_DIR}/bin/\r\n")

add_executable(ex_error_category_creating_example
  creating_error_category.cpp
)
target_link_libraries(ex_error_category_creating_example drivers)
_postbuild_task(ex_error_category_creating_example)

add_executable(ex_error_codes_example
  using_error_codes.cpp
)
target_link_libraries(ex_error_codes_example drivers)
_postbuild_task(ex_error_codes_example)