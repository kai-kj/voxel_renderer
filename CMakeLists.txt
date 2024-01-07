cmake_minimum_required(VERSION 3.20)

project(voxel_world)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RENDERER_SPV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/shader/renderer.glsl)
add_custom_command(
    OUTPUT renderer.spv
    COMMAND glslangValidator -V -S comp ${RENDERER_SPV_SRC} -o renderer.spv
    DEPENDS ${RENDERER_SPV_SRC}
    VERBATIM
)

file(GLOB_RECURSE C_FILES src/*.c)
add_executable(voxel_world ${CPP_FILES} ${C_FILES} renderer.spv)

target_include_directories(voxel_world PRIVATE src include)
target_link_libraries(voxel_world PRIVATE vulkan)
target_compile_options(
    voxel_world
    PRIVATE
    -g
    -Wall
    -Wextra
    -pedantic
    -Wno-unused-parameter
    -Wno-missing-braces
    -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.
)