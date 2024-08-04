#include "Car/Core/Ref.hpp"
#include "Car/Renderer/VertexArray.hpp"

#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/internal/Vulkan/Shader.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Car/internal/Vulkan/Renderer.hpp"

#include <glad/vulkan.h>

namespace Car {
    Renderer* Renderer::sInstance = new VulkanRenderer();
    Ref<VulkanGraphicsContext> sGraphicsContext;
    static glm::vec4 sClearColor;

    void VulkanRenderer::InitImpl() {
        sGraphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());

        sClearColor = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    }

    void VulkanRenderer::ShutdownImpl() {}

    void VulkanRenderer::ClearColorImpl(float r, float g, float b, float a) { sClearColor = glm::vec4(r, g, b, a); }

    void VulkanRenderer::ClearImpl() {}

    void VulkanRenderer::EnableBlendingImpl() {}

    void VulkanRenderer::DisableBlendingImpl() {}

    void VulkanRenderer::EnableDepthTestImpl() {}

    void VulkanRenderer::DisableDepthTestImpl() {}

    void VulkanRenderer::BeginRecordingImpl() {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;                  // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();
        VkDevice device = sGraphicsContext->getDevice();

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
        std::memcpy(clearColor.color.float32, glm::value_ptr(sClearColor), sizeof(glm::vec4));

        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderer::EndRecordingImpl() {
        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();

        vkCmdEndRenderPass(cmdBuffer);
        if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to end recording of command buffer");
        }
    }

    void VulkanRenderer::DrawTrianglesImpl(const Ref<VertexArray> va, uint32_t triangleCount) {
        if (triangleCount > va->getIndexBuffer()->getCount() * 3) {
            CR_CORE_ERROR("Car::Renderer::DrawTriangles(VertexArray, triangleCount={}) "
                          "index buffer doesnt have enough items",
                          triangleCount);
            return;
        }
        Ref<VulkanShader> shader = reinterpretCastRef<VulkanShader>(va->getShader());
        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();
        VkExtent2D swapChainExtent = sGraphicsContext->getSwapChainExtent();

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->getGraphicsPipeline());

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

        vkCmdDrawIndexed(cmdBuffer, triangleCount * 3, 1, 0, 0, 0);
    }

    void VulkanRenderer::DrawTrianglesImpl(const Ref<VertexArray> va) {
        // if (va->getIndexBuffer()->getCount() % 3 != 0) {
        //     CR_CORE_ERROR("Car::Render::DrawTriangles recieved vertex array which "
        //                   "holds an incompatible index buffer (size={})",
        //                   va->getIndexBuffer()->getCount());
        //     return;
        // }

        va->bind();

        VkCommandBuffer cmdBuffer = sGraphicsContext->getCurrentRenderCommandBuffer();
        VkExtent2D swapChainExtent = sGraphicsContext->getSwapChainExtent();

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

        vkCmdDrawIndexed(cmdBuffer, va->getIndexBuffer()->getCount(), 1, 0, 0, 0);
    }
} // namespace Car
