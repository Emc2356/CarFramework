#include <Car/Car>


class Sandbox : public Car::Application {
public:
    Sandbox() {
        Car::Renderer::EnableBlending();

        mTexture = Car::ResourceManager::LoadTexture2D("pg_np.png");
        mFont = Car::ResourceManager::LoadFont("zed-mono-regular.ttf", 50);
    }

    virtual ~Sandbox() override {} 
    
    virtual void onImGuiRender(double dt) override {
        ImGui::Begin("Performance");
        
        ImGui::Text("[FPS]: %f", 1/dt);
        
        ImGui::End();   
    }

    void onRender() override {
        Car::Renderer::ClearColor(0.1f);
        Car::Renderer::Clear();
 
        Car::Renderer2D::Begin();
        
        auto [mX, mY] = Car::Input::MousePos();
        Car::Renderer2D::DrawTexture(mTexture, {mX , mY , 96, 96});
        Car::Renderer2D::DrawTexture(mTexture, {256, 256, 96, 96});
        
        Car::Renderer2D::DrawText(mFont, "Hello World!-\nFrom too much work", {50, 50});
        
        // Car::Rect rect = {512, 512, 96, 96};
        // int8_t textureID = Car::Renderer2D::getTextureID(mTexture);
        // for (uint32_t i = 0; i < 19998; i++) {
        //     Car::Renderer2D::DrawTextureFromID(rect, textureID);
        // }
        
        Car::Renderer2D::End();
    }
private:
    Car::Ref<Car::Texture2D> mTexture;
    Car::Ref<Car::Font> mFont;
};

Car::Application *Car::createApplication() {
    Car::Application::Specification spec;
    spec.width = 1280;
    spec.height = 720;
    spec.title = "Sandbox";
    spec.vsync = false;
    spec.resizable = true;
    Car::Application::SetSpecification(spec);
    
    return new Sandbox();
}
