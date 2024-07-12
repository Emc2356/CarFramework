#include "Car/internal/OpenGL/SSBO.hpp"

#include <glad/gl.h>


namespace Car {
    OpenGLSSBO::OpenGLSSBO(void* data, uint32_t size, Buffer::Usage usage) {
        switch (usage) {
            case Buffer::Usage::StaticDraw: { mOpenGLUsage = GL_STATIC_DRAW; break; }
            case Buffer::Usage::DynamicDraw: {mOpenGLUsage = GL_DYNAMIC_DRAW; break; }
            default: {  CR_CORE_ERROR("Unrecognized Usage for IndexBuffer: {}", static_cast<uint32_t>(usage)); CR_DEBUGBREAK(); }
        }
        glGenBuffers(1, &mID);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, mOpenGLUsage);

        mSize = size;
    }
    
    OpenGLSSBO::~OpenGLSSBO() {
        glDeleteBuffers(1, &mID);
    }

    void OpenGLSSBO::updateBuffer(void* data, uint32_t size) {
        if (size <= mSize) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);    
        } else {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, mID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, mOpenGLUsage);
        }
    }
    
    void OpenGLSSBO::bind(uint32_t binding) const {
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding, mID, 0, mSize);
    }
    
    void OpenGLSSBO::unbind() const {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    Ref<SSBO> SSBO::Create(void* data, uint32_t size, Buffer::Usage usage) {
        return createRef<OpenGLSSBO>(data, size, usage);
    }
}
