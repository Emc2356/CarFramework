#pragma once

#include "Car/Renderer/GraphicsContext.hpp"
#include <glad/vulkan.h>

struct GLFWwindow;

struct CrQueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
    }
};

struct CrSwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

namespace Car {
    class VulkanGraphicsContext : public GraphicsContext {
    public:
        VulkanGraphicsContext(GLFWwindow* windowHandle);
        virtual ~VulkanGraphicsContext() override;

        virtual void init() override;
        virtual void swapBuffers() override;
        virtual void resize(uint32_t width, uint32_t height) override;

        VkInstance getInstance() const { return mInstance; }
        VkDebugUtilsMessengerEXT getDebugMessenger() const { return mDebugMessenger; }
        VkSurfaceKHR getSurface() const { return mSurface; }
        VkPhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; }
        VkDevice getDevice() const { return mDevice; }
        VkQueue getGraphicsQueue() const { return mGraphicsQueue; }
        VkQueue getPresentQueue() const { return mPresentQueue; }
        VkQueue getTransferQueue() const { return mTransferQueue; }
        VkSwapchainKHR getSwapChain() const { return mSwapChain; }
        VkFormat getSwapChainImageFormat() const { return mSwapChainImageFormat; }
        VkExtent2D getSwapChainExtent() const { return mSwapChainExtent; }
        std::vector<VkImage> getSwapChainImages() const { return mSwapChainImages; }
        std::vector<VkImageView> getSwapChainImageViews() const { return mSwapChainImageViews; }
        VkRenderPass getRenderPass() const { return mRenderPass; }
        std::vector<VkFramebuffer> getSwapChainFramebuffers() const { return mSwapChainFramebuffers; }
        VkCommandPool getRenderCommandPool() const { return mRenderCommandPool; }
        std::vector<VkCommandBuffer> getRenderCommandBuffers() const { return mRenderCommandBuffers; }
        VkCommandPool getTransferCommandPool() const { return mTransferCommandPool; }
        std::vector<VkCommandBuffer> getTransferCommandBuffers() const { return mTransferCommandBuffers; }
        std::vector<VkSemaphore> getImageAvailableSemaphores() const { return mImageAvailableSemaphores; }
        std::vector<VkSemaphore> getRenderFinishedSemaphores() const { return mRenderFinishedSemaphores; }
        std::vector<VkFence> getInFlightFences() const { return mInFlightFences; }
        VkCommandBuffer getCurrentRenderCommandBuffer() const { return mRenderCommandBuffers[mCurrentFrame]; }
        VkCommandBuffer getCurrentTransferCommandBuffer() const { return mTransferCommandBuffers[mCurrentFrame]; }
        VkSemaphore getCurrentImageAvailableSemaphore() const { return mImageAvailableSemaphores[mCurrentFrame]; }
        VkSemaphore getCurrentRenderFinishedSemaphore() const { return mRenderFinishedSemaphores[mCurrentFrame]; }
        VkFence getCurrentInFlightFence() const { return mInFlightFences[mCurrentFrame]; }
        uint32_t getCurrentFrameIndex() const { return mCurrentFrame; }
        uint32_t aquireNextImageIndex();
        uint32_t getImageIndex() const { return mImageIndex; }
        VkDescriptorPool getDescriptorPool() const { return mDescriptorPool; }

        // functions meant to be used by vulkan objects
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        // NOTE: it doesnt bind the memory
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                          VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset = 0,
                        VkDeviceSize dstOffset = 0);
        uint32_t getMaxFramesInFlight() const { return mMaxFramesInFlight; }

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void createSyncObjects();
        void createDescriptorPool();

        void cleanupSwapChain();
        void recreateSwapchain();

        // helper functions
        CrQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool isDeviceSuitable(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        CrSwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    private:
        GLFWwindow* mWindowHandle;

        VkInstance mInstance;

        VkDebugUtilsMessengerEXT mDebugMessenger;

        VkSurfaceKHR mSurface;

        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice;
        VkQueue mGraphicsQueue;
        VkQueue mPresentQueue;
        VkQueue mTransferQueue;

        VkSwapchainKHR mSwapChain;
        VkFormat mSwapChainImageFormat;
        VkExtent2D mSwapChainExtent;
        std::vector<VkImage> mSwapChainImages;
        std::vector<VkImageView> mSwapChainImageViews;

        VkRenderPass mRenderPass;

        std::vector<VkFramebuffer> mSwapChainFramebuffers;

        VkCommandPool mRenderCommandPool;
        std::vector<VkCommandBuffer> mRenderCommandBuffers;
        VkCommandPool mTransferCommandPool;
        std::vector<VkCommandBuffer> mTransferCommandBuffers;

        std::vector<VkSemaphore> mImageAvailableSemaphores;
        std::vector<VkSemaphore> mRenderFinishedSemaphores;
        std::vector<VkFence> mInFlightFences;

        VkDescriptorPool mDescriptorPool;

        uint32_t mCurrentFrame = 0;
        uint32_t mImageIndex = 0;
        uint32_t mMaxFramesInFlight = 2;
    };
} // namespace Car
