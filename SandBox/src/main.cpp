#include "Car/Application.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/Renderer/VertexArray.hpp"
#define CR_ENTRY
#include <Car/Car.hpp>

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class SandboxLayer : public Car::Layer {
public:
    SandboxLayer() : Car::Layer("Sandbox Layer") {}

    virtual void onAttach() override {
        Car::Renderer::EnableBlending();
        Car::Renderer::ClearColor(0.1f, 0.0f);

        std::vector<float> vertices = {
            -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        };

        std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

        Car::ShaderLayoutInput layout = {
            {"iPos", Car::ShaderLayoutInput::DataType::Float2},
            {"iColor", Car::ShaderLayoutInput::DataType::Float3},
            {"iUv", Car::ShaderLayoutInput::DataType::Float2},
        };

        mUb = Car::UniformBuffer::Create(sizeof(UniformBufferObject), 0);

        auto Vb = Car::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(vertices[0]), Car::Buffer::Usage::StaticDraw);
        auto Ib = Car::IndexBuffer::Create(indices.data(), indices.size() * sizeof(indices[0]),
                                           Car::Buffer::Usage::StaticDraw, Car::Buffer::Type::UnsignedInt);
        auto shader = Car::Shader::Create("sandbox.vert", "sandbox.frag", layout);

        mTexture = Car::Texture2D::Create("resources/images/pg_np.png");
        
        shader->setInput(0, 0, true, mUb);
        shader->setInput(1, 1, true, mTexture);

        mVa = Car::VertexArray::Create(Vb, Ib, shader);
    }

    virtual void onImGuiRender(double dt) override {
        ImGui::Begin("Performance");

        ImGui::Text("[FPS]: %f", 1 / dt);

        ImGui::End();
    }

    virtual void onRender() override {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto window = Car::Application::Get()->getWindow();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj =
            glm::perspective(glm::radians(45.0f), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 10.0f);

        ubo.proj[1][1] *= -1;

        mUb->setData(&ubo);

        Car::Renderer::DrawTriangles(mVa, 2);
    }

private:
    Car::Ref<Car::VertexArray> mVa;
    Car::Ref<Car::UniformBuffer> mUb;
    Car::Ref<Car::Texture2D> mTexture;
};

class Sandbox : public Car::Application {
public:
    Sandbox() {
        mSandboxLayer = new SandboxLayer();
        pushLayer(mSandboxLayer);
    }

    virtual void onRender() override { Car::Renderer::Clear(); }

    virtual ~Sandbox() override {}

private:
    SandboxLayer* mSandboxLayer;
};

Car::Application* Car::createApplication() {
    Car::Application::Specification spec{};
    spec.width = 1280;
    spec.height = 720;
    spec.title = "Sandbox";
    spec.vsync = true;
    spec.resizable = true;
    spec.useImGui = false;
    Car::Application::SetSpecification(spec);

    return new Sandbox();
}
