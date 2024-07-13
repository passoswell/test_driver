# Run some postproject task
function (_postbuild_task TARGET)

    # Set the folder where the binaries should be generated
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR})
    set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR})

    if(DEFINED USE_PIPICO)

        if (NOT ELF2UF2_FOUND)
            find_package(pico-sdk-tools "${PICO_SDK_VERSION_MAJOR}.${PICO_SDK_VERSION_MINOR}.${PICO_SDK_VERSION_REVISION}" QUIET CONFIG CMAKE_FIND_ROOT_PATH_BOTH)
            if (NOT ELF2UF2_FOUND)
                set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PICO_SDK_PATH}/tools)
                find_package(ELF2UF2)
            endif()
        endif()
        if (ELF2UF2_FOUND)
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND ELF2UF2 $<TARGET_FILE:${TARGET}> ${BIN_DIR}/${TARGET}.uf2
                VERBATIM)
        endif()
    endif()
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
