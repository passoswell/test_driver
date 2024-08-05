message("Building gpio example blink_read_blink and saving binary on ${CMAKE_BINARY_DIR}/bin/\r\n")

add_executable(blink_read_blink
blink_read_blink.cpp
)

# pull in common dependencies
target_link_libraries(blink_read_blink drivers)


_postbuild_task(blink_read_blink)