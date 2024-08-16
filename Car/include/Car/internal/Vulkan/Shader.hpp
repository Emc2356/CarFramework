#pragma once

#include "Car/Core/Ref.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/internal/Vulkan/CompiledShader.hpp"
#include "Car/internal/Vulkan/UniformBuffer.hpp"
#include "Car/internal/Vulkan/VertexBuffer.hpp"

namespace Car {
    class VulkanShader : public Shader {
    public:
        VulkanShader(const CompiledShader& compiledShader, const ShaderLayoutInput& inputLayout);
        virtual ~VulkanShader() override;

        virtual void bind() const override;

        virtual const ShaderLayoutInput& getInputLayout() const override { return mInputLayout; }
        
        VkPipelineLayout getPipelineLayout() const { return mPipelineLayout; }
        VkPipeline getGraphicsPipeline() const { return mGraphicsPipeline; }
        
        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<UniformBuffer> ub) override;
        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<Texture2D> texture) override;

        VkShaderModule createShaderModule(const std::string& code);
    
        void createGraphicsPipeline();
        void createDescriptors();
    private:
        CompiledShader mCompiledShader;

        ShaderLayoutInput mInputLayout;
        // TODO: Support multiple sets in the future
        std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
        std::vector<std::vector<VkDescriptorSet>> mDescriptorSets;

        VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
        VkPipeline mGraphicsPipeline = VK_NULL_HANDLE;

        Ref<VulkanGraphicsContext> mGraphicsContext;
    };
} // namespace Car
