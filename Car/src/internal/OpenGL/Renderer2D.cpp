#include "Car/internal/OpenGL/Renderer2D.hpp"
#include "Car/Core/Log.hpp"
#include "Car/OrthographicCamera.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/internal/OpenGL/VertexBuffer.hpp"
#include "Car/internal/OpenGL/IndexBuffer.hpp"

#include <glad/gl.h>
 

namespace Car {
    Renderer2D* Renderer2D::sInstance = new OpenGLRenderer2D();

    void OpenGLRenderer2D::InitImpl() {
        mCarUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 0, Buffer::Usage::DynamicDraw);
        CR_CORE_TRACE("Renderer Initialized");
    }

    void OpenGLRenderer2D::ClearColorImpl(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
 
    void OpenGLRenderer2D::ClearImpl() {
        glClear(GL_COLOR_BUFFER_BIT);
    } 

    void OpenGLRenderer2D::BeginSceneImpl(const Car::OrthographicCamera& camera) {
        mCarUniformBuffer->setData(&(camera.getViewProjectionMatrix())[0][0], 0);
    }
    
    void OpenGLRenderer2D::EndSceneImpl() {
        // future defered renderer :/
    }

    void OpenGLRenderer2D::DrawSceneImpl(Ref<Scene> scene) {
        UNUSED(scene);

        CR_CORE_INFO("HELLLLLLLLLLLLOOOOOOO");
    }

    void OpenGLRenderer2D::EnableBlendingImpl() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRenderer2D::DrawTrianglesImpl(const Ref<VertexArray> va) {
        va->bind();

        glDrawElements(GL_TRIANGLES, va->getIndexBuffer()->getCount(), ((OpenGLIndexBuffer*)(va->getIndexBuffer().get()))->getOpenGLType(), nullptr);
    }
}
