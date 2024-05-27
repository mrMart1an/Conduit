# Event systems source files
event_src = [
    'events/eventSystem.cpp',
    'events/eventCallback.cpp',
    'events/eventRegister.cpp'
]

# Engine core source files
core_src = [
    'core/logging.cpp', 
    'core/time.cpp', 
    'core/assert.cpp', 
    'core/application.cpp',
    'core/appRunner.cpp',
    'core/deleteQueue.cpp',
]

# Engine entity component system source files
ecs_src = [
    'ecs/entityRegister.cpp',
    'ecs/world.cpp'
]

# Engine window source code
window_src = [
    'window/glfw/glfwWindow.cpp',
    'window/glfw/glfwVulkan.cpp',
    'window/glfw/glfwCallbacks.cpp'
]

# Vulkan renderer source files
if(vulkan_enable.enabled())
    vulkan_src = [
        'renderer/vulkan/utils/vkValidation.cpp',    
        
        'renderer/vulkan/vkCommandBuffer.cpp',   
        'renderer/vulkan/vkContext.cpp',
        'renderer/vulkan/vkDevice.cpp',
        'renderer/vulkan/vkRenderer.cpp',   
        'renderer/vulkan/vkRenderPass.cpp',   
        'renderer/vulkan/vkSwapChain.cpp',
        
    ]
else
    vulkan_src = []
endif

# OpenGL renderer source files
if(opengl_enable.enabled())
    opengl_src = []
else
    opengl_src = []
endif

# Renderer source files
renderer_src = [
    vulkan_src,
    opengl_src,
    'renderer/renderer.cpp'
]

# Platform specific code source files
platform_src = [
    'platform/entryPoint.cpp'    
]

# Define the engine_source variable
engine_source = files([
    core_src,
    ecs_src,
    event_src,
    window_src,
    renderer_src,
    platform_src
])
