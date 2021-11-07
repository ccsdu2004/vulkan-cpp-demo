TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += include
INCLUDEPATH += D:/glfw-3.3.5.bin.WIN32/include
INCLUDEPATH += C:/VulkanSDK/1.2.189.2/Include

LIBS += C:/VulkanSDK/1.2.189.2/Lib32/vulkan-1.lib
LIBS += D:/glfw-3.3.5.bin.WIN32/lib-vc2017/glfw3dll.lib

SOURCES += \
    demo/main.cpp \
    source/VK_Buffer.cpp \
    source/VK_ContextImpl.cpp \
    source/VK_ShaderSetImpl.cpp \
    source/VK_Vertex.cpp \
    source/VK_VertexBuffer.cpp

HEADERS += \
    include/VK_Buffer.h \
    include/VK_Vertex.h\
    include/VK_Context.h \
    source/VK_ContextImpl.h \
    source/VK_ShaderSetImpl.h \
    source/VK_VertexBuffer.h

