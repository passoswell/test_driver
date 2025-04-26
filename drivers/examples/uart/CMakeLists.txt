if((DEFINED BUILD_EX_UART) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_UART TRUE)
  message("Building uart examples and saving binary on ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(ex_uart_hello_world
  hello_world.cpp
  )
  target_link_libraries(ex_uart_hello_world drivers)
  _postbuild_task(ex_uart_hello_world)

  add_executable(ex_uart_echo_blocking
  echo_blocking.cpp
  )
  target_link_libraries(ex_uart_echo_blocking drivers)
  _postbuild_task(ex_uart_echo_blocking)

  add_executable(ex_uart_echo_non_blocking
  echo_non_blocking.cpp
  )
  target_link_libraries(ex_uart_echo_non_blocking drivers)
  _postbuild_task(ex_uart_echo_non_blocking)

endif()
