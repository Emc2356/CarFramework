#include <Car/Car>

#include <glad/gl.h>

// glEnable
// glBlendFunc
// glBlendFuncSeparate
// glBlendEquation
// glBlendEquationSeparate
/*

glEnable(GL_BLEND);
glBlendEquation(GL_FUNC_ADD);
glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
glDisable(GL_CULL_FACE);
glDisable(GL_DEPTH_TEST);
glDisable(GL_STENCIL_TEST);
glEnable(GL_SCISSOR_TEST);
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_PRIMITIVE_RESTART
if (bd->GlVersion >= 310)
    glDisable(GL_PRIMITIVE_RESTART);
#endif
#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_POLYGON_MODE
if (bd->HasPolygonMode)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif*/
// class RendererState {
// public:
// enum class AlphaEquation;
// public:
//     struct Specification {
        
//     };
// public:
//     void enableBlending(AlphaEquation);
// };


class SandboxLayer : public Car::Layer {
public:
    SandboxLayer() : Car::Layer("Sandbox Layer") {}
    
    virtual void onAttach() override {
        Car::Renderer::EnableBlending();
        
        mTexture = Car::ResourceManager::LoadTexture2D("pg_np.png");
        mFont = Car::ResourceManager::LoadFont("zed-mono-regular.ttf", 50);
    }
    
    virtual void onImGuiRender(double dt) override {
        ImGui::Begin("Performance");
        
        ImGui::Text("[FPS]: %f", 1/dt);
        
        ImGui::End();   
    }
    
    virtual void onRender() override {        
        // Car::Renderer2D::DrawRect({Car::Input::MousePos(), {256, 256}}, {1.0f, 0.0f, 0.0f});
        Car::Renderer2D::DrawTexture(mTexture, Car::Input::MousePos());
        Car::Renderer2D::DrawTexture(mTexture, {256, 256});
        
        Car::Renderer2D::DrawText(mFont, "{Hello World!~\n{From too much work\nScore: {1.4}", {50, 50}, {.7, .7, .7});
    }
    
private:
    Car::Ref<Car::Texture2D> mTexture;
    Car::Ref<Car::Font> mFont;
};


class Sandbox : public Car::Application {
public:
    Sandbox() {
        mSandboxLayer = new SandboxLayer();
        pushLayer(mSandboxLayer);
    }
    
    virtual void onRender() override {
        Car::Renderer::ClearColor(0.1f);
        Car::Renderer::Clear();        
    }

    virtual ~Sandbox() override {}
private:
    SandboxLayer* mSandboxLayer;
};

Car::Application *Car::createApplication() {
    Car::Application::Specification spec{};
    spec.width = 1280;
    spec.height = 720;
    spec.title = "Sandbox";
    spec.vsync = true;
    spec.resizable = true;
    spec.useImGui = true;
    Car::Application::SetSpecification(spec);
    
    return new Sandbox();
}
