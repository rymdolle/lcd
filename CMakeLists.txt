cmake_minimum_required(VERSION 3.27)

add_library(lcd INTERFACE)
target_sources(lcd INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lcd.cpp)

target_link_libraries(lcd INTERFACE
  pico_stdlib
)

target_include_directories(lcd INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}
)
