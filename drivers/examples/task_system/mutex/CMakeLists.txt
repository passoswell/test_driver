if((DEFINED BUILD_EX_MUTEX) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_MUTEX TRUE)
  message("Building mutex examples and saving binary on  ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(ex_mutex
  ex_mutex.cpp
  )
  target_link_libraries(ex_mutex drivers)
  _postbuild_task(ex_mutex)

  add_executable(ex_atomic_mutex
  ex_atomic_mutex.cpp
  )
  target_link_libraries(ex_atomic_mutex drivers)
  _postbuild_task(ex_atomic_mutex)

  add_executable(ex_critical_mutex
  ex_critical_mutex.cpp
  )
  target_link_libraries(ex_critical_mutex drivers)
  _postbuild_task(ex_critical_mutex)

endif()