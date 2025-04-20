if((DEFINED BUILD_EX_QUEUE) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_TASK TRUE)
  message("Building queue examples and saving binary on  ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(ex_queue
  ex_queue.cpp
  )
  target_link_libraries(ex_queue drivers)
  _postbuild_task(ex_queue)

endif()