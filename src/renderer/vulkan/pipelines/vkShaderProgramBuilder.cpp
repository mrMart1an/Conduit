#include "renderer/vulkan/pipelines/vkShaderProgramBuilder.h"
#include "conduit/renderer/rendererException.h"
#include "renderer/vulkan/pipelines/vkShaderProgram.h"
#include "renderer/vulkan/utils/vkExceptions.h"
#include <vector>

namespace cndt::vulkan {

// Parse the shader modules
std::vector<VulkanShaderModule> 
VulkanShaderProgramBuilder::parseShaderModule() const
{
    // Crete shader module vector
    std::vector<VulkanShaderModule> modules;
    // Reserve space for the maximum number of modules 
    modules.reserve(5);

    try {
        if (m_compute_shader.has_value()) {
            modules.push_back(
                m_device_p->createShaderModule(m_compute_shader.value())
            );
        }
    
        if (m_vertex_shader.has_value()) {
            modules.push_back(
                m_device_p->createShaderModule(m_vertex_shader.value())
            );
        }
        if (m_fragment_shader.has_value()) {
            modules.push_back(
                m_device_p->createShaderModule(m_fragment_shader.value())
            );
        }
        if (m_geometry_shader.has_value()) {
            modules.push_back(
                m_device_p->createShaderModule(m_geometry_shader.value())
            );
        }
        if (m_tessel_eval_shader.has_value()) {
            modules.push_back(
                m_device_p->createShaderModule(m_tessel_eval_shader.value())
            );
        }
        if (m_tessel_control_shader.has_value()) {
            modules.push_back(
                m_device_p->createShaderModule(m_tessel_control_shader.value())
            );
        }
    } catch (ShaderModuleException &e) {
        for (auto& module : modules) {
            m_device_p->destroyShaderModule(module);
        }

        throw e;
    }
  
    return modules;
}

// Parse the rasterizer information 
VkPipelineRasterizationStateCreateInfo 
VulkanShaderProgramBuilder::parseRasterizationInfo() const
{
    // Rasterizer create info
    VkPipelineRasterizationStateCreateInfo rasterizer = { };
    rasterizer.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // Check if raster config has a value
    if (!m_raster_config.has_value())
        return rasterizer;

    ShaderProgram::RasterConfig config = m_raster_config.value();

    rasterizer.rasterizerDiscardEnable = VK_FALSE;

    // Depth settings
    rasterizer.depthClampEnable = VK_FALSE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // polygon mode settings
    switch (config.poly_mode) {
        case ShaderProgram::RasterConfig::PolyMode::Fill:
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            break;
        case ShaderProgram::RasterConfig::PolyMode::Line:
            rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
            break;

        default:
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    }
    rasterizer.lineWidth = 1.0f;

    // Cull config
    switch (config.cull_mode) {
        case ShaderProgram::RasterConfig::CullMode::None:
            rasterizer.cullMode = VK_CULL_MODE_NONE;
            break;
        case ShaderProgram::RasterConfig::CullMode::Back:
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            break;
        case ShaderProgram::RasterConfig::CullMode::Front:
            rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
            break;
        case ShaderProgram::RasterConfig::CullMode::FrontAndBack:
            rasterizer.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
            break;

        default:
            rasterizer.cullMode = VK_CULL_MODE_NONE;
    }

    switch (config.front_face) {
        case ShaderProgram::RasterConfig::FrontFace::Clockwise:
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            break;
        case ShaderProgram::RasterConfig::FrontFace::CounterClockwise:
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;

        default:
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    }

    return rasterizer;
}

// Parse the multi sampling information 
VkPipelineMultisampleStateCreateInfo 
VulkanShaderProgramBuilder::parseMultisamplingInfo() const
{
    VkPipelineMultisampleStateCreateInfo multisampling = { };
    multisampling.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    // Check if raster config has a value
    if (!m_raster_config.has_value())
        return multisampling;

    // Enable SSAA
    multisampling.sampleShadingEnable = 
        m_raster_config.value().ssaa_sampling ? VK_TRUE : VK_FALSE;

    // Get vulkan sample count
    switch (m_raster_config.value().sample_count) {
        case ShaderProgram::RasterConfig::SampleCount::Count_1:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            break;
        case ShaderProgram::RasterConfig::SampleCount::Count_2:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_2_BIT;
            break;
        case ShaderProgram::RasterConfig::SampleCount::Count_4:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_4_BIT;
            break;
        case ShaderProgram::RasterConfig::SampleCount::Count_8:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_8_BIT;
            break;
        case ShaderProgram::RasterConfig::SampleCount::Count_16:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_16_BIT;
            break;
        case ShaderProgram::RasterConfig::SampleCount::Count_32:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_32_BIT;
            break;
        case ShaderProgram::RasterConfig::SampleCount::Count_64:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_64_BIT;
            break;

        default:
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    }

    return multisampling;
}

// Parse depth stencil information
VkPipelineDepthStencilStateCreateInfo 
VulkanShaderProgramBuilder::parseDepthStencilInfo() const
{
    VkPipelineDepthStencilStateCreateInfo depth_stencil = { };
    depth_stencil.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Check if raster config has a value
    if (!m_raster_config.has_value())
        return depth_stencil;

    ShaderProgram::RasterConfig config = m_raster_config.value();

    depth_stencil.depthTestEnable = config.depth_testing ? VK_TRUE : VK_FALSE;
    depth_stencil.depthWriteEnable = 
        config.depth_write_back ? VK_TRUE : VK_FALSE;

    switch (config.depth_comparison_op) {
        case ShaderProgram::RasterConfig::DepthComparison::Less:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
            break;
        case ShaderProgram::RasterConfig::DepthComparison::LessOrEqual:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
            break;
        case ShaderProgram::RasterConfig::DepthComparison::Greater:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_GREATER;
            break;
        case ShaderProgram::RasterConfig::DepthComparison::GreaterOrEqual:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
            break;
        case ShaderProgram::RasterConfig::DepthComparison::Equal:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_EQUAL;
            break;
        case ShaderProgram::RasterConfig::DepthComparison::NotEqual:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_NOT_EQUAL;
            break;
        case ShaderProgram::RasterConfig::DepthComparison::Always:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;
            break;
        case ShaderProgram::RasterConfig::DepthComparison::Never:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_NEVER;
            break;

        default:
            depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
    }

    return depth_stencil;
}

// Build the shader program from the information 
// and shader stage currently stored in the builder
RendererResRef<ShaderProgram> VulkanShaderProgramBuilder::buildCache()
{
    // Check configuration validity
    if (m_type == ShaderProgram::Type::Compute) {
        if (!m_compute_shader.has_value()) {
            throw InvalidShaderProgram(
                RendererBackend::Vulkan,
                "Compute program need a compute shader"
            );
        }
    } else if (m_type == ShaderProgram::Type::Graphics) {
        if (!m_vertex_shader.has_value() || !m_fragment_shader.has_value()) {
            throw InvalidShaderProgram(
                RendererBackend::Vulkan,
                "Graphics program need at least a vertex and fragment shader"
            );
        }

        if (!m_raster_config.has_value()) {
            throw InvalidShaderProgram(
                RendererBackend::Vulkan,
                "Graphics program need rasterizer configuration"
            );
        }
    } else {
        throw InvalidShaderProgram(
            RendererBackend::Vulkan,
            "Program type undefined"
        );
    }
    
    // Parse shader stage modules
    std::vector<VulkanShaderModule> modules = parseShaderModule();

    // Parse rasterizer and multi sampling information
    VkPipelineRasterizationStateCreateInfo rasterizer_info =
        parseRasterizationInfo();
    VkPipelineMultisampleStateCreateInfo multisampling_info = 
        parseMultisamplingInfo();
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = 
        parseDepthStencilInfo();

    VulkanShaderProgram *out_program_p = new VulkanShaderProgram(
        m_device_p,
        std::move(modules),
        
        rasterizer_info,
        multisampling_info,
        depth_stencil_info,

        m_type
    );

    return RendererResRef<ShaderProgram>(out_program_p);
}

} // namespace cndt::vulkan 
