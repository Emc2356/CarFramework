#include <Car/Car>

#include <GLFW/glfw3.h>
#include <glad/gl.h>



class Sandbox : public Car::Application {
public:
    Sandbox() : mCamera(16.0f/9.0f, 2) {
        Car::Renderer::EnableBlending();

        std::vector<float> vertices = {
            -0.5,  0.5 , 0.0,  1.0, // top left
            0.5 ,  0.5 , 1.0,  1.0, // top right
            0.5 ,  -0.5,1.0, 0.0, // bottom right
            -0.5,-0.5,0.0, 0.0, // bottom left
        };
        Car::Ref<Car::VertexBuffer> vbo = Car::VertexBuffer::Create(vertices.data(), vertices.size(), Car::Buffer::Usage::StaticDraw, Car::Buffer::Type::Float);

        Car::BufferLayout layout = {
            {"iPos", Car::BufferLayout::DataType::Float2},
            {"iUv", Car::BufferLayout::DataType::Float2},
        }; 

        vbo->setLayout(layout); 

        std::vector<uint32_t> indices = {
            0, 1, 2,
            2, 3, 0
        };
        Car::Ref<Car::IndexBuffer> ibo = Car::IndexBuffer::Create(indices.data(), indices.size(), Car::Buffer::Usage::StaticDraw, Car::Buffer::Type::UnsignedInt);

        mShader = Car::Shader::Create("texture.vert", "texture.frag");

        mVao = Car::VertexArray::Create(vbo, ibo, mShader);

        mTexture = Car::Texture2D::Create("resources/images/pg_np.png", true);
    }

    virtual ~Sandbox() override {} 

    virtual bool onWindowCloseEvent(Car::WindowCloseEvent&) override { isRunning = false; return true; }

    virtual void onImGuiRender(double dt) override {
        bool demo = true;
        ImGui::ShowDemoWindow(&demo);
        ImGui::Begin("Time");
        ImGui::Text("dt=%lf", dt);
        ImGui::End();
    }

    virtual void onUpdate(double dt) override {
        if (Car::Input::IsKeyPressed(CR_KEY_LEFT)) {
            mRotation += 360 * dt;
        } else if (Car::Input::IsKeyPressed(CR_KEY_RIGHT)) {
            mRotation -= 360 * dt;
        }

        if (Car::Input::IsKeyPressed(CR_KEY_A)) {
            mPosition.x += 2 * dt;
        } else if (Car::Input::IsKeyPressed(CR_KEY_D)) {
            mPosition.x -= 2 * dt;
        }
        if (Car::Input::IsKeyPressed(CR_KEY_W)) {
            mPosition.y -= 2 * dt;
        } else if (Car::Input::IsKeyPressed(CR_KEY_S)) {
            mPosition.y += 2 * dt;
        }
    }

    void onRender() override {
        mCamera.setPosition(mPosition);
        mCamera.setRotation(mRotation);
        Car::Renderer::BeginScene(mCamera);

        Car::Renderer::ClearColor(0x1C1C1CFFu);
        Car::Renderer::Clear();

        mTexture->bind(5);

        Car::Renderer::DrawTriangles(mVao);


        Car::Renderer::EndScene();
    }
private:
    Car::Ref<Car::VertexArray> mVao;
    Car::Ref<Car::Shader> mShader;
    Car::OrthographicCamera mCamera;
    Car::Ref<Car::Texture2D> mTexture;
    float mRotation = 0;
    glm::vec3 mPosition = glm::vec3(0.0f);
};

Car::Application *Car::createApplication() { return new Sandbox(); }
