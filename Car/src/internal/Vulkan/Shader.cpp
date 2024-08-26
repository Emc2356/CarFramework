#include "Car/internal/Vulkan/Shader.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/internal/Vulkan/CompiledShader.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/Utils.hpp"

#include "Car/ResourceManager.hpp"
#include "Car/internal/Vulkan/Texture2D.hpp"
#include "Car/internal/Vulkan/UniformBuffer.hpp"

#include <glad/vulkan.h>
#include <stdexcept>

#ifdef CR_HAVE_SPIRV_CROSS
#include <spirv_cross/spirv_cross.hpp>
#endif

#ifdef CR_HAVE_SHADERC
#include <shaderc/shaderc.hpp>

static std::string crVkCompileSingleShader(const std::string& path, const shaderc_shader_kind kind) {
    std::string sourceCode = Car::readFile(path);
    shaderc::CompileOptions options;
    shaderc::Compiler compiler;
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetSourceLanguage(shaderc_source_language_glsl);

    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(sourceCode, kind, path.c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw std::runtime_error(result.GetErrorMessage());
    }

    std::vector<uint32_t> res(result.cbegin(), result.cend());

    return std::string(reinterpret_cast<char*>(res.data()), res.size() * sizeof(uint32_t));
}
#endif // CR_HAVE_SHADERC

namespace Car {
    CR_FORCE_INLINE VkFormat BufferLayoutDataTypeToVulkanType(ShaderLayoutInput::DataType type) {
        switch (type) {
        case ShaderLayoutInput::DataType::Float:
            return VK_FORMAT_R32_SFLOAT;
        case ShaderLayoutInput::DataType::Float2:
            return VK_FORMAT_R32G32_SFLOAT;
        case ShaderLayoutInput::DataType::Float3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case ShaderLayoutInput::DataType::Float4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ShaderLayoutInput::DataType::Mat3:
            CR_CORE_ERROR("ShaderLayoutInput::DataType::Mat3 not supported as vulkan vertex attribute");
            CR_DEBUGBREAK();
            return VK_FORMAT_UNDEFINED;
        case ShaderLayoutInput::DataType::Mat4:
            CR_CORE_ERROR("ShaderLayoutInput::DataType::Mat4 not supported as vulkan vertex attribute");
            CR_DEBUGBREAK();
            return VK_FORMAT_UNDEFINED;
        case ShaderLayoutInput::DataType::Int:
            return VK_FORMAT_R32_SINT;
        case ShaderLayoutInput::DataType::Int2:
            return VK_FORMAT_R32G32_SINT;
        case ShaderLayoutInput::DataType::Int3:
            return VK_FORMAT_R32G32B32_SINT;
        case ShaderLayoutInput::DataType::Int4:
            return VK_FORMAT_R32G32B32A32_SINT;
        case ShaderLayoutInput::DataType::UInt:
            return VK_FORMAT_R32_UINT;
        case ShaderLayoutInput::DataType::UInt2:
            return VK_FORMAT_R32G32_UINT;
        case ShaderLayoutInput::DataType::UInt3:
            return VK_FORMAT_R32G32B32_UINT;
        case ShaderLayoutInput::DataType::UInt4:
            return VK_FORMAT_R32G32B32A32_UINT;
        default:
            CR_CORE_ERROR("Unrecognized ShaderLayoutInput::DataType: {0}", static_cast<uint32_t>(type));
            CR_DEBUGBREAK();
            return VK_FORMAT_UNDEFINED;
        }
    }

    VulkanShader::VulkanShader(const CompiledShader& compiledShader, const ShaderLayoutInput& inputLayout,
                               const Specification* pSpec)
        : mCompiledShader(compiledShader), mInputLayout(inputLayout) {
        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());

        createDescriptors();
        createGraphicsPipeline(pSpec);
    }

    VulkanShader::~VulkanShader() {
        VkDevice device = mGraphicsContext->getDevice();

        vkDeviceWaitIdle(device);

        for (const VkDescriptorSetLayout& descriptorSetLayout : mDescriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        }

        vkDestroyPipeline(device, mGraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
    }

    void VulkanShader::bind() const {
        VkCommandBuffer cmdBuffer = mGraphicsContext->getCurrentRenderCommandBuffer();
        if (mCompiledShader.sets.size()) {
            // maybe it shouldnt bind it if it doesnt changes
            vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0,
                                    mDescriptorSets[mGraphicsContext->getCurrentFrameIndex()].size(),
                                    mDescriptorSets[mGraphicsContext->getCurrentFrameIndex()].data(), 0, nullptr);
        }
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);
    }

    void VulkanShader::setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<UniformBuffer> ub) {
        CR_IF (set > mCompiledShader.sets.size()) {
            CR_CORE_ERROR("set {0} binding {1} does not exist", set, binding);
            CR_DEBUGBREAK();
            return;
        }
        CR_IF (1) {
            bool exists = false;
            for (const Descriptor& descriptor : mCompiledShader.sets[set]) {
                if (descriptor.binding == binding) {
                    if (descriptor.descriptorType != DescriptorType::UniformBuffer) {
                        CR_CORE_ERROR("set {0} binding {1} is not a uniformbuffer", set, binding);
                        CR_DEBUGBREAK();
                        return;
                    }
                    exists = true;
                }
            }

            if (!exists) {
                CR_CORE_ERROR("set {0} binding {1} does not exist", set, binding);
                CR_DEBUGBREAK();
                return;
            }
        }

        VkDevice device = mGraphicsContext->getDevice();
        if (applyToAll) {
            for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
                VkDescriptorBufferInfo bufferInfo =
                    reinterpretCastRef<VulkanUniformBuffer>(ub)->getDescriptorBufferInfo(i);

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = mDescriptorSets[i][set];
                descriptorWrite.dstBinding = binding;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = &bufferInfo;
                descriptorWrite.pImageInfo = nullptr;
                descriptorWrite.pTexelBufferView = nullptr;

                vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
            }
        } else {
            VkDescriptorBufferInfo bufferInfo = reinterpretCastRef<VulkanUniformBuffer>(ub)->getDescriptorBufferInfo(
                mGraphicsContext->getCurrentFrameIndex());

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = mDescriptorSets[mGraphicsContext->getCurrentFrameIndex()][set];
            descriptorWrite.dstBinding = binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        }
    }

    void VulkanShader::setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<Texture2D> texture) {
        CR_IF (set > mCompiledShader.sets.size()) {
            CR_CORE_ERROR("set {0} binding {1} does not exist", set, binding);
            CR_DEBUGBREAK();
            return;
        }
        CR_IF (1) {
            bool exists = false;
            for (const Descriptor& descriptor : mCompiledShader.sets[set]) {
                if (descriptor.binding == binding) {
                    if (descriptor.descriptorType != DescriptorType::Sampler2D) {
                        CR_CORE_ERROR("set {0} binding {1} is not a sampler2D", set, binding);
                        CR_DEBUGBREAK();
                        return;
                    }
                    exists = true;
                }
            }

            if (!exists) {
                CR_CORE_ERROR("set {0} binding {1} does not exist", set, binding);
                CR_DEBUGBREAK();
                return;
            }
        }

        VkDevice device = mGraphicsContext->getDevice();
        if (applyToAll) {
            for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
                VkDescriptorImageInfo imageInfo =
                    reinterpretCastRef<VulkanTexture2D>(texture)->getDescriptorImageInfo();

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = mDescriptorSets[i][set];
                descriptorWrite.dstBinding = binding;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrite.descriptorCount = 1;
                descriptorWrite.pBufferInfo = nullptr;
                descriptorWrite.pImageInfo = &imageInfo;
                descriptorWrite.pTexelBufferView = nullptr;

                vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
            }
        } else {
            VkDescriptorImageInfo imageInfo = reinterpretCastRef<VulkanTexture2D>(texture)->getDescriptorImageInfo();

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = mDescriptorSets[mGraphicsContext->getCurrentFrameIndex()][set];
            descriptorWrite.dstBinding = binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = nullptr;
            descriptorWrite.pImageInfo = &imageInfo;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        }
    }

    void VulkanShader::createDescriptors() {
        VkDevice device = mGraphicsContext->getDevice();

        mDescriptorSetLayouts.resize(mCompiledShader.sets.size());
        mDescriptorSets.resize(mGraphicsContext->getMaxFramesInFlight());
        for (uint32_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
            mDescriptorSets[i].resize(mCompiledShader.sets.size());
        }

        for (uint32_t i = 0; i < mCompiledShader.sets.size(); i++) {
            std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayouts;
            for (uint32_t j = 0; j < mCompiledShader.sets[i].size(); j++) {
                switch (mCompiledShader.sets[i][j].descriptorType) {
                case Car::DescriptorType::UniformBuffer: {
                    VkDescriptorSetLayoutBinding uboLayoutBinding{};
                    uboLayoutBinding.binding = mCompiledShader.sets[i][j].binding;
                    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    uboLayoutBinding.descriptorCount = 1;

                    switch (mCompiledShader.sets[i][j].stageFlags) {
                    case Car::DescriptorStage::VertexShader:
                        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                        break;
                    case Car::DescriptorStage::FragmeantShader:
                        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                        break;
                    case Car::DescriptorStage::Combined:
                        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                        break;
                    }
                    uboLayoutBinding.pImmutableSamplers = nullptr;

                    descriptorSetLayouts.push_back(uboLayoutBinding);
                    break;
                }
                case Car::DescriptorType::Sampler2D: {
                    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
                    samplerLayoutBinding.binding = mCompiledShader.sets[i][j].binding;
                    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    samplerLayoutBinding.descriptorCount = 1;

                    switch (mCompiledShader.sets[i][j].stageFlags) {
                    case Car::DescriptorStage::VertexShader:
                        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                        break;
                    case Car::DescriptorStage::FragmeantShader:
                        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                        break;
                    case Car::DescriptorStage::Combined:
                        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                        break;
                    }
                    samplerLayoutBinding.pImmutableSamplers = nullptr;

                    descriptorSetLayouts.push_back(samplerLayoutBinding);
                    break;
                }
                case Car::DescriptorType::NONE: {
                    throw std::runtime_error("internal error in VulkanShader.cpp in createGraphicsPipeline");
                }
                }
            }

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = descriptorSetLayouts.size();
            layoutInfo.pBindings = descriptorSetLayouts.data();

            if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &mDescriptorSetLayouts[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor set layout!");
            }

            std::vector<VkDescriptorSetLayout> layouts(mGraphicsContext->getMaxFramesInFlight(),
                                                       mDescriptorSetLayouts[i]);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = mGraphicsContext->getDescriptorPool();
            allocInfo.descriptorSetCount = layouts.size();
            allocInfo.pSetLayouts = layouts.data();

            std::vector<VkDescriptorSet> descriptorSets(mGraphicsContext->getMaxFramesInFlight());

            if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }
            for (uint32_t k = 0; k < descriptorSets.size(); k++) {
                mDescriptorSets[k][i] = descriptorSets[k];
            }
        }
    }

    void VulkanShader::createGraphicsPipeline(const Specification* pSpec) {
        VkDevice device = mGraphicsContext->getDevice();

        VkShaderModule vertShaderModule = createShaderModule(mCompiledShader.vertexShader);
        VkShaderModule fragShaderModule = createShaderModule(mCompiledShader.fragmeantShader);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = pSpec->vertexShaderEntryName.c_str();

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = pSpec->fragmentShaderEntryName.c_str();

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = mInputLayout.getTotalSize();
        switch (pSpec->vertexInputRate) {
        case Shader::VertexInputRate::VERTEX: {
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            break;
        }
        default: {
            throw std::runtime_error("unrecognized VertexInputRate " +
                                     std::to_string((uint32_t)pSpec->vertexInputRate));
            break;
        }
        }

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        const std::vector<ShaderLayoutInput::Element>& elements = mInputLayout.getElements();
        attributeDescriptions.resize(elements.size());

        for (uint32_t i = 0; i < attributeDescriptions.size(); i++) {
            attributeDescriptions[i].binding = 0;
            // TODO: location might be wrong if ex an element is dvec4
            attributeDescriptions[i].location = i;
            attributeDescriptions[i].format = BufferLayoutDataTypeToVulkanType(elements[i].type);
            attributeDescriptions[i].offset = elements[i].offset;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        switch (pSpec->primitiveTopology) {
        case Shader::PrimitiveTopology::POINT_LIST: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        }
        case Shader::PrimitiveTopology::LINE_LIST: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        }
        case Shader::PrimitiveTopology::LINE_STRIP: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        }
        case Shader::PrimitiveTopology::TRIANGLE_LIST: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        }
        case Shader::PrimitiveTopology::TRIANGLE_STRIP: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
        }
        case Shader::PrimitiveTopology::TRIANGLE_FAN: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
            break;
        }
        case Shader::PrimitiveTopology::LINE_LIST_WITH_ADJACENCY: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;
            break;
        }
        case Shader::PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;
            break;
        }
        case Shader::PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;
            break;
        }
        case Shader::PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;
            break;
        }
        case Shader::PrimitiveTopology::PATCH_LIST: {
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
            break;
        }
        default: {
            throw std::runtime_error("unrecognized primitive topology " +
                                     std::to_string((uint32_t)pSpec->primitiveTopology));
            break;
        }
        }
        inputAssembly.primitiveRestartEnable = pSpec->primitiveRestartEnable ? VK_TRUE : VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)mGraphicsContext->getSwapChainExtent().width;
        viewport.height = (float)mGraphicsContext->getSwapChainExtent().height;
        viewport.minDepth = pSpec->minDepth;
        viewport.maxDepth = pSpec->maxDepth;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = mGraphicsContext->getSwapChainExtent();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        switch (pSpec->polygonMode) {
        case Shader::PolygonMode::FILL: {
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            break;
        }
        case Shader::PolygonMode::LINE: {
            rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
            break;
        }
        case Shader::PolygonMode::POINT: {
            rasterizer.polygonMode = VK_POLYGON_MODE_POINT;
            break;
        }
        default: {
            throw std::runtime_error("unrecognized polygon mode " + std::to_string((uint32_t)pSpec->polygonMode));
            break;
        }
        }
        rasterizer.lineWidth = 1.0f;
        switch (pSpec->cullMode) {
        case Shader::CullMode::NONE: {
            rasterizer.cullMode = VK_CULL_MODE_NONE;
            break;
        }
        case Shader::CullMode::FRONT: {
            rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
            break;
        }
        case Shader::CullMode::BACK: {
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            break;
        }
        case Shader::CullMode::FRONT_AND_BACK: {
            rasterizer.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
            break;
        }
        default: {
            throw std::runtime_error("unrecognized cull mode " + std::to_string((uint32_t)pSpec->cullMode));
            break;
        }
        }
        switch (pSpec->frontFace) {
        case Shader::FrontFace::CLOCKWISE: {
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            break;
        }
        case Shader::FrontFace::COUNTER_CLOCKWISE: {
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
        }
        default: {
            throw std::runtime_error("unrecognized front face " + std::to_string((uint32_t)pSpec->frontFace));
            break;
        }
        }
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f;          // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;          // Optional
        multisampling.pSampleMask = nullptr;            // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE;      // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        if (mDescriptorSetLayouts.size() > 0) {
            pipelineLayoutInfo.setLayoutCount = mDescriptorSetLayouts.size();
            pipelineLayoutInfo.pSetLayouts = mDescriptorSetLayouts.data();
        } else {
            pipelineLayoutInfo.setLayoutCount = 0;
            pipelineLayoutInfo.pSetLayouts = nullptr;
        }

        // i dont think i want to support push_constant
        pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = mPipelineLayout;
        pipelineInfo.renderPass = mGraphicsContext->getRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1;              // Optional

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }

    VkShaderModule VulkanShader::createShaderModule(const std::string& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        // questionable code sadly
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.c_str());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(mGraphicsContext->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }
    
#if defined(CR_HAVE_SPIRV_CROSS)
    static void fillSetField(SingleCompiledShader* pSCS) {
        pSCS->sets.resize(0);
        spirv_cross::Compiler compiler((uint32_t*)pSCS->shader.data(),
                                           pSCS->shader.size() / 4);
        spirv_cross::ShaderResources resources(compiler.get_shader_resources());

        uint32_t maxDescriptorSetCount = 0;
        for (const auto& uniformBuffer : resources.uniform_buffers) {
            maxDescriptorSetCount = MAX(compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet) + 1,
                                        maxDescriptorSetCount);
        }
        for (const auto& uniformBuffer : resources.sampled_images) {
            maxDescriptorSetCount = MAX(compiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet) + 1,
                                        maxDescriptorSetCount);
        }

        if (maxDescriptorSetCount > 4) {
            throw std::runtime_error("A shader can have max of 4 sets per the vulkan spec");
        }

        pSCS->sets.resize(maxDescriptorSetCount);

        for (const auto& resource : resources.uniform_buffers) {
            uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint8_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            pSCS->sets[set].push_back({
                binding,
                Car::DescriptorType::UniformBuffer,
                Car::DescriptorStage::VertexShader,
            });
        }

        for (const auto& resource : resources.sampled_images) {
            uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint8_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            pSCS->sets[set].push_back({
                binding,
                Car::DescriptorType::Sampler2D,
                Car::DescriptorStage::VertexShader,
            });
        }
    }
#endif
    
    static CompiledShader combineSingleShaders(SingleCompiledShader* vertShader, SingleCompiledShader* fragShader) {
        CompiledShader ret;
        ret.vertexShader = std::string(vertShader->shader);
        ret.fragmeantShader = std::string(fragShader->shader);
        ret.sets.resize(MAX(vertShader->sets.size(), fragShader->sets.size()));
        
        for (uint32_t i = 0; i < vertShader->sets.size(); i++) {
            std::vector<Descriptor>& set = vertShader->sets[i];
            for (const Descriptor& descriptor : set) {
                ret.sets[i].push_back({
                    descriptor.binding,
                    descriptor.descriptorType,
                    DescriptorStage::VertexShader,
                });
            }
        }
        
        for (uint32_t i = 0; i < fragShader->sets.size(); i++) {
            std::vector<Descriptor>& set = fragShader->sets[i];
            for (const Descriptor& descriptor : set) {
                bool shouldPush = true;
                
                for (auto& otherDescriptor : ret.sets[i]) {
                    if (descriptor.binding == otherDescriptor.binding) {
                        shouldPush = false;
                        
                        if (descriptor.descriptorType != otherDescriptor.descriptorType) {
                            throw std::runtime_error("descriptor missmatch");
                        }
                        
                        otherDescriptor.stageFlags = DescriptorStage::Combined;
                    }
                }
                
                if (shouldPush) {
                    ret.sets[i].push_back({
                        descriptor.binding,
                        descriptor.descriptorType,
                        DescriptorStage::FragmeantShader,
                    });
                }
            }
        }
        
        for (const auto& set : ret.sets) {
            if (set.size() == 0) {
                throw std::runtime_error("can not have an empty set in a shader");
            }
        }
        
        return ret;
    }

    Ref<Shader> Shader::Create(const std::string& vertexShaderName, const std::string& fragmeantShaderName,
                               const ShaderLayoutInput& inputLayout, const Shader::Specification* pSpec) {
        SingleCompiledShader vertCompiledShader;
        SingleCompiledShader fragCompiledShader;

        std::filesystem::path shadersPath =
            (std::filesystem::path)ResourceManager::getResourceDirectory() / ResourceManager::getShadersSubdirectory();
        std::filesystem::path cacheShaderDir = shadersPath / "__CACHE__";

        std::string vertPath = shadersPath / vertexShaderName;
        std::string fragPath = shadersPath / fragmeantShaderName;
        std::filesystem::path vertCacheFile = std::string(cacheShaderDir / vertexShaderName) + ".crss";
        std::filesystem::path fragCacheFile = std::string(cacheShaderDir / fragmeantShaderName) + ".crss";

        if (!std::filesystem::exists(vertCacheFile)) {
#if defined(CR_HAVE_SHADERC) && defined(CR_HAVE_SPIRV_CROSS)
            std::filesystem::create_directories(vertCacheFile.parent_path());
            CR_CORE_DEBUG("compiling vertex shader {}", vertPath);
            vertCompiledShader.shader = crVkCompileSingleShader(vertPath, shaderc_vertex_shader);
            fillSetField(&vertCompiledShader);
            
            writeToFile(vertCacheFile, vertCompiledShader.toBytes());
#else
            CR_CORE_ERROR("Can not online compile shaders without shaderc and spirv-cross");
#endif // CR_HAVE_SHADERC
        } else {
            CR_CORE_DEBUG("Loading pre-processed vertex shader from {}", (std::string)vertCacheFile);
            std::string data = readFile(vertCacheFile);
            vertCompiledShader = SingleCompiledShader::fromBytes({data.begin(), data.end()});
        }

        if (!std::filesystem::exists(fragCacheFile)) {
#if defined(CR_HAVE_SHADERC) && defined(CR_HAVE_SPIRV_CROSS)
            std::filesystem::create_directories(fragCacheFile.parent_path());
            CR_CORE_DEBUG("compiling fragmeant shader {}", fragPath);
            fragCompiledShader.shader = crVkCompileSingleShader(fragPath, shaderc_fragment_shader);
            fillSetField(&fragCompiledShader);
            
            writeToFile(fragCacheFile, fragCompiledShader.toBytes());
#else
            CR_CORE_ERROR("Can not online compile shaders without shaderc and spirv-cross");
#endif // CR_HAVE_SHADERC
        } else {
            CR_CORE_DEBUG("Loading pre-processed fragmeant shader from {}", (std::string)fragCacheFile);
            std::string data = readFile(fragCacheFile);
            fragCompiledShader = SingleCompiledShader::fromBytes({data.begin(), data.end()});
        }

        CompiledShader compiledShader = combineSingleShaders(&vertCompiledShader, &fragCompiledShader);
        
        return createRef<VulkanShader>(compiledShader, inputLayout, pSpec);
    }
} // namespace Car
