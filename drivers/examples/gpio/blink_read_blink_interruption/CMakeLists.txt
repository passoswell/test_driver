message("Building gpio example blink_read_blink_interruption and saving binary on ${CMAKE_BINARY_DIR}/bin/\r\n")

add_executable(blink_read_blink_interruption
blink_read_blink_interruption.cpp
)

# pull in common dependencies
target_link_libraries(blink_read_blink_interruption drivers)


_postbuild_task(blink_read_blink_interruption)