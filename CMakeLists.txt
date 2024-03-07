cmake_minimum_required(VERSION 3.20)

project(voxel_renderer)

file(GLOB_RECURSE C_FILES src/*.c)

add_executable(voxel_renderer ${C_FILES})
target_include_directories(voxel_renderer PRIVATE src include)
target_link_libraries(voxel_renderer PRIVATE m vulkan SPIRV-Tools stdc++ lua5.3)

target_compile_options(
    voxel_renderer PRIVATE
    -std=c11
    -Wall -Wextra -Werror
    -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function
    -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.
)