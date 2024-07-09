if((DEFINED BUILD_HELLO_WORLD) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  message("Building uart example hello_world and saving binary on build/bin/\r\n")

  add_executable(hello_world
  hello_world.cpp
  )

  # pull in common dependencies
  target_link_libraries(hello_world drivers)

  set_target_properties(hello_world
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME_OUTPUT_DIRECTORY})

endif()