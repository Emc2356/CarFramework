#include "Car/Core/Core.hpp"

#include "Car/Layers/ImGuiLayer.hpp"
#include "Car/Application.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <Car/ext/ImGui/imgui_impl_car.hpp>
#if defined(CR_OPENGL)
#include <backends/imgui_impl_opengl3.h>
#elif defined(CR_VULKAN)
#include <backends/imgui_impl_vulkan.h>
#else
#error only opengl and vulkan are supported at this time
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
// Setup Platform/Renderer bindings
#if defined(CR_OPENGL)
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        // ImGui_ImplCar_Init();
        ImGui_ImplOpenGL3_Init("#version 410");
#elif defined(CR_VULKAN)

#else
#error only opengl and vulkan is supported right now
#endif
    }

    void ImGuiLayer::onDetach() {
#if defined(CR_OPENGL)
        // ImGui_ImplCar_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
#elif defined(CR_VULKAN)

#else
#error only opengl and vulkan is supported right now
#endif
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

#if defined(CR_OPENGL)
        ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplCar_NewFrame();
#elif defined(CR_VULKAN)

#else
#error only opengl and vulkan is supported right now
#endif
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::end() {
        ImGuiIO& io = ImGui::GetIO();

        // Rendering
        ImGui::Render();
#if defined(CR_OPENGL)
        // ImGui_ImplCar_RenderDrawData(ImGui::GetDrawData());
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#elif defined(CR_VULKAN)
#else
#error only opengl and vulkan is supported right now
#endif

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }
} // namespace Car
