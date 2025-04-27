if((DEFINED BUILD_EX_IIC) OR ((DEFINED BUILD_ALL_EXAMPLES) AND (BUILD_ALL_EXAMPLES EQUAL 1)))

  set(BUILD_EX_IIC TRUE)
  message("Building iic examples and saving binary on ${CMAKE_BINARY_DIR}/bin/\r\n")

  add_executable(ex_iic_write_read_eeprom
    write_read_eeprom.cpp
  )
  target_link_libraries(ex_iic_write_read_eeprom drivers)
  _postbuild_task(ex_iic_write_read_eeprom)

endif()
