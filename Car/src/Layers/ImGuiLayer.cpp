#include "Car/Core/Core.hpp"

#include "Car/Layers/ImGuiLayer.hpp"
#include "Car/Application.hpp"
#include "Car/Core/Ref.hpp"
#include "Car/Renderer/GraphicsContext.hpp"
#include "Car/internal/Vulkan/GraphicsContext.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <Car/ext/ImGui/imgui_impl_car.hpp>
#if defined(CR_VULKAN)
// #include <backends/imgui_impl_vulkan.h>
#else
#error only vulkan is supported
#endif

#include <GLFW/glfw3.h>

namespace Car {
    ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer") {}

    void ImGuiLayer::onAttach() {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable
        // Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
        // for now i disabled multiple viewports as it doesnt look and feel that
        // good in linux maybe in the future if i figure out wtf is causing it to
        // look so bad (looking at you plasma KDE) i will enable it io.ConfigFlags
        // |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport /
        // Platform Windows io.ConfigFlags |=
        // ImGuiConfigFlags_ViewportsNoTaskBarIcons; io.ConfigFlags |=
        // ImGuiConfigFlags_ViewportsNoMerge;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsClassic();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform
        // windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        const Application* app = Car::Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app->getWindow()->getWindowHandle());
        UNUSED(window);

        ImGui_ImplGlfw_InitForVulkan(window, true);
        auto graphicsContext = reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get());
        // ImGui_ImplVulkan_InitInfo info{};
        // info.Instance = graphicsContext->getInstance();
        // info.PhysicalDevice = graphicsContext->getPhysicalDevice();
        // info.Device = graphicsContext->getDevice();
        // info.QueueFamily = graphicsContext->findQueueFamilies(graphicsContext->getPhysicalDevice()).graphicsFamily.value();
        // info.Queue = graphicsContext->getGraphicsQueue();
        // info.DescriptorPool = graphicsContext->getDescriptorPool();
        // info.RenderPass = graphicsContext->getRenderPass();
        // info.MinImageCount = 3;
        // info.ImageCount = 3;
        // info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        // ImGui_ImplVulkan_Init(&info);
    }

    void ImGuiLayer::onDetach() {
        // ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent&) { return mWantCaptureMouse; }

    bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent&) { return mWantCaptureMouse; }

    bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent&) { return mWantCaptureMouse; }

    bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent&) { return mWantCaptureMouse; }

    bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent&) { return mWantCaptureKeyboard | mWantTextInput; }

    bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent&) { return mWantCaptureKeyboard | mWantTextInput; }

    bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent&) { return mWantCaptureKeyboard | mWantTextInput; }

    void ImGuiLayer::begin() {
        ImGuiIO& io = ImGui::GetIO();

        mWantCaptureMouse = io.WantCaptureMouse;
        mWantCaptureKeyboard = io.WantCaptureKeyboard;
        mWantTextInput = io.WantTextInput;

        // ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::end() {
        ImGuiIO& io = ImGui::GetIO();

        // Rendering
        ImGui::Render();
        // ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), reinterpretCastRef<VulkanGraphicsContext>(GraphicsContext::Get())->getCurrentRenderCommandBuffer());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
} // namespace Car
