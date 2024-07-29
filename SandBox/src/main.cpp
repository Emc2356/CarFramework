#include "Car/Renderer/VertexArray.hpp"
#include "GLFW/glfw3.h"
#include <Car/Car>

class SandboxLayer : public Car::Layer {
public:
    SandboxLayer() : Car::Layer("Sandbox Layer") {}

    virtual void onAttach() override {
        // Car::Renderer::EnableBlending();

        // mTexture = Car::ResourceManager::LoadTexture2D("pg_np.png");
        // mFont = Car::ResourceManager::LoadFont("zed-mono-regular.ttf", 50);
        // std::vector<float> vertices = {
        //     -0.5,  0.5 , 0.0,  1.0, // top left
        //     0.5 ,  0.5 , 1.0,  1.0, // top right
        //     0.5 ,  -0.5,1.0, 0.0, // bottom right
        //     -0.5,-0.5,0.0, 0.0, // bottom left
        // };
    
        std::vector<float> vertices = {
            0.0f, -0.5f, 1.0f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
        };    
        
        Car::BufferLayout layout = {
            {"inPosition", Car::BufferLayout::DataType::Float2},
            {"inColor", Car::BufferLayout::DataType::Float3},
        }; 
        
        Car::Ref<Car::VertexBuffer> vbo = Car::VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(vertices[0]), layout, Car::Buffer::Usage::StaticDraw);

        std::vector<uint32_t> indices = {
            0, 1, 2,
            2, 3, 0
        };
        Car::Ref<Car::IndexBuffer> ibo = Car::IndexBuffer::Create(indices.data(), indices.size(), Car::Buffer::Usage::StaticDraw, Car::Buffer::Type::UnsignedInt);

        mShader = Car::Shader::Create("temp/spv/vertex.spv", "temp/spv/fragmeant.spv");

        mVao = Car::VertexArray::Create(vbo, ibo, mShader);
    }

    virtual void onImGuiRender(double dt) override {
        ImGui::Begin("Performance");

        ImGui::Text("[FPS]: %f", 1 / dt);

        ImGui::End();
    }

    virtual void onRender() override {
        Car::Renderer::DrawTriangles(mVao);
        // Car::Renderer2D::DrawRect({Car::Input::MousePos(), {256, 256}}, {1.0f, 0.0f, 0.0f});
        // Car::Renderer2D::DrawTexture(mTexture, Car::Input::MousePos());
        // Car::Renderer2D::DrawTexture(mTexture, {256, 256});

        // Car::Renderer2D::DrawText(mFont, "{Hello World!~\n{From too much work\nScore: {1.4}", {50, 50}, {.7, .7, .7});
    }

private:
    // Car::Ref<Car::Texture2D> mTexture;
    // Car::Ref<Car::Font> mFont;
    Car::Ref<Car::Shader> mShader;
    Car::Ref<Car::VertexArray> mVao;
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
