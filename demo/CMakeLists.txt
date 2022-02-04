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
    texture-push-descriptor
    model
    model-model
    model-mesh
    model-mesh-dynamic-linewidth
    model-mesh-geom
    model-mesh-tess
    model-toon
    model-outline
    model-push-constant
    model-light
    model-instance
    )

foreach(var IN LISTS DEMOS)
    message("build ${var}")
    add_executable(${var} main-${var}.cpp)
    if(WIN32)
        target_link_libraries(${var} vulkan-framwork glfw3 vulkan-1)
    else()
        target_link_libraries(${var} vulkan-framwork glfw vulkan)
    endif()
endforeach()

if(WIN32)
else()
    add_executable(opengl main-opengl.cpp ../glad/glad.c)
    target_link_libraries(opengl glfw GL dl)
endif()
