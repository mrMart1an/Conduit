#include "renderer/vulkan/pipelines/vkShaderProgramBuilder.h"
#include "conduit/renderer/rendererException.h"
#include "renderer/vulkan/pipelines/vkShaderProgram.h"
#include "renderer/vulkan/utils/vkExceptions.h"

#include "spirv_cross.hpp"

#include <vector>

namespace cndt::vulkan {

namespace spvc = spirv_cross;

// Build the shader program from the information 
// and shader stage currently stored in the builder
RenderRef<ShaderProgram> VulkanShaderProgramBuilder::buildCache()
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

        if (!m_vertex_config.has_value()) {
            throw InvalidShaderProgram(
                RendererBackend::Vulkan,
                "Graphics program need vertex input configuration"
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

    // Parse vertex input info
    VkVertexInputBindingDescription vertex_binding_desc = 
        parseVertexBinding();
    std::vector<VkVertexInputAttributeDescription> vertex_attribute_desc =
        parseVertexAttribute();

    VulkanShaderProgram *out_program_p = new VulkanShaderProgram(
        m_device_p,
        std::move(modules),
        
        rasterizer_info,
        multisampling_info,
        depth_stencil_info,

        vertex_binding_desc,
        vertex_attribute_desc,

        m_type
    );

    return RenderRef<ShaderProgram>(out_program_p);
}

// Parse vertex binding description
VkVertexInputBindingDescription 
VulkanShaderProgramBuilder::parseVertexBinding() const
{
    VkVertexInputBindingDescription binding_description { };

    if (!m_vertex_config.has_value())
        return binding_description;
    
    binding_description.binding = m_vertex_config->binding;
    binding_description.stride = m_vertex_config->stride;
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
}

// Parse vertex attribute description
std::vector<VkVertexInputAttributeDescription> 
VulkanShaderProgramBuilder::parseVertexAttribute() const
{
    if (!m_vertex_config.has_value())
        return std::vector<VkVertexInputAttributeDescription>(); 

    // Get reflection location type information
    spvc::Compiler comp(m_vertex_shader.value()->getVkSpvVec());
    spvc::ShaderResources resources = comp.get_shader_resources();

    std::vector<std::pair<u32, spvc::SPIRType>> type_table;
    type_table.reserve(resources.stage_inputs.size());

    for (auto& input : resources.stage_inputs) {
        u32 location = comp.get_decoration(
            input.id, spv::DecorationLocation
        );

        spvc::SPIRType type = comp.get_type(input.type_id);

        type_table.push_back({location, type});
    }

    // Generate the attribute descriptor
    std::vector<VkVertexInputAttributeDescription> 
    attributes(m_vertex_config->attributes.size());

    for (int i = 0; i < m_vertex_config->attributes.size(); i++) {
        auto& attribute = m_vertex_config->attributes[i];

        // Linear search should be efficient enough 
        std::optional<spvc::SPIRType> type = std::nullopt;
        for (auto& element : type_table) {
            // Check if the location match
            if (element.first == attribute.location) {
                type = element.second;
                break;
            }
        }

        // Check if the location exist in the shader 
        if (!type.has_value()) {
            throw InvalidShaderProgram(
                RendererBackend::Vulkan,
                "Location {} doesn't exist in vertex shader",
                attribute.location
            );            
        }

        // Check if the location type is currently supported
        if (
            type->basetype != spvc::SPIRType::Half &&
            type->basetype != spvc::SPIRType::Float &&
            type->basetype != spvc::SPIRType::Double &&
            type->basetype != spvc::SPIRType::Int64 &&
            type->basetype != spvc::SPIRType::UInt64 &&
            type->basetype != spvc::SPIRType::Int &&
            type->basetype != spvc::SPIRType::UInt &&
            type->basetype != spvc::SPIRType::Short &&
            type->basetype != spvc::SPIRType::UShort &&
            type->basetype != spvc::SPIRType::SByte &&
            type->basetype != spvc::SPIRType::UByte
        ) {
            throw InvalidShaderProgram(
                RendererBackend::Vulkan,
                "The type at location {} is currently unsupported",
                attribute.location
            );            
        }

        // Check if the type is a vector, matrix are currently unsupported
        if (type->columns != 1) {
            throw InvalidShaderProgram(
                RendererBackend::Vulkan,
                "Matrix aren't currently supported",
                attribute.location
            );            
        }

        // Check for 64 bit type vector length
        if (
            type->basetype == spvc::SPIRType::Double || 
            type->basetype == spvc::SPIRType::Int64 || 
            type->basetype == spvc::SPIRType::UInt64 
        ) {
            if (type->vecsize > 2) {
                throw InvalidShaderProgram(
                    RendererBackend::Vulkan,
                    "vector with 64-bit type and legth > 2 arent't supported",
                    attribute.location
                );            
            }
        }

	    attributes[i].binding = m_vertex_config->binding;
	    attributes[i].location = attribute.location;
	    attributes[i].offset = attribute.offset;

	    attributes[i].format = getVkFormat(
            attribute.format, 
            attribute.size,
            type.value()
        );
    }

	return attributes;
}


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

// Convert vertex input and count to vkFormat
VkFormat VulkanShaderProgramBuilder::getVkFormat(
    VertexLayout::Format format,
    u32 size,
    spvc::SPIRType loc_type
) const {
    // Parse the shader type
    bool shader_float;
    if (
        loc_type.basetype == spvc::SPIRType::Half ||
        loc_type.basetype == spvc::SPIRType::Float ||
        loc_type.basetype == spvc::SPIRType::Double 
    ) {
        shader_float = true;
    } else {
        shader_float = false;
    }

    // Check for 64 bit compatibility
    if (
        loc_type.basetype == spvc::SPIRType::Int64 ||
        loc_type.basetype == spvc::SPIRType::UInt64 ||
        loc_type.basetype == spvc::SPIRType::Double 
    ) {
        if (
            format != VertexLayout::Format::i64 ||
            format != VertexLayout::Format::u64 ||
            format != VertexLayout::Format::f64 
        ) {
            goto invalid_format;
        }
    }

    switch (format) {
        case VertexLayout::Format::u8: {
            switch (size) {
                case 1:
                    if (shader_float)
                        return VK_FORMAT_R8_UNORM;
                    else
                        return VK_FORMAT_R8_UINT;
                case 2:
                    if (shader_float)
                        return VK_FORMAT_R8G8_UNORM;
                    else
                        return VK_FORMAT_R8G8_UINT;
                case 3:
                    if (shader_float)
                        return VK_FORMAT_R8G8B8_UNORM;
                    else
                        return VK_FORMAT_R8G8B8_UINT;
                case 4:
                    if (shader_float)
                        return VK_FORMAT_R8G8B8A8_UNORM;
                    else
                        return VK_FORMAT_R8G8B8A8_UINT;

                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::u16: {
            switch (size) {
                case 1:
                    if (shader_float)
                        return VK_FORMAT_R16_UNORM;
                    else
                        return VK_FORMAT_R16_UINT;
                case 2:
                    if (shader_float)
                        return VK_FORMAT_R16G16_UNORM;
                    else
                        return VK_FORMAT_R16G16_UINT;
                case 3:
                    if (shader_float)
                        return VK_FORMAT_R16G16B16_UNORM;
                    else
                        return VK_FORMAT_R16G16B16_UINT;
                case 4:
                    if (shader_float)
                        return VK_FORMAT_R16G16B16A16_UNORM;
                    else
                        return VK_FORMAT_R16G16B16A16_UINT;

                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::u32: {
            if (shader_float)
                goto invalid_format;

            switch (size) {
                case 1:
                    return VK_FORMAT_R32_UINT;
                case 2:
                    return VK_FORMAT_R32G32_UINT;
                case 3:
                    return VK_FORMAT_R32G32B32_UINT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_UINT;

                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::u64: {
            if (shader_float)
                goto invalid_format;

            switch (size) {
                case 1:
                    return VK_FORMAT_R64_UINT;
                case 2:
                    return VK_FORMAT_R64G64_UINT;

                default:
                    goto invalid_size;
            }
        }

        case VertexLayout::Format::i8: {
            switch (size) {
                case 1:
                    if (shader_float)
                        return VK_FORMAT_R8_SNORM;
                    else
                        return VK_FORMAT_R8_SINT;
                case 2:
                    if (shader_float)
                        return VK_FORMAT_R8G8_SNORM;
                    else
                        return VK_FORMAT_R8G8_SINT;
                case 3:
                    if (shader_float)
                        return VK_FORMAT_R8G8B8_SNORM;
                    else
                        return VK_FORMAT_R8G8B8_SINT;
                case 4:
                    if (shader_float)
                        return VK_FORMAT_R8G8B8A8_SNORM;
                    else
                        return VK_FORMAT_R8G8B8A8_SINT;

                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::i16: {
            switch (size) {
                case 1:
                    if (shader_float)
                        return VK_FORMAT_R16_SNORM;
                    else
                        return VK_FORMAT_R16_SINT;
                case 2:
                    if (shader_float)
                        return VK_FORMAT_R16G16_SNORM;
                    else
                        return VK_FORMAT_R16G16_SINT;
                case 3:
                    if (shader_float)
                        return VK_FORMAT_R16G16B16_SNORM;
                    else
                        return VK_FORMAT_R16G16B16_SINT;
                case 4:
                    if (shader_float)
                        return VK_FORMAT_R16G16B16A16_SNORM;
                    else
                        return VK_FORMAT_R16G16B16A16_SINT;

                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::i32: {
            if (shader_float)
                goto invalid_format;

            switch (size) {
                case 1:
                    return VK_FORMAT_R32_SINT;
                case 2:
                    return VK_FORMAT_R32G32_SINT;
                case 3:
                    return VK_FORMAT_R32G32B32_SINT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_SINT;

                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::i64: {
            if (shader_float)
                goto invalid_format;

            switch (size) {
                case 1:
                    return VK_FORMAT_R64_SINT;
                case 2:
                    return VK_FORMAT_R64G64_SINT;

                default:
                    goto invalid_size;
            }
        }

        case VertexLayout::Format::f16: {
            if (!shader_float)
                goto invalid_format;

            switch (size) {
                case 1:
                    return VK_FORMAT_R16_SFLOAT;
                case 2:
                    return VK_FORMAT_R16G16_SFLOAT;
                case 3:
                    return VK_FORMAT_R16G16B16_SFLOAT;
                case 4:
                    return VK_FORMAT_R16G16B16A16_SFLOAT;
                
                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::f32: {
            if (!shader_float)
                goto invalid_format;

            switch (size) {
                case 1:
                    return VK_FORMAT_R32_SFLOAT;
                case 2:
                    return VK_FORMAT_R32G32_SFLOAT;
                case 3:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case 4:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                
                default:
                    goto invalid_size;
            }
        }
        case VertexLayout::Format::f64: {
            if (!shader_float)
                goto invalid_format;

            switch (size) {
                case 1:
                    return VK_FORMAT_R64_SFLOAT;
                case 2:
                    return VK_FORMAT_R64G64_SFLOAT;
                
                default:
                    goto invalid_size;
            }
        }
        default:
            goto invalid_format;
    }

invalid_format:
    throw InvalidShaderProgram(
        RendererBackend::Vulkan,
        "Invalid vertex format specified in shader program builder"
    );

// Exceptions go to 
invalid_size:
    throw InvalidShaderProgram(
        RendererBackend::Vulkan,
        "Invalid size for the given vertex format ({})",
        size
    );
}

} // namespace cndt::vulkan 
