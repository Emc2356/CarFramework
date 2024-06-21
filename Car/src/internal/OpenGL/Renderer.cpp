#include "Car/internal/OpenGL/Renderer.hpp"
#include "Car/OrthographicCamera.hpp"
#include "Car/Renderer/UniformBuffer.hpp"
#include "Car/internal/OpenGL/VertexBuffer.hpp"
#include "Car/internal/OpenGL/IndexBuffer.hpp"

#include <glad/gl.h>


namespace Car {
    Renderer* Renderer::sInstance = (Renderer*)new OpenGLRenderer();

    void OpenGLRenderer::InitImpl() {
        mCarUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 0, Buffer::Usage::DynamicDraw);
        CR_CORE_TRACE("Renderer Initialized");
    }

    void OpenGLRenderer::ClearColorImpl(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void OpenGLRenderer::ClearImpl() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLRenderer::BeginSceneImpl(const Car::OrthographicCamera& camera) {
        mCarUniformBuffer->setData(&(camera.getViewProjectionMatrix())[0][0], 0);
    }
    
    void OpenGLRenderer::EndSceneImpl() {
        // future defered renderer :/
    }

    void OpenGLRenderer::EnableBlendingImpl() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGLRenderer::DrawTrianglesImpl(const Ref<VertexArray> va) {
        va->bind();

        glDrawElements(GL_TRIANGLES, va->getIndexBuffer()->getCount(), ((OpenGLIndexBuffer*)(va->getIndexBuffer().get()))->getOpenGLType(), nullptr);
    }
}
