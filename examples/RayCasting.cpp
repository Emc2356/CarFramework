#define CR_ENTRY
#include <Car/Car.hpp>

class RayCastingLayer : public Car::Layer {
public:
    struct Wall {
        glm::vec2 start;
        glm::vec2 end;
    };

public:
    RayCastingLayer() : Car::Layer("RayCasting Layer") {}

    virtual void onAttach() override {
        Car::Renderer::EnableBlending();
        Car::Renderer::ClearColor(0.1f);

        mWallCount = 5;
        resetWalls();
    }

    virtual void onImGuiRender(double dt) override {
        ImGui::Begin("Performance");
        ImGui::Text("[FPS]: %f", 1 / dt);
        ImGui::End();

        ImGui::Begin("Settings");

        if (ImGui::Button("Recreate walls")) {
            resetWalls();
        }

        if (ImGui::SliderInt("Wall count", &mWallCount, 1, 50)) {
            resetWalls();
        }

        ImGui::End();
    }

    void resetWalls() {
        mWalls.clear();
        glm::ivec2 screenSize(Car::Application::Get()->getWindow()->getWidth(),
                              Car::Application::Get()->getWindow()->getHeight());
        for (int32_t i = 0; i < mWallCount; i++) {
            mWalls.push_back({{(float)Car::Random::UInt(0, screenSize.x), (float)Car::Random::UInt(0, screenSize.y)},
                              {(float)Car::Random::UInt(0, screenSize.x), (float)Car::Random::UInt(0, screenSize.y)}});
        }
    }

    virtual void onRender() override {
        for (const auto& wall : mWalls) {
            Car::Renderer2D::DrawLine(wall.start, wall.end);
        }

        glm::ivec2 mousePos_ = Car::Input::MousePos();
        glm::vec2 mousePos = {mousePos_.x, mousePos_.y};
        
        for (float i = 0; i < 360.0f; i += 1.0f) {
            glm::vec2 ray = glm::vec2(glm::cos(glm::radians(i)), glm::sin(glm::radians(i)));

            float x3 = mousePos.x;
            float y3 = mousePos.y;
            float x4 = x3 + ray.x * mMaxDistance;
            float y4 = y3 + ray.y * mMaxDistance;
            float record = FLT_MAX;
            glm::vec2 closePoint;
            for (const auto& wall : mWalls) {
                float x1 = wall.start.x;
                float y1 = wall.start.y;
                float x2 = wall.end.x;
                float y2 = wall.end.y;

                float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

                if (den == 0) { // line are parallel and they will never meet even if you stretch them out infinitely
                    continue;
                }

                float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
                float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

                if (0 <= t && t <= 1 && 0 <= u) {
                    float dis = glm::distance(mousePos, {x1 + t * (x2 - x1), y1 + t * (y2 - y1)});
                    if (dis < record) {
                        record = dis;
                        closePoint.x = x1 + t * (x2 - x1);
                        closePoint.y = y1 + t * (y2 - y1);
                    }
                }
            }
            if (record < FLT_MAX) {
                Car::Renderer2D::DrawLine(mousePos, closePoint, {0.0f, 1.0f, 0.f});
            }
        }
    }

private:
    std::vector<Wall> mWalls;
    int32_t mWallCount;
    float mMaxDistance = 500;
};

class RayCastingApplication : public Car::Application {
public:
    RayCastingApplication() {
        mSandboxLayer = new RayCastingLayer();
        pushLayer(mSandboxLayer);
    }
    virtual ~RayCastingApplication() override {}

    virtual void onRender() override { Car::Renderer::Clear(); }

private:
    RayCastingLayer* mSandboxLayer;
};

Car::Application* Car::createApplication() {
    Car::Application::Specification spec{};
    spec.width = 1280;
    spec.height = 720;
    spec.title = "Ray Casting";
    spec.vsync = false;
    spec.resizable = true;
    spec.useImGui = true;
    Car::Application::SetSpecification(spec);

    return new RayCastingApplication();
}
