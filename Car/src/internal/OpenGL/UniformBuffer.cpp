#include "Car/internal/OpenGL/UniformBuffer.hpp"
#include "Car/Core/Log.hpp"

#include <glad/gl.h>

namespace Car {
    static std::vector<uint32_t> sTakenBindings{};
    
    OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        if (std::find(sTakenBindings.begin(), sTakenBindings.end(), binding) != sTakenBindings.end()) {
            CR_CORE_ERROR("Binding {} is already taken", binding);
            CR_DEBUGBREAK();
        }
        sTakenBindings.push_back(binding);
        
        mSize = size;
        mBinding = binding;
        mUsage = usage;

        switch (usage) {
        case Buffer::Usage::StaticDraw: {
            mOpenGLUsage = GL_STATIC_DRAW;
            break;
        }
        case Buffer::Usage::DynamicDraw: {
            mOpenGLUsage = GL_DYNAMIC_DRAW;
            break;
        }
        default: {
            CR_CORE_ERROR("Unrecognized Usage for VertexBuffer: {}", static_cast<uint32_t>(usage));
            CR_DEBUGBREAK();
        }
        }

        glCreateBuffers(1, &mID);
        glNamedBufferData(mID, mSize + 1, nullptr, mOpenGLUsage);
        glBindBufferBase(GL_UNIFORM_BUFFER, mBinding, mID);
    }

    OpenGLUniformBuffer::~OpenGLUniformBuffer() { 
        glDeleteBuffers(1, &mID);
        sTakenBindings.erase(std::find(sTakenBindings.begin(), sTakenBindings.end(), mBinding));
    }

    void OpenGLUniformBuffer::setData(const void* data, uint32_t offset /*=0*/) {
        glBindBufferBase(GL_UNIFORM_BUFFER, mBinding, mID);
        glNamedBufferSubData(mID, offset, mSize, data);
    }

    Ref<Car::UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding, Buffer::Usage usage) {
        return createRef<OpenGLUniformBuffer>(size, binding, usage);
    }
} // namespace Car
