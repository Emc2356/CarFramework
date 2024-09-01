#pragma once

// include vulkan first so glfw knows to define vulkan functions
#include <glad/vulkan.h>
#include <GLFW/glfw3.h>

#include "Car/Core/Core.hpp"

#include "Car/Events/Event.hpp"
#include "Car/Renderer/GraphicsContext.hpp"

namespace Car {
    class Window {
    public:
        using eventCallbackFn = std::function<void(Car::Event&)>;

        struct Specification {
            uint32_t width;
            uint32_t height;
            std::string title;
            bool resizable;
            eventCallbackFn eventCallback;
        };

    public:
        Window(const Window::Specification&);
        ~Window();

        static Window* Get();

        void init();

        void onUpdate();

        uint32_t getWidth() const { return mSpec.width; }
        uint32_t getHeight() const { return mSpec.height; }
        float getAspectRation() const { return (float)mSpec.width / (float)mSpec.height; }
        const std::string getTitle() const { return mSpec.title; }

        GLFWwindow* getWindowHandle() const { return mHandle; }

        VkResult createVulkanSurface(VkInstance instance, const VkAllocationCallbacks* allocator,
                                     VkSurfaceKHR* surface);

        const Ref<GraphicsContext> getGraphicsContext() const { return mGraphicsContext; }

        // Vulkan related function
        std::vector<const char*> getRequiredInstanceExtensions() const;

    private:
        Specification mSpec;
        GLFWwindow* mHandle;
        Ref<GraphicsContext> mGraphicsContext;
    };
} // namespace Car