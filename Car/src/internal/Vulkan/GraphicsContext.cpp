#include "Car/internal/Vulkan/GraphicsContext.hpp"
#include "Car/Core/Core.hpp"
#include "Car/Utils.hpp"
#include "Car/Application.hpp"
#include "Car/Core/Log.hpp"

// include glad before glfw so `VK_VERSION_1_0` is defined
#include <glad/vulkan.h>
#include <GLFW/glfw3.h>

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#if defined(CR_DEBUG)
constexpr bool gEnableValidationLayers = true;
#else
constexpr bool gEnableValidationLayers = false;
#endif

const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static VKAPI_ATTR VkBool32 VKAPI_CALL crVkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData) {
    UNUSED(pUserData);
    std::string msgType;

    switch (messageType) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        msgType = "GENERAL";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        msgType = "VALIDATION";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        msgType = "PERFORMANCE";
        break;
    default:
        msgType = "UNKNOWN";
        break;
    }

    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        CR_CORE_INFO("[VALIDATION LAYER] from [{0}]: {1}", msgType, std::string(pCallbackData->pMessage));
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        CR_CORE_WARN("[VALIDATION LAYER] from [{0}]: {1}", msgType, std::string(pCallbackData->pMessage));
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        CR_CORE_ERROR("[VALIDATION LAYER] from [{0}]: {1}", msgType, std::string(pCallbackData->pMessage));
        CR_DEBUGBREAK();
        break;
    default:
        CR_CORE_ERROR("[VALIDATION LAYER] from [{0}]: {1}", msgType, std::string(pCallbackData->pMessage));
        CR_DEBUGBREAK();
        break;
    }

    return VK_FALSE;
}

// used to populate the struct for the actual debug messager and this also get passed in the instance create info so we
// get debug info about VkCreateInstance and VkDestroyInstance functions
void crPopulateDebugMessagerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo, void* userData) {
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = crVkDebugCallback;
    createInfo->pUserData = userData;
}

std::string crGetPhysicalDeviceName(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    return std::string(properties.deviceName);
}

bool crCheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

namespace Car {
    CrQueueFamilyIndices VulkanGraphicsContext::findQueueFamilies(VkPhysicalDevice device) {
        CrQueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[i];

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }
            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                indices.transferFamily = i;
            }

            if (indices.isComplete()) {
                return indices;
            }
        }

        return indices;
    }

    VulkanGraphicsContext::VulkanGraphicsContext(GLFWwindow* windowHandle) : mWindowHandle(windowHandle) {
        CR_ASSERT(windowHandle, "Interal Error: null window handle sent to vulkan graphics context");
    }

    void VulkanGraphicsContext::init() {
        if (!gladLoaderLoadVulkan(nullptr, nullptr, nullptr)) {
            throw std::runtime_error("Car: Failed to initialize glad");
        }

        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();
        createDescriptorPool();

        CR_CORE_DEBUG("Vulkan Context Initialized");
    }

    bool VulkanGraphicsContext::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool VulkanGraphicsContext::isDeviceSuitable(VkPhysicalDevice device) {
        CrQueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            CrSwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    std::vector<const char*> getRequiredExtensions() {
        std::vector<const char*> extensions = Application::Get()->getWindow()->getRequiredInstanceExtensions();

        if (gEnableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    CrSwapChainSupportDetails VulkanGraphicsContext::querySwapChainSupport(VkPhysicalDevice device) {
        CrSwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR
    VulkanGraphicsContext::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR
    VulkanGraphicsContext::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        if (Application::Get()->getWindow()->isVSync()) {
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanGraphicsContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(mWindowHandle, &width, &height);

            VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

            actualExtent.width =
                std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height =
                std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void VulkanGraphicsContext::createInstance() {
        CR_CORE_DEBUG("Creating Vulkan Instance");

        if (gEnableValidationLayers && !crCheckValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        const Ref<Car::Window> window = Application::Get()->getWindow();
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Car Application";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> extensions = getRequiredExtensions();

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (gEnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
            crPopulateDebugMessagerCreateInfo(&debugCreateInfo, nullptr);
            createInfo.pNext = static_cast<void*>(&debugCreateInfo);
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance");
        }

        if (!gladLoaderLoadVulkan(mInstance, nullptr, nullptr)) {
            throw std::runtime_error("Car: Failed to initialize glad");
        }
    }

    void VulkanGraphicsContext::setupDebugMessenger() {
        CR_CORE_DEBUG("Creating debug messager");

        if (!gEnableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        crPopulateDebugMessagerCreateInfo(&createInfo, nullptr);

        if (vkCreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to craete debug messager");
        }
    }

    void VulkanGraphicsContext::createSurface() {
        CR_CORE_DEBUG("creating Vulkan surface");

        if (glfwCreateWindowSurface(mInstance, mWindowHandle, nullptr, &mSurface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void VulkanGraphicsContext::pickPhysicalDevice() {
        CR_CORE_DEBUG("Picking physical device");
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        CR_CORE_DEBUG("GPU: {0}", crGetPhysicalDeviceName(physicalDevice));

        mPhysicalDevice = physicalDevice;

        if (!gladLoaderLoadVulkan(mInstance, mPhysicalDevice, nullptr)) {
            throw std::runtime_error("Car: Failed to initialize glad");
        }
    }

    void VulkanGraphicsContext::createLogicalDevice() {
        CR_CORE_DEBUG("Creating logical device");
        CrQueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(),
                                                  indices.transferFamily.value()};
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (gEnableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
        vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);
        vkGetDeviceQueue(mDevice, indices.transferFamily.value(), 0, &mTransferQueue);

        if (!gladLoaderLoadVulkan(mInstance, mPhysicalDevice, mDevice)) {
            throw std::runtime_error("Car: Failed to initialize glad");
        }
    }

    void VulkanGraphicsContext::createSwapChain() {
        CR_CORE_DEBUG("Creating Vulkan Swapchain");
        CrSwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = mSurface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        CrQueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value(),
                                         indices.transferFamily.value()};

        // indices.graphicsFamily != indices.transferFamily
        // indices.graphicsFamily != indices.presentFamily
        // indices.transferFamily != indices.presentFamily
        if (indices.graphicsFamily != indices.transferFamily || indices.graphicsFamily != indices.presentFamily ||
            indices.transferFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 3;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;     // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        mSwapChainImageFormat = surfaceFormat.format;
        mSwapChainExtent = extent;

        vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
        mSwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());
    }

    void VulkanGraphicsContext::createImageViews() {
        CR_CORE_DEBUG("creating vulkan images views");

        mSwapChainImageViews.resize(mSwapChainImages.size());

        for (size_t i = 0; i < mSwapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = mSwapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = mSwapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void VulkanGraphicsContext::createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = mSwapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanGraphicsContext::createFramebuffers() {
        CR_CORE_DEBUG("Creating vulkan framebuffers");

        mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

        for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
            VkImageView attachments[] = {mSwapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = mRenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = mSwapChainExtent.width;
            framebufferInfo.height = mSwapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanGraphicsContext::createCommandPool() {
        CR_CORE_DEBUG("Creating vulkan command pools");

        CrQueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice);

        VkCommandPoolCreateInfo renderPoolInfo{};
        renderPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        renderPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        renderPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

        VkCommandPoolCreateInfo transferPoolInfo{};
        transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        transferPoolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();

        if (vkCreateCommandPool(mDevice, &renderPoolInfo, nullptr, &mRenderCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render command pool!");
        }

        if (vkCreateCommandPool(mDevice, &transferPoolInfo, nullptr, &mTransferCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render command pool!");
        }
    }

    void VulkanGraphicsContext::createCommandBuffers() {
        CR_CORE_DEBUG("Creating vulkan command buffers");

        VkCommandBufferAllocateInfo renderAllocInfo{};
        renderAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        renderAllocInfo.commandPool = mRenderCommandPool;
        renderAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        renderAllocInfo.commandBufferCount = mMaxFramesInFlight;

        VkCommandBufferAllocateInfo transferAllocInfo{};
        transferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        transferAllocInfo.commandPool = mRenderCommandPool;
        transferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        transferAllocInfo.commandBufferCount = mMaxFramesInFlight;

        mRenderCommandBuffers.resize(mMaxFramesInFlight);
        mTransferCommandBuffers.resize(mMaxFramesInFlight);

        if (vkAllocateCommandBuffers(mDevice, &renderAllocInfo, mRenderCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate render command buffers!");
        }

        if (vkAllocateCommandBuffers(mDevice, &transferAllocInfo, mTransferCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate transfer command buffers!");
        }
    }

    void VulkanGraphicsContext::createSyncObjects() {
        CR_CORE_DEBUG("Creating vulkan sync objects");

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        mImageAvailableSemaphores.resize(mMaxFramesInFlight);
        mRenderFinishedSemaphores.resize(mMaxFramesInFlight);
        mInFlightFences.resize(mMaxFramesInFlight);

        for (size_t i = 0; i < mMaxFramesInFlight; i++) {
            if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create semaphores!");
            }
        }
    }

    void VulkanGraphicsContext::createDescriptorPool() {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = mMaxFramesInFlight;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = mMaxFramesInFlight;

        if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    VulkanGraphicsContext::~VulkanGraphicsContext() {
        vkDeviceWaitIdle(mDevice);

        vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);

        for (size_t i = 0; i < mMaxFramesInFlight; i++) {
            vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
        }

        vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

        vkDestroyCommandPool(mDevice, mRenderCommandPool, nullptr);
        vkDestroyCommandPool(mDevice, mTransferCommandPool, nullptr);

        cleanupSwapChain();

        vkDestroyDevice(mDevice, nullptr);

        vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

        if (gEnableValidationLayers) {
            vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugMessenger, nullptr);
        }

        vkDestroyInstance(mInstance, nullptr);

        gladLoaderUnloadVulkan();
        CR_CORE_DEBUG("Vulkan Context shutdown");
    }

    uint32_t VulkanGraphicsContext::aquireNextImageIndex() {
        VkResult result = vkAcquireNextImageKHR(mDevice, mSwapChain, UINT64_MAX, getCurrentImageAvailableSemaphore(),
                                                VK_NULL_HANDLE, &mImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            // dont really know what i think about this.
            // Recreating a semaphore so it can be not signlated is kind of sketchy.
            vkDestroySemaphore(mDevice, getCurrentImageAvailableSemaphore(), nullptr);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[mCurrentFrame]) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create semaphore!");
            }

            recreateSwapchain();
            return aquireNextImageIndex();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        return mImageIndex;
    }

    void VulkanGraphicsContext::swapBuffers() {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mRenderCommandBuffers[mCurrentFrame];

        VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {mSwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &mImageIndex;

        vkQueuePresentKHR(mPresentQueue, &presentInfo);

        mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
    }

    void VulkanGraphicsContext::cleanupSwapChain() {
        for (size_t i = 0; i < mSwapChainFramebuffers.size(); i++) {
            vkDestroyFramebuffer(mDevice, mSwapChainFramebuffers[i], nullptr);
        }

        for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
            vkDestroyImageView(mDevice, mSwapChainImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
    }

    void VulkanGraphicsContext::recreateSwapchain() {
        vkDeviceWaitIdle(mDevice);

        cleanupSwapChain();

        createSwapChain();
        createImageViews();
        createFramebuffers();
    }

    void VulkanGraphicsContext::resize(uint32_t, uint32_t) { recreateSwapchain(); }

    uint32_t VulkanGraphicsContext::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    // TODO: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    // or my own heap implementation maybe
    void VulkanGraphicsContext::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                                             VkMemoryPropertyFlags properties, VkBuffer* pBuffer,
                                             VkDeviceMemory* pBufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, pBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create a buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(mDevice, *pBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        // TODO: A custom heap implementation
        if (vkAllocateMemory(mDevice, &allocInfo, nullptr, pBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
    }

    // TODO: We have command buffers allocated already so we dont need to craete them on the fly
    void VulkanGraphicsContext::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                                           VkDeviceSize srcOffset /*=0*/, VkDeviceSize dstOffset /*=0*/) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = mTransferCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = srcOffset;
        copyRegion.dstOffset = dstOffset;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(mTransferQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(mTransferQueue);

        vkFreeCommandBuffers(mDevice, mTransferCommandPool, 1, &commandBuffer);
    }

    Ref<GraphicsContext> GraphicsContext::Create(GLFWwindow* windowHandle) {
        Ref<GraphicsContext> context = createRef<VulkanGraphicsContext>(windowHandle);

        GraphicsContext::Set(context);

        return context;
    }
} // namespace Car