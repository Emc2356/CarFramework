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
        VulkanShader(const CompiledShader& compiledShader, const ShaderLayoutInput& inputLayout,
                     const Specification* pSpec);
        virtual ~VulkanShader() override;

        virtual void bind() const override;

        virtual const ShaderLayoutInput& getInputLayout() const override { return mInputLayout; }

        VkPipelineLayout getPipelineLayout() const { return mPipelineLayout; }
        VkPipeline getGraphicsPipeline() const { return mGraphicsPipeline; }

        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<UniformBuffer> ub) override;
        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<Texture2D> texture) override;

        VkShaderModule createShaderModule(const std::string& code);

        void createDescriptors();
        void createPipelineLayout();
        void createGraphicsPipeline();

    private:
        CompiledShader mCompiledShader;

        ShaderLayoutInput mInputLayout;

        std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
        std::vector<std::vector<VkDescriptorSet>> mDescriptorSets;

        VkPipelineLayout mPipelineLayout;
        VkPipeline mGraphicsPipeline;

        Ref<VulkanGraphicsContext> mGraphicsContext;

        Specification mSpec;
    };
} // namespace Car
