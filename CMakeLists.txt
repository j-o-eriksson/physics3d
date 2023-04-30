cmake_minimum_required(VERSION 3.15)

# set the project name
project(Blackhole)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# glad dependency
add_subdirectory(ext/glad)

# glfw dependency
add_subdirectory(ext/glfw)

# ------------------------------- #
# physics library 
add_library(physicslib
    ../phycpp/particle.cpp
    )

target_include_directories(physicslib PUBLIC ../ext/glm)

# ------------------------------- #
# rendering library 
add_library(graphicslib
    src/graphics/shader/shader.cpp
    src/graphics/shader/phong_shader.cpp
    src/graphics/renderer/renderer.cpp
    src/graphics/gl_utilities/cube_mesh.cpp
    )

target_link_libraries(graphicslib PUBLIC glad)
target_include_directories(graphicslib PUBLIC ../ext/glm src/graphics)

# ------------------------------- #
# executable
add_executable(Phy3d
    src/main.cpp
    )

target_include_directories(Phy3d PUBLIC ../ext/glm)
target_link_libraries(Phy3d PUBLIC glfw physics graphicslib)