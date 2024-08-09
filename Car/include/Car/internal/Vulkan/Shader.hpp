#pragma once

#include "Car/Core/Ref.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/internal/Vulkan/UniformBuffer.hpp"
#include "Car/internal/Vulkan/VertexBuffer.hpp"

namespace Car {
    struct UniformBufferInput {
        Ref<VulkanUniformBuffer> ub;
        bool useInVertexShader;
        bool useInFragmeantShader;
    };
    
    class VulkanShader : public Shader {
    public:
        VulkanShader(const std::string& vertexBinary, const std::string& fragmeantBinary);
        virtual ~VulkanShader() override;

        virtual void bind() const override;
        virtual void unbind() const override;

        VkPipelineLayout getPipelineLayout() const { return mPipelineLayout; }
        VkPipeline getGraphicsPipeline() const { return mGraphicsPipeline; }

        virtual void setInput(Ref<UniformBuffer> ub, bool useInVertexShader, bool useInFragmeantShader) override {
            if (mPipelineLayout != VK_NULL_HANDLE) {
                CR_CORE_ERROR("Can not set an input to a shader after it has been binded with the VertexArray object");
                return;
            }
            mUniformBufferInputs.push_back({
                reinterpretCastRef<VulkanUniformBuffer>(ub),
                useInVertexShader,
                useInFragmeantShader 
            });
        }
        
        // called by VulkanVertexArray to associtate shader and vb
        void trueCreateImplementation(Ref<VulkanVertexBuffer> vb);

    private:
        VkShaderModule createShaderModule(const std::string& code);

        void createGraphicsPipeline(Ref<VulkanVertexBuffer> vb);

    private:
        std::string mVertBin;
        std::string mFragBin;
        
        std::vector<UniformBufferInput> mUniformBufferInputs;

        // TODO: Support multiple sets in the future 
        VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> mDescriptorSets;
        
        VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
        VkPipeline mGraphicsPipeline = VK_NULL_HANDLE;

        Ref<VulkanGraphicsContext> mGraphicsContext;
    };
} // namespace Car
