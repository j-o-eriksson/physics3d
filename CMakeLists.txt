cmake_minimum_required(VERSION 3.15)

# set the project name
project(JPhysics3d)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# ---- external dependencies ---- #
# glad dependency
add_subdirectory(ext/glad)

# glfw dependency
add_subdirectory(ext/glfw)

# physics dependency 
add_subdirectory(ext/physics_playground)


# ---- internal modules ---- #
# rendering module 
add_library(graphicslib
    src/graphics/shader/shader.cpp
    src/graphics/shader/phong_shader.cpp
    src/graphics/renderer/renderer.cpp
    src/graphics/utils/cube_mesh.cpp
    )

target_link_libraries(graphicslib PUBLIC glad)
target_include_directories(graphicslib PUBLIC ext/glm src/graphics)

# ------------------------------- #
# executable
add_executable(Phy3d
    src/main.cpp
    )

target_link_libraries(Phy3d PUBLIC glad glfw graphicslib jphys)
target_include_directories(Phy3d PUBLIC ext/glm)
