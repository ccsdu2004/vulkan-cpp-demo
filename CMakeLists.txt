cmake_minimum_required(VERSION 3.5)

project(vulkan-demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/module")

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(GLFW REQUIRED)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(source)

add_library(
     vulkan-framwork
     include/VK_Buffer.h
     include/VK_Vertex.h
     include/VK_Context.h
     source/VK_ContextImpl.h
     source/VK_ShaderSetImpl.h
     source/VK_VertexBuffer.h
     source/VK_Buffer.cpp
     source/VK_ContextImpl.cpp
     source/VK_ShaderSetImpl.cpp
     source/VK_Vertex.cpp
     source/VK_VertexBuffer.cpp
     source/VK_UniformBufferImpl.cpp
     source/VK_Util.cpp
)

set(DEMOS blend-color hello-triangle clear-color indexed-traingle uniform-buffer)

foreach(var IN LISTS DEMOS)
    message("build ${var}")
    add_executable(${var} demo/main-${var}.cpp)
    target_link_libraries(${var} vulkan-framwork glfw vulkan)
endforeach()

message("finished ......................")
