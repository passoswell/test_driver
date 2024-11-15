add_library(devices STATIC
basic_graphics.c
drv_ssd1306.cpp
basic_graphics.h
gfxfont.h
glcdfont.h
dev_ssd1306_fonts.hpp
dev_ssd1306_types.hpp
dev_ssd1306.hpp
)

target_link_libraries(devices drivers)