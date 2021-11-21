cmake_minimum_required(VERSION 3.16)
project(arista C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXE_LINKER_FLAGS "-static")

find_package(PkgConfig REQUIRED)
pkg_check_modules(libtirpc REQUIRED libtirpc)
include_directories(${libtirpc_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${libtirpc_LIBRARIES} )

