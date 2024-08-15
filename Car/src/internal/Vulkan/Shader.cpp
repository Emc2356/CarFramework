#include "Car/internal/Vulkan/Shader.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/Renderer/BufferLayout.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/internal/Vulkan/CompiledShader.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/Utils.hpp"

#include "Car/ResourceManager.hpp"
#include "Car/internal/Vulkan/UniformBuffer.hpp"

#include <glad/vulkan.h>
#include <spirv_cross/spirv_cross.hpp>
#include <stdexcept>
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

    VulkanShader::VulkanShader(const CompiledShader& compiledShader, const ShaderLayoutInput& inputLayout)
        : mCompiledShader(compiledShader), mInputLayout(inputLayout) {
        mGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());
        
        createDescriptors();
        createGraphicsPipeline();
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
            vkCmdBindDescriptorSets(
                cmdBuffer, 
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mPipelineLayout,
                0,
                mDescriptorSets[mGraphicsContext->getCurrentFrameIndex()].size(),
                mDescriptorSets[mGraphicsContext->getCurrentFrameIndex()].data(),
                0,
                nullptr
            );
        }
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);
    }
    
    void VulkanShader::setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<UniformBuffer> ub) {
        VkDevice device = mGraphicsContext->getDevice();
        if (applyToAll) {
            for (size_t i = 0; i < mGraphicsContext->getMaxFramesInFlight(); i++) {
                VkDescriptorBufferInfo bufferInfo = reinterpretCastRef<VulkanUniformBuffer>(ub)->getDescriptorBufferInfo(i);
                
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
            VkDescriptorBufferInfo bufferInfo = reinterpretCastRef<VulkanUniformBuffer>(ub)->getDescriptorBufferInfo(mGraphicsContext->getCurrentFrameIndex());
            
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
                            case Car::DescriptorStage::VertexShader: uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; break;
                            case Car::DescriptorStage::FragmeantShader: uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                            case Car::DescriptorStage::Combined: uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; break;
                        }

                        descriptorSetLayouts.push_back(uboLayoutBinding);
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
            
            std::vector<VkDescriptorSetLayout> layouts(mGraphicsContext->getMaxFramesInFlight(), mDescriptorSetLayouts[i]);
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

    void VulkanShader::createGraphicsPipeline() {
        VkDevice device = mGraphicsContext->getDevice();

        VkShaderModule vertShaderModule = createShaderModule(mCompiledShader.vertexShader);
        VkShaderModule fragShaderModule = createShaderModule(mCompiledShader.fragmeantShader);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = mInputLayout.getTotalSize();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

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
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)mGraphicsContext->getSwapChainExtent().width;
        viewport.height = (float)mGraphicsContext->getSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

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
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        // rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        // rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

    static void fillSetField(CompiledShader* pCompiledShader) {
        spirv_cross::Compiler vertCompiler((uint32_t*)pCompiledShader->vertexShader.data(), pCompiledShader->vertexShader.size() / 4);
        spirv_cross::ShaderResources vertResources(vertCompiler.get_shader_resources());
        spirv_cross::Compiler fragCompiler((uint32_t*)pCompiledShader->fragmeantShader.data(), pCompiledShader->fragmeantShader.size() / 4);
        spirv_cross::ShaderResources fragResources(fragCompiler.get_shader_resources());

        uint32_t maxDescriptorSetCount = 0;
        for (const auto& uniformBuffer : vertResources.uniform_buffers) {
            maxDescriptorSetCount = MAX(vertCompiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet) + 1, maxDescriptorSetCount);
        }
        for (const auto& uniformBuffer : fragResources.uniform_buffers) {
            maxDescriptorSetCount = MAX(fragCompiler.get_decoration(uniformBuffer.id, spv::DecorationDescriptorSet) + 1, maxDescriptorSetCount);
        }
        if (maxDescriptorSetCount > 4) {
            throw std::runtime_error("A shader can have max of 4 sets per the vulkan spec");
        }

        pCompiledShader->sets.resize(maxDescriptorSetCount);

        for (const auto& resource : vertResources.uniform_buffers) {
            uint32_t set = vertCompiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint8_t binding = vertCompiler.get_decoration(resource.id, spv::DecorationBinding);
            pCompiledShader->sets[set].push_back({
               binding,
               Car::DescriptorType::UniformBuffer,
               Car::DescriptorStage::VertexShader,
            });
        }

        for (const auto& resource : fragResources.uniform_buffers) {
            uint32_t set = fragCompiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            uint8_t binding = fragCompiler.get_decoration(resource.id, spv::DecorationBinding);
            bool shouldPush = true;
            for (auto& binding_ : pCompiledShader->sets[set]) {
                if (binding == binding_.binding) {
                    if (binding_.descriptorType == Car::DescriptorType::UniformBuffer) {
                        binding_.stageFlags = Car::DescriptorStage::Combined;
                        shouldPush = false;
                    } else {
                        throw std::runtime_error("Descriptor missmatch between two shader in set " + std::to_string(set) + "and in binding " + std::to_string(binding));
                    }
                }
            }
            if (shouldPush) {
                pCompiledShader->sets[set].push_back({
                    binding,
                    Car::DescriptorType::UniformBuffer,
                    Car::DescriptorStage::FragmeantShader,
                });
            }
        }

        for (const auto& set : pCompiledShader->sets) {
            if (set.size() == 0) {
                throw std::runtime_error("Interal error in " + (std::string)__FILE__ + ":" + std::to_string(__LINE__));
            }
        }
    }

    Ref<Shader> Shader::Create(const std::string& vertexShaderName, const std::string& fragmeantShaderName, const ShaderLayoutInput& inputLayout) {
        std::string vertexBinary;
        std::string fragmeantBinary;

        std::filesystem::path shadersPath =
            (std::filesystem::path)ResourceManager::getResourceDirectory() / ResourceManager::getShadersSubdirectory();
        std::filesystem::path cacheShaderDir = shadersPath / "__CACHE__";

        std::string vertPath = shadersPath / vertexShaderName;
        std::string fragPath = shadersPath / fragmeantShaderName;
        std::filesystem::path vertCacheFile = std::string(cacheShaderDir / vertexShaderName) + ".spv";
        std::filesystem::path fragCacheFile = std::string(cacheShaderDir / fragmeantShaderName) + ".spv";

        if (!std::filesystem::exists(vertCacheFile)) {
#ifdef CR_HAVE_SHADERC
            std::filesystem::create_directories(vertCacheFile.parent_path());
            vertexBinary = crVkCompileSingleShader(vertPath, shaderc_vertex_shader);

            writeToFile(vertCacheFile, reinterpret_cast<const uint8_t*>(vertexBinary.data()), vertexBinary.size());
#else
            CR_CORE_ERROR("Can not online compile shaders without shaderc");
#endif // CR_HAVE_SHADERC
        } else {
            CR_CORE_DEBUG("Loading pre-compiled SPIRV vertex shader");
            std::string data = readFile(vertCacheFile);
            vertexBinary = {data.cbegin(), data.cend()};
        }

        if (!std::filesystem::exists(fragCacheFile)) {
#ifdef CR_HAVE_SHADERC
            std::filesystem::create_directories(fragCacheFile.parent_path());
            fragmeantBinary = crVkCompileSingleShader(fragPath, shaderc_fragment_shader);

            writeToFile(fragCacheFile, reinterpret_cast<const uint8_t*>(fragmeantBinary.data()),
                        fragmeantBinary.size() * sizeof(fragmeantBinary[0]));
#else
            CR_CORE_ERROR("Can not online compile shaders without shaderc");
#endif // CR_HAVE_SHADERC
        } else {
            CR_CORE_DEBUG("Loading pre-compiled SPIRV fragmeant shader");
            std::string data = readFile(fragCacheFile);
            fragmeantBinary = {data.cbegin(), data.cend()};
        }

        CompiledShader compiledShader;

        compiledShader.vertexShader = vertexBinary;
        compiledShader.fragmeantShader = fragmeantBinary;
        fillSetField(&compiledShader);

        return createRef<VulkanShader>(compiledShader, inputLayout);
    }
} // namespace Car
