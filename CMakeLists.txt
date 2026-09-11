cmake_minimum_required(VERSION 3.10)
project(RayTracer)
set(CMAKE_CXX_STANDARD 17)

# Disable GLFW extras
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add GLFW from third_party
add_subdirectory(third_party/glfw)

# Find OpenGL for display
find_package(OpenGL REQUIRED)

file(GLOB SOURCES "src/*.cpp")
add_executable(RayTracer ${SOURCES})

# Link OpenGL and GLFW to RayTracer
target_link_libraries(RayTracer OpenGL::GL glfw)
