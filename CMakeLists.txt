cmake_minimum_required(VERSION 3.5)

project(vulkan-demo)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/module")

set(GLFW_ROOT "D:\\glfw-3.3.5.bin.WIN64")
include_directories("C:\\VulkanSDK\\1.2.189.2\\Include")
LINK_DIRECTORIES("C:\\VulkanSDK\\1.2.189.2\\Lib")
LINK_DIRECTORIES("D:\\glfw-3.3.5.bin.WIN64\\lib-mingw-w64")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/module")

#SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -pg")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#find_package(GLFW REQUIRED)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(thirdparty)
INCLUDE_DIRECTORIES(source)
INCLUDE_DIRECTORIES(./)
link_directories(./lib)

add_library(
     vulkan-framwork
     include/VK_Buffer.h
     include/VK_Vertex.h
     include/VK_Context.h
     include/VK_ShaderSet.h
     include/VK_Texture.h
     include/VK_UniformBuffer.h
     include/VK_PipelineCache.h
     include/VK_Pipeline.h
     include/VK_Util.h
     source/VK_ContextImpl.h
     source/VK_ShaderSetImpl.h
     source/VK_VertexBuffer.h
     source/VK_Buffer.cpp
     source/VK_ContextImpl.cpp
     source/VK_ShaderSetImpl.cpp
     source/VK_Vertex.cpp
     source/VK_VertexBuffer.cpp
     source/VK_InstanceBuffer.h
     source/VK_InstanceBuffer.cpp
     source/VK_IndirectBuffer.h
     source/VK_IndirectBuffer.cpp
     source/VK_UniformBufferImpl.h
     source/VK_UniformBufferImpl.cpp
     source/VK_ImageImpl.cpp
     source/VK_SamplerImpl.cpp
     source/VK_ImageView.cpp
     source/VK_ImageViewImpl.cpp
     source/VK_ValidationLayer.cpp
     source/VK_ShaderSet.cpp
     source/VK_Util.cpp
     source/VK_Allocator.cpp
     source/VK_ObjLoader.cpp
     source/VK_ObjLoader.h
     source/VK_DynamicStateImpl.h
     source/VK_DynamicStateImpl.cpp
     source/VK_PipelineCacheImpl.h
     source/VK_PipelineCacheImpl.cpp
     source/VK_Pipeline.cpp
     source/VK_PipelineImpl.h
     source/VK_PipelineImpl.cpp
     source/VK_PipelineDeriveImpl.h
     source/VK_PipelineDeriveImpl.cpp
     source/VK_DescriptorSets.h
     source/VK_DescriptorSets.cpp
     source/VK_DescriptorSetLayout.h
     source/VK_DescriptorSetLayout.cpp
     source/VK_PipelineLayout.h
     source/VK_PipelineLayout.cpp
     source/VK_DescriptorPool.h
     source/VK_DescriptorPool.cpp
     source/VK_RenderPass.h
     source/VK_RenderPass.cpp
     source/VK_PushDescriptor.h
     source/VK_PushDescriptor.cpp
     include/VK_CommandPool.h
     source/VK_CommandPool.cpp
     include/VK_SecondaryCommandBuffer.h
     source/VK_SecondaryCommandBuffer.cpp
     include/VK_SecondaryCommandBufferCallback.h
     source/VK_SecondaryCommandBufferCallback.cpp
     thirdparty/tiny_obj_loader.cc
     thirdparty/stb_image.cpp
     thirdparty/tiff.cpp
)

add_subdirectory(demo)

message("finished ......................")
