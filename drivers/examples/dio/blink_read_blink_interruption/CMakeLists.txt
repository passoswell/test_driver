if((DEFINED BUILD_EX_BLINK_READ_BLINK_INTERRUPTION) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_BLINK_READ_BLINK_INTERRUPTION TRUE)
  message("Building gpio example blink_read_blink_interruption and saving binary on ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(ex_blink_read_blink_interruption
  blink_read_blink_interruption.cpp
  )

  # pull in common dependencies
  target_link_libraries(ex_blink_read_blink_interruption drivers)


  _postbuild_task(ex_blink_read_blink_interruption)

endif()