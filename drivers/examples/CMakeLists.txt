if((DEFINED BUILD_GPIO_EXAMPLES) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))
  add_subdirectory(gpio)
endif()

add_subdirectory(uart)
