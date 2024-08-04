#define CR_ENTRY
#include <Car/Car.hpp>

class RayCastingLayer : public Car::Layer {
public:
    RayCastingLayer() : Car::Layer("Sandbox Layer") {}

    virtual void onAttach() override {
        Car::Renderer::EnableBlending();
        Car::Renderer::ClearColor(0.1f);

        mFont = Car::ResourceManager::LoadFont("zed-mono-regular.ttf", 50);
    }

    virtual void onImGuiRender(double dt) override {
        ImGui::Begin("Performance");

        ImGui::Text("[FPS]: %f", 1 / dt);

        ImGui::End();
    }

    virtual void onRender() override {
        // Car::Renderer2D::DrawText(mFont, "Hello\nWorld", Car::Input::MousePos());
        // glm::ivec2 screenSize(Car::Application::Get()->getWindow()->getWidth(),
        // Car::Application::Get()->getWindow()->getHeight());

        // glm::ivec2 mousePos = Car::Input::MousePos();
        glm::ivec2 mousePos(Car::Application::Get()->getWindow()->getWidth() / 2,
                            Car::Application::Get()->getWindow()->getHeight() / 2);

        float radius = 256.0f;
        for (uint32_t i = 0; i < 360; i++) {
            glm::ivec2 pos{glm::cos(glm::radians(float(i))) * radius, glm::sin(glm::radians(float(i))) * radius};
            pos += mousePos;

            Car::Renderer2D::DrawLine(mousePos, pos, {1.0f, 1.0f, 1.0f});
            // Car::Renderer2D::DrawPoint(pos, {1.0f, 0.0f, 0.0f});
        }

        // Car::Renderer2D::DrawLine(mousePos, mousePos + glm::ivec2(50, -100)*3);

        // Car::Renderer2D::DrawLine(screenSize / 2, Car::Input::MousePos(), {1.0f, 1.0f, 1.0f});
        // Car::Renderer2D::DrawRect({Car::Input::MousePos(), {256, 256}}, {1.0f, 0.0f, 0.0f});
        // Car::Renderer2D::DrawTexture(mTexture, Car::Input::MousePos());
        // Car::Renderer2D::DrawTexture(mTexture, {256, 256});

        Car::Renderer2D::DrawText(mFont, "{Hello World!~\n{From too much work\nScore: {1.4}", {50, 50}, glm::vec3(.7f));
    }

private:
    Car::Ref<Car::Texture2D> mTexture;
    Car::Ref<Car::Font> mFont;
};

class Sandbox : public Car::Application {
public:
    Sandbox() {
        mSandboxLayer = new RayCastingLayer();
        pushLayer(mSandboxLayer);
    }

    virtual void onRender() override { Car::Renderer::Clear(); }

    virtual ~Sandbox() override {}

private:
    RayCastingLayer* mSandboxLayer;
};

Car::Application* Car::createApplication() {
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
