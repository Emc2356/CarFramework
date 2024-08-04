#pragma once

#include "Car/Renderer/Shader.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/internal/Vulkan/VertexBuffer.hpp"

namespace Car {
    class VulkanShader : public Shader {
    public:
        VulkanShader(const std::string& vertexBinary, const std::string& fragmeantBinary);
        virtual ~VulkanShader() override;

        virtual void bind() const override;
        virtual void unbind() const override;

        VkPipelineLayout getPipelineLayout() const { return mPipelineLayout; }
        VkPipeline getGraphicsPipeline() const { return mGraphicsPipeline; }

        // called by VulkanVertexArray to associtate shader and vb
        void trueCreateImplementation(Ref<VulkanVertexBuffer> vb);

    private:
        VkShaderModule createShaderModule(const std::vector<uint32_t>& code);

        void createDescriptorLayouts();
        // void createDescriptorSets();
        void createGraphicsPipeline(Ref<VulkanVertexBuffer> vb);

    private:
        std::vector<uint32_t> mVertBin;
        std::vector<uint32_t> mFragBin;

        // VkDescriptorSetLayout mDescriptorSetLayout;
        // std::vector<VkDescriptorSet> mDescriptorSets;

        VkPipelineLayout mPipelineLayout;
        VkPipeline mGraphicsPipeline;

        Ref<VulkanGraphicsContext> mGraphicsContext;
    };
} // namespace Car
