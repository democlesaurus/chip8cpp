cmake_minimum_required(VERSION 3.0.0)
project(chip8 VERSION 0.1.0)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include(CTest)
enable_testing()

add_executable(chip8 main.cpp chip8.cpp)

target_link_libraries(chip8 ${SDL2_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
