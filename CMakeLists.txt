cmake_minimum_required(VERSION 3.5)

project(vulkan-demo)

set(GLFW_ROOT "D:\\glfw-3.3.5.bin.WIN64")
include_directories("C:\\VulkanSDK\\1.2.189.2\\Include")
LINK_DIRECTORIES("C:\\VulkanSDK\\1.2.189.2\\Lib")
LINK_DIRECTORIES("D:\\glfw-3.3.5.bin.WIN64\\lib-mingw-w64")

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
INCLUDE_DIRECTORIES(./)

add_library(
     vulkan-framwork
     include/VK_Buffer.h
     include/VK_Vertex.h
     include/VK_Context.h
     include/VK_ShaderSet.h
     include/VK_UniformBuffer.h
     include/VK_Util.h
     source/stb_image.cpp
     source/VK_ContextImpl.h
     source/VK_ShaderSetImpl.h
     source/VK_VertexBuffer.h
     source/VK_Buffer.cpp
     source/VK_ContextImpl.cpp
     source/VK_ShaderSetImpl.cpp
     source/VK_Vertex.cpp
     source/VK_VertexBuffer.cpp
     source/VK_UniformBufferImpl.h
     source/VK_UniformBufferImpl.cpp
     source/VK_Viewports.cpp
     source/VK_ImageImpl.cpp
     source/VK_SamplerImpl.cpp
     source/VK_ImageView.cpp
     source/VK_ImageViewImpl.cpp
     source/VK_ValidationLayer.cpp
     source/VK_ShaderSet.cpp
     source/VK_Util.cpp
     source/VK_ObjLoader.cpp
     source/VK_ObjLoader.h
     source/tiny_obj_loader.cc
)

set(DEMOS
    hello-triangle
    indexed-traingle
    viewport-buffer
    blend-color

    cube-texture
    cube-color
    clear-color
    uniform-buffer
    uniform-buffer2
    sampler
    texture
    model-mesh
    model-mesh-geom
    model-mesh-tess
    )

foreach(var IN LISTS DEMOS)
    message("build ${var}")
    add_executable(${var} demo/main-${var}.cpp)
    if(WIN32)
        target_link_libraries(${var} vulkan-framwork glfw3 vulkan-1)
    else()
        target_link_libraries(${var} vulkan-framwork glfw vulkan)
    endif()
endforeach()

if(WIN32)
else()
    add_executable(opengl demo/opengl.cpp glad/glad.c)
    target_link_libraries(opengl glfw GL dl)
endif()

message("finished ......................")
