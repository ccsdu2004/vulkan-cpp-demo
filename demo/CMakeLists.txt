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
    texture-clear-image
    texture-copy-image
    texture-blit-image
    texture-save-image
    texture-push-descriptor
    texel-buffer
    query
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
    secondary-command-buffer
    )

foreach(var IN LISTS DEMOS)
    message("build ${var}")
    add_executable(${var} main-${var}.cpp)
    if(WIN32)
        target_link_libraries(${var} vulkan-framwork glfw3 vulkan-1 tiff)
    else()
        target_link_libraries(${var} vulkan-framwork glfw vulkan tiff)
    endif()
endforeach()

if(WIN32)
else()
    add_executable(opengl main-opengl.cpp ../glad/glad.c)
    target_link_libraries(opengl glfw GL dl)
endif()
