if((DEFINED BUILD_EX_BLINK_READ_BLINK) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_BLINK_READ_BLINK TRUE)
  message("Building gpio example blink_read_blink and saving binary on ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(blink_read_blink
  blink_read_blink.cpp
  )

  # pull in common dependencies
  target_link_libraries(blink_read_blink drivers)


  _postbuild_task(blink_read_blink)

endif()