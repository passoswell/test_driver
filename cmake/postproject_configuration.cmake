# # Add targets to flash on esp32
# function(add_flashable_target target_name)
#     add_custom_target(flash_${target_name}
#         COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/flash.py ${target_name}
#         COMMENT "Flashing target ${target_name}"
#         DEPENDS ${target_name}  # Optional: build target before flashing
#     )
# endfunction()


# Run some postproject task
function (_postbuild_task A_TARGET)
    #get_target_property(FOO_NAME ${A_TARGET} NAME)
    # Set the folder where the binaries should be generated
    set_target_properties(${A_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set_target_properties(${A_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

    IF(DEFINED USE_ESP32)

        idf_build_executable(${A_TARGET})

        # Generate .bin file for every target
        add_custom_command(TARGET ${A_TARGET} POST_BUILD
            COMMAND python ${IDF_PATH}/components/esptool_py/esptool/esptool.py -c ${ESP32_TARGET} elf2image ${CMAKE_BINARY_DIR}/bin/${A_TARGET}
            VERBATIM
            )

        # Add flashable target
        add_custom_target(flash_${A_TARGET}
            # COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/flash.py ${A_TARGET}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMAND python ${IDF_PATH}/components/esptool_py/esptool/esptool.py -c ${ESP32_TARGET} write_flash --flash_mode dio --flash_freq 80m --flash_size 2MB 0x1000 bootloader/bootloader.bin 0x10000 bin/${A_TARGET}.bin 0x8000 partition_table/partition-table.bin
            COMMENT "Flashing target ${A_TARGET}"
            # DEPENDS ${A_TARGET}  # Optional: build target before flashing
            USES_TERMINAL
            VERBATIM
            )
        add_dependencies(flash_${A_TARGET} ${A_TARGET} bootloader)


    ELSEIF(DEFINED USE_PIPICO)

        pico_add_extra_outputs(${A_TARGET})
        add_custom_command(TARGET ${A_TARGET} POST_BUILD
            COMMAND mv "${A_TARGET}.uf2" "${CMAKE_BINARY_DIR}/bin"
            )

    ENDIF()
endfunction()


# Find all cmake targets and append to a list
function (_get_all_cmake_targets out_var current_dir)
    get_property(targets DIRECTORY ${current_dir} PROPERTY BUILDSYSTEM_TARGETS)
    get_property(subdirs DIRECTORY ${current_dir} PROPERTY SUBDIRECTORIES)

    foreach(subdir ${subdirs})
        _get_all_cmake_targets(subdir_targets ${subdir})
        list(APPEND targets ${subdir_targets})
    endforeach()

    set(${out_var} ${targets} PARENT_SCOPE)
endfunction()


# Find all pipico .elf executables generated in the last build in ${BIN_DIR} path
function (_get_generated_executables out_var)

    file(GLOB PIPICO_ELF_PATHS_LIST "${BIN_DIR}/*.elf")
    string(COMPARE NOTEQUAL "${PIPICO_ELF_PATHS_LIST}" "" result)
        IF(result EQUAL 1)
          # message("${result}, ${PIPICO_ELF_PATHS_LIST}")
          string(REPLACE "${BIN_DIR}/" "" PIPICO_ELFS_LIST ${PIPICO_ELF_PATHS_LIST})
          string(REPLACE ".elf" ";" PIPICO_TARGETS_LIST ${PIPICO_ELFS_LIST})
        ENDIF()
    #message("${PIPICO_TARGETS_LIST}")
    set(${out_var} ${PIPICO_TARGETS_LIST} PARENT_SCOPE)
endfunction()
