#include "Car/internal/OpenGL/Renderer.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Renderer/VertexArray.hpp"
#include "Car/internal/OpenGL/IndexBuffer.hpp"

#include <glad/gl.h>


namespace Car {
    Renderer* Renderer::sInstance = new OpenGLRenderer();

    void OpenGLRenderer::InitImpl() {
    }

    void OpenGLRenderer::ShutdownImpl() {
        
    }
    
    void OpenGLRenderer::ClearColorImpl(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
 
    void OpenGLRenderer::ClearImpl() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } 

    void OpenGLRenderer::EnableBlendingImpl() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    void OpenGLRenderer::DisableBlendingImpl() {
        glDisable(GL_BLEND);
    }
    
    void OpenGLRenderer::EnableDepthTestImpl() {
        glEnable(GL_DEPTH_TEST);
    }
    
    void OpenGLRenderer::DisableDepthTestImpl() {
        glDisable(GL_DEPTH_TEST);
    }
    
    void OpenGLRenderer::DrawTrianglesImpl(const Ref<VertexArray> va, uint32_t triangleCount) {
        if (triangleCount > va->getIndexBuffer()->getCount() * 3) {
            CR_CORE_ERROR("Car::Renderer::DrawTriangles(VertexArray, triangleCount={}) index buffer doesnt have enough items", triangleCount);
            return;
        }
        va->bind();

        glDrawElements(
            GL_TRIANGLES,
            triangleCount * 3,
            ((OpenGLIndexBuffer*)(va->getIndexBuffer().get()))->getOpenGLType(),
            nullptr
        );
    }

    void OpenGLRenderer::DrawTrianglesImpl(const Ref<VertexArray> va) {
        if (va->getIndexBuffer()->getCount() % 3 != 0) {
            CR_CORE_ERROR("Car::Render::DrawTriangles recieved vertex array which holds an incompatible index buffer (size={})", va->getIndexBuffer()->getCount());
            return;
        }
        va->bind();

        glDrawElements(
            GL_TRIANGLES,
            va->getIndexBuffer()->getCount(),
            ((OpenGLIndexBuffer*)(va->getIndexBuffer().get()))->getOpenGLType(),
            nullptr
        );
    }
}
