# Run image converter
find_package(Python3 COMPONENTS Interpreter REQUIRED)
file(GLOB IMAGE_FILES ${CMAKE_SOURCE_DIR}/images/*)
# add_custom_command(
add_custom_target(
  RunAlways ALL
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/image_converter/png_to_header.py ${CMAKE_SOURCE_DIR}/images
  COMMENT "Generating header from images..."
)

add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/images)
add_custom_target(images DEPENDS ${CMAKE_SOURCE_DIR}/images/images.h)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/hub75.pio)

target_sources(${PROJECT_NAME} PRIVATE main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  pico_stdlib
  hardware_pio
)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)