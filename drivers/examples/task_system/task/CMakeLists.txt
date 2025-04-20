if((DEFINED BUILD_EX_TASK) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_TASK TRUE)
  message("Building task examples and saving binary on  ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(ex_task
  ex_task.cpp
  )
  target_link_libraries(ex_task drivers)
  _postbuild_task(ex_task)

  add_executable(ex_task_multicore
  ex_task_multicore.cpp
  )
  target_link_libraries(ex_task_multicore drivers)
  _postbuild_task(ex_task_multicore)

endif()