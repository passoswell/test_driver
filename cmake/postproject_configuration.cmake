# Run some postproject task
function (_postbuild_task A_TARGET)
    #get_target_property(FOO_NAME ${A_TARGET} NAME)
    # Set the folder where the binaries should be generated
    set_target_properties(${A_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set_target_properties(${A_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

    IF(DEFINED USE_ESP32)

        idf_build_executable(${A_TARGET})
        add_custom_command(TARGET ${A_TARGET} POST_BUILD
            COMMAND cp ${CMAKE_BINARY_DIR}/bin/${A_TARGET} ${CMAKE_BINARY_DIR}/bin/${A_TARGET}.elf
            VERBATIM)

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
