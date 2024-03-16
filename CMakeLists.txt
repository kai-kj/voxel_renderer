cmake_minimum_required(VERSION 3.20)
project(voxel_renderer)

add_executable(voxel_renderer)

# vulkan
find_package(Vulkan REQUIRED)
target_include_directories(voxel_renderer PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(voxel_renderer PRIVATE ${Vulkan_LIBRARIES})

# spirv-tools
find_package(SPIRV-Tools REQUIRED)
target_include_directories(voxel_renderer PRIVATE ${SPIRV-Tools_INCLUDE_DIRS})
target_link_libraries(voxel_renderer PRIVATE ${SPIRV-Tools_LIBRARIES} SPIRV-Tools stdc++)

# lua
find_package(Lua REQUIRED VERSION 5.4)
target_include_directories(voxel_renderer PRIVATE ${LUA_INCLUDE_DIRS})
target_link_libraries(voxel_renderer PRIVATE ${LUA_LIBRARIES})

# local includes
target_include_directories(voxel_renderer PRIVATE src include)

target_sources(
        voxel_renderer PRIVATE
        src/main.c
        src/lib_impl.c
        src/world/scene.c
        src/world/camera.c
        src/world/material.c
        src/renderer/renderer.c
        src/renderer/shader_code.c
        src/logger/logger.c
        src/lua/lua_extra.c
)

target_compile_options(
        voxel_renderer PRIVATE
        -Wall -Wextra -Werror
        -Wno-unused-parameter -Wno-missing-braces -Wno-unused-function
        -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=. # so __FILE__ is relative to the project root
)