set(BASE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Event systems source files
set(EVENT_SRC
    "${BASE_PATH}/events/eventSystem.cpp"
    "${BASE_PATH}/events/eventCallback.cpp"
    "${BASE_PATH}/events/eventRegister.cpp"
)

# Engine core source files
set(CORE_SRC
    "${BASE_PATH}/core/logging.cpp" 
    "${BASE_PATH}/core/time.cpp" 
    "${BASE_PATH}/core/assert.cpp" 
    "${BASE_PATH}/core/application.cpp"
    "${BASE_PATH}/core/appRunner.cpp"
    "${BASE_PATH}/core/deleteQueue.cpp"
)

# Assets manager source file
set(ASSETS_SRC
    "${BASE_PATH}/assets/assetManager.cpp" 
    "${BASE_PATH}/assets/assetManagerShader.cpp" 
    "${BASE_PATH}/assets/assetParserFuns.cpp" 
    "${BASE_PATH}/assets/assetLoaderFuns.cpp" 
)

# Engine entity component system source files
set(ECS_SRC
    "${BASE_PATH}/ecs/commandBuffer.cpp"
    "${BASE_PATH}/ecs/componentRegister.cpp"
    "${BASE_PATH}/ecs/entityRegister.cpp"
    "${BASE_PATH}/ecs/world.cpp"
)

# Engine window source code
set(WINDOW_SRC
    "${BASE_PATH}/window/glfw/glfwWindow.cpp"
    "${BASE_PATH}/window/glfw/glfwVulkan.cpp"
    "${BASE_PATH}/window/glfw/glfwCallbacks.cpp"
)

# Vulkan renderer source files
if(VULKAN_BACKEND)
    set(VULKAN_SRC
        "${BASE_PATH}/renderer/vulkan/utils/vkValidation.cpp"    
        "${BASE_PATH}/renderer/vulkan/utils/vkAttributeDescriptor.cpp"    
        
        "${BASE_PATH}/renderer/vulkan/pipelines/vkPipeline.cpp"   
        
        "${BASE_PATH}/renderer/vulkan/descriptor/vkDescriptorLayout.cpp"   
        "${BASE_PATH}/renderer/vulkan/descriptor/vkDescriptorAllocator.cpp"   
        "${BASE_PATH}/renderer/vulkan/descriptor/vkDescriptorWriter.cpp"   
        
        "${BASE_PATH}/renderer/vulkan/storage/vkBuffer.cpp"   
        "${BASE_PATH}/renderer/vulkan/storage/vkImage.cpp"   
        
        "${BASE_PATH}/renderer/vulkan/sync/vkFence.cpp"   
        
        "${BASE_PATH}/renderer/vulkan/vkCommandBuffer.cpp"   
        "${BASE_PATH}/renderer/vulkan/vkCommandPool.cpp"   
        "${BASE_PATH}/renderer/vulkan/vkContext.cpp"
        "${BASE_PATH}/renderer/vulkan/vkDevice.cpp"
        "${BASE_PATH}/renderer/vulkan/vkRenderer.cpp"   
        "${BASE_PATH}/renderer/vulkan/vkRenderPass.cpp"   
        "${BASE_PATH}/renderer/vulkan/vkSwapChain.cpp"
    )
endif()

# OpenGL renderer source files
#if(OPENGL_BACKEND)
#endif()

# Renderer source files
set(RENDERER_SRC
    ${VULKAN_SRC}
    ${OPENGL_SRC}
    "${BASE_PATH}/renderer/renderer.cpp"
)

# Platform specific code source files
set(PLATFORM_SRC
    "${BASE_PATH}/platform/entryPoint.cpp"    
)

# Define the engine_source variable
set(ENGINE_SRC
    
    ${CORE_SRC}
    ${ECS_SRC}
    ${ASSETS_SRC}
    ${EVENT_SRC}
    ${WINDOW_SRC}
    ${RENDERER_SRC}
    ${PLATFORM_SRC}
    
    PARENT_SCOPE
)
