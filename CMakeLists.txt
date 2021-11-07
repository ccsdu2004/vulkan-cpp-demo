cmake_minimum_required(VERSION 3.5)

project(vulkan-demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/module")

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#find_package(GLFW REQUIRED)

SET(FILES
    main.cpp
    VK_Vertex.h
    VK_Deleter.h
    VK_ShaderSet.h
    source/VK_Vertex.cpp
    source/VK_ShaderSetImpl.h
    source/VK_ShaderSetImpl.cpp
    Vertex.cpp
    VertexBuffer.cpp
    stb_image.cpp
    VulkanApp.cpp
    VulkanApp.h
    VulkanAppData.cpp)

add_executable(vulkan-demo main.cpp) #${FILES})
target_link_libraries(vulkan-demo glfw vulkan)
