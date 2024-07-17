#include "Car/Core/Core.hpp"

#include "Car/Layers/ImGuiLayer.hpp"
#include "Car/Application.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <Car/ext/ImGui/imgui_impl_car.hpp>
#if defined(CR_OPENGL)
#include <backends/imgui_impl_opengl3.h>
#else
#error only opengl supported right now
#endif

#include <GLFW/glfw3.h>


namespace Car {
    ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer") {}

    void ImGuiLayer::onAttach() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		// for now i disabled multiple viewports as it doesnt look and feel that good in linux
		// maybe in the future if i figure out wtf is causing it to look so bad (looking at you plasma KDE)
		// i will enable it
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		const Application* app = Car::Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app->getWindow()->getWindowHandle());

		// Setup Platform/Renderer bindings
		#if defined(CR_OPENGL)
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 410");
		#else
		#error only opengl is supported right now
		#endif
    }

    void ImGuiLayer::onDetach() {
		#if defined(CR_OPENGL)
			ImGui_ImplOpenGL3_Shutdown();
		#else
		#error only opengl is supported right now
		#endif
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
    }
    
    bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& event) {
        UNUSED(event);
        return mWantCaptureMouse;
    }
    
    bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& event) {
        UNUSED(event);
        return mWantCaptureMouse;
    }
    
    bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& event) {
        UNUSED(event);
        return mWantCaptureMouse;
    }
    
    bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& event) {
        UNUSED(event);
        return mWantCaptureMouse;
    }
    
    bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& event) {
        UNUSED(event);
        return mWantCaptureKeyboard | mWantTextInput;
    }
    
    bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& event) {
        UNUSED(event);
        return mWantCaptureKeyboard | mWantTextInput;
    }
    
    bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& event) {
        UNUSED(event);
        return mWantCaptureKeyboard | mWantTextInput;
    }
    
    void ImGuiLayer::begin() {
        ImGuiIO& io = ImGui::GetIO();
        
        mWantCaptureMouse = io.WantCaptureMouse;
        mWantCaptureKeyboard = io.WantCaptureKeyboard;
        mWantTextInput = io.WantTextInput;
        
		#if defined(CR_OPENGL)
        	ImGui_ImplOpenGL3_NewFrame();
		#else
		    #error only opengl is supported right now
		#endif
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
    }

    void ImGuiLayer::end() {
		ImGuiIO& io = ImGui::GetIO();

		// Rendering
		ImGui::Render();
		#if defined(CR_OPENGL)
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		#else
		#error only opengl is supported right now
		#endif

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
    }
}
