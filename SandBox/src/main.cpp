#include "Car/Events/MouseEvent.hpp"
#include "Car/Layers/Layer.hpp"
#include <Car/Car>


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
        Car::Renderer::ClearColor(0.1f);
        Car::Renderer::Clear();
 
        Car::Renderer2D::Begin();
        
        Car::Renderer2D::DrawTexture(mTexture, Car::Input::MousePos());
        Car::Renderer2D::DrawTexture(mTexture, {256, 256});
        
        Car::Renderer2D::DrawText(mFont, "{Hello World!~\n{From too much work\nScore: {1.4}", {50, 50}, {.7, .7, .7});
        
        Car::Renderer2D::End();
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

    virtual ~Sandbox() override {}
private:
    SandboxLayer* mSandboxLayer;
};

Car::Application *Car::createApplication() {
    Car::Application::Specification spec;
    spec.width = 1280;
    spec.height = 720;
    spec.title = "Sandbox";
    spec.vsync = true;
    spec.resizable = true;
    Car::Application::SetSpecification(spec);
    
    return new Sandbox();
}
