cmake_minimum_required(VERSION 3.20)

project(voxel_renderer)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(WORKGROUP_SIZE_X 16)
set(WORKGROUP_SIZE_Y 16)
set(RENDERER_SPV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/shader/renderer.glsl)
set(OUTPUT_SPV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/shader/output.glsl)

add_custom_command(
    OUTPUT renderer.spv
    COMMAND glslangValidator -V -S comp ${RENDERER_SPV_SRC} -o renderer.spv -DWORKGROUP_SIZE_X=${WORKGROUP_SIZE_X} -DWORKGROUP_SIZE_Y=${WORKGROUP_SIZE_Y}
    DEPENDS ${RENDERER_SPV_SRC}
    VERBATIM
)

add_custom_command(
    OUTPUT output.spv
    COMMAND glslangValidator -V -S comp ${OUTPUT_SPV_SRC} -o output.spv -DWORKGROUP_SIZE_X=${WORKGROUP_SIZE_X} -DWORKGROUP_SIZE_Y=${WORKGROUP_SIZE_Y}
    DEPENDS ${OUTPUT_SPV_SRC}
    VERBATIM
)

file(GLOB_RECURSE C_FILES src/*.c)
add_executable(voxel_renderer ${C_FILES} renderer.spv output.spv)

target_include_directories(voxel_renderer PRIVATE src include)
target_link_libraries(voxel_renderer PRIVATE m vulkan)
target_compile_options(
    voxel_renderer
    PRIVATE
    -g
    -Wall
    -Wextra
    -Werror
    -pedantic
    -Wno-unused-parameter
    -Wno-missing-braces
    -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=.
    -DWORKGROUP_SIZE_X=${WORKGROUP_SIZE_X}
    -DWORKGROUP_SIZE_Y=${WORKGROUP_SIZE_Y}
)