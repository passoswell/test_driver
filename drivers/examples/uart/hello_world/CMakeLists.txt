if((DEFINED BUILD_EX_HELLO_WORLD) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_HELLO_WORLD TRUE)
  message("Building uart example hello_world and saving binary on  ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(hello_world
  hello_world.cpp
  )

  # pull in common dependencies
  target_link_libraries(hello_world drivers)


  _postbuild_task(hello_world)

endif()