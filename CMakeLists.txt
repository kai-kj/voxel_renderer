cmake_minimum_required(VERSION 3.20)
project(voxel_renderer)

add_executable(
        voxel_renderer
        src/main.c
        src/lib_impl.c
        src/world/scene.c
        src/world/camera.c
        src/world/material.c
        src/renderer/renderer.c
        src/renderer/shader_compiler.c
        src/logger/logger.c
        src/lua/lua_extra.c
)

target_include_directories(voxel_renderer PRIVATE src include)

target_compile_options(
        voxel_renderer PRIVATE
        -g -O0
        -Wall -Wextra -Werror
        -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function
)

# vulkan
find_package(Vulkan REQUIRED)
target_include_directories(voxel_renderer PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(voxel_renderer PRIVATE ${Vulkan_LIBRARIES})

# shaderc
message("-- checking shaderc dependencies")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(COMMAND ./utils/git-sync-deps WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/shaderc)

set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON)
add_subdirectory(lib/shaderc)

# lua
find_package(Lua REQUIRED VERSION 5.4)
target_include_directories(voxel_renderer PRIVATE ${LUA_INCLUDE_DIRS})
target_link_libraries(voxel_renderer PRIVATE ${LUA_LIBRARIES})

# microcompute
add_subdirectory(lib/microcompute)
target_link_libraries(voxel_renderer PRIVATE microcompute microcompute_extra shaderc)
