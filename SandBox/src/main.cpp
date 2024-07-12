#include <Car/Car>


class Sandbox : public Car::Application {
public:
    Sandbox() {
        Car::Renderer::EnableBlending();

        mTexture = Car::Texture2D::Create("resources/images/pg_np.png");
    }

    virtual ~Sandbox() override {} 

    virtual bool onWindowCloseEvent(Car::WindowCloseEvent&) override { isRunning = false; return true; }

    virtual void onImGuiRender(double dt) override {
        UNUSED(dt);
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
        
        // Car::Rect rect = {512, 512, 96, 96};
        // for (uint32_t i = 0; i < 19998; i++) {
        //     Car::Renderer2D::DrawTexture(mTexture, rect);
        // }
        
        Car::Renderer2D::End();
    }
private:
    Car::Ref<Car::Texture2D> mTexture;
};

Car::Application *Car::createApplication() {
    return new Sandbox();
}
