#pragma once

#include "Car/Renderer/GraphicsContext.hpp"
#include <glad/vulkan.h>
#include <vulkan/vulkan_core.h>

struct GLFWwindow;

#ifndef CR_MAX_FRAMES_IN_FLIGHT
#define CR_MAX_FRAMES_IN_FLIGHT 2
#endif 

struct CrQueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
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
        VkSwapchainKHR getSwapChain() const { return mSwapChain; }
        VkFormat getSwapChainImageFormat() const { return mSwapChainImageFormat; }
        VkExtent2D getSwapChainExtent() const { return mSwapChainExtent; }
        std::vector<VkImage> getSwapChainImages() const { return mSwapChainImages; }
        std::vector<VkImageView> getSwapChainImageViews() const { return mSwapChainImageViews; }
        VkRenderPass getRenderPass() const { return mRenderPass; }
        std::vector<VkFramebuffer> getSwapChainFramebuffers() const { return mSwapChainFramebuffers; }
        VkCommandPool getCommandPool() const { return mCommandPool; }
        std::array<VkCommandBuffer, CR_MAX_FRAMES_IN_FLIGHT> getCommandBuffers() const { return mCommandBuffers; }
        std::array<VkSemaphore, CR_MAX_FRAMES_IN_FLIGHT> getImageAvailableSemaphores() const { return mImageAvailableSemaphores; }
        std::array<VkSemaphore, CR_MAX_FRAMES_IN_FLIGHT> getRenderFinishedSemaphores() const { return mRenderFinishedSemaphores; }
        std::array<VkFence, CR_MAX_FRAMES_IN_FLIGHT> getInFlightFences() const { return mInFlightFences; }
        VkCommandBuffer getCurrentCommandBuffer() const { return mCommandBuffers[mCurrentFrame]; }
        VkSemaphore getCurrentImageAvailableSemaphore() const { return mImageAvailableSemaphores[mCurrentFrame]; }
        VkSemaphore getCurrentRenderFinishedSemaphore() const { return mRenderFinishedSemaphores[mCurrentFrame]; }
        VkFence getCurrentInFlightFence() const { return mInFlightFences[mCurrentFrame]; }
        uint32_t aquireNextImageIndex();
        uint32_t getImageIndex() const { return mImageIndex; }
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
        
        void cleanupSwapChain();
        void recreateSwapchain();

        // helper functions
        CrQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkQueueFlagBits requestedFlags);
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

        VkSwapchainKHR mSwapChain;
        VkFormat mSwapChainImageFormat;
        VkExtent2D mSwapChainExtent;
        std::vector<VkImage> mSwapChainImages;
        std::vector<VkImageView> mSwapChainImageViews;
        
        VkRenderPass mRenderPass;
        
        std::vector<VkFramebuffer> mSwapChainFramebuffers;
        
        VkCommandPool mCommandPool;
        std::array<VkCommandBuffer, CR_MAX_FRAMES_IN_FLIGHT> mCommandBuffers;
        
        std::array<VkSemaphore, CR_MAX_FRAMES_IN_FLIGHT> mImageAvailableSemaphores;
        std::array<VkSemaphore, CR_MAX_FRAMES_IN_FLIGHT> mRenderFinishedSemaphores;
        std::array<VkFence, CR_MAX_FRAMES_IN_FLIGHT> mInFlightFences;

        uint32_t mCurrentFrame = 0;
        uint32_t mImageIndex = 0;
    };
} // namespace Car
