cmake_minimum_required(VERSION 3.25)

project(main)

file(GLOB_RECURSE source_files src/*.c)
file(GLOB_RECURSE header_files src/*.h)

add_executable(main ${source_files} ${header_files})

target_compile_options(main PUBLIC -Wall -Werror -Wextra)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

target_link_libraries(main PRIVATE SDL3::SDL3)
target_link_libraries(main PRIVATE SDL3_image::SDL3_image)
target_link_libraries(main PRIVATE m)
