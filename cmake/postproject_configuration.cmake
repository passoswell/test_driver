# Run some postproject task
function (_postbuild_task A_TARGET)
    #get_target_property(FOO_NAME ${A_TARGET} NAME)
    # Set the folder where the binaries should be generated
    set_target_properties(${A_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
    set_target_properties(${A_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
endfunction()
