#include "Car/Core/Ref.hpp"
#include "Car/Renderer/VertexArray.hpp"

#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/internal/Vulkan/Shader.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Car/internal/Vulkan/Renderer.hpp"

#include <glad/vulkan.h>

struct VulkanRendererData {
    glm::vec4 clearColor;
};

namespace Car {
    Renderer* Renderer::sInstance = new VulkanRenderer();
    Ref<VulkanGraphicsContext> sGraphicsContext;
    static VulkanRendererData* sData;

    void VulkanRenderer::InitImpl() {
        sData = new VulkanRendererData();
        sGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());
    }

    void VulkanRenderer::ShutdownImpl() { delete sData; }

    void VulkanRenderer::ClearColorImpl(float r, float g, float b, float a) {
        sData->clearColor = glm::vec4(r, g, b, a);
    }

    void VulkanRenderer::BeginRecordingImpl() {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                  // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();
        VkDevice device = sGraphicsContext->getDevice();
        VkExtent2D swapChainExtent = sGraphicsContext->getSwapChainExtent();

        VkFence inFlightFence = sGraphicsContext->getCurrentInFlightFence();
        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFence);

        uint32_t imageIndex = sGraphicsContext->aquireNextImageIndex();
        vkResetCommandBuffer(cmdBuffer, /*VkCommandBufferResetFlagBits*/ 0);

        if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = sGraphicsContext->getRenderPass();
        renderPassInfo.framebuffer = sGraphicsContext->getSwapChainFramebuffers()[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = sGraphicsContext->getSwapChainExtent();

        VkClearValue clearColor;
        std::memcpy(clearColor.color.float32, glm::value_ptr(sData->clearColor), sizeof(glm::vec4));

        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
    }

    void VulkanRenderer::EndRecordingImpl() {
        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();

        vkCmdEndRenderPass(cmdBuffer);
        if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to end recording of command buffer");
        }
    }
    
    void VulkanRenderer::SetViewportImpl(float x, float y, float width, float height, float minDepth, float maxDepth) {
        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();
        
        VkViewport viewport{};
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        
        vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
    }
    
    void VulkanRenderer::SetScissorImpl(int32_t x, int32_t y, int32_t width, int32_t height) {
        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();
        
        VkRect2D scissor{};
        scissor.offset.x = x;
        scissor.offset.y = y;
        scissor.extent.width = width;
        scissor.extent.height = height;

        vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
    }

    void VulkanRenderer::DrawCommandImpl(const Ref<VertexArray> va, uint32_t indicesCount) {
        va->bind();

        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();

        vkCmdDrawIndexed(cmdBuffer, indicesCount, 1, 0, 0, 0);
    }
} // namespace Car
