#include "Car/internal/OpenGL/IndexBuffer.hpp"

#include <glad/gl.h>


namespace Car {
    OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        switch (usage) {
        case Buffer::Usage::StaticDraw: { mOpenGLUsage = GL_STATIC_DRAW; break; }
        case Buffer::Usage::DynamicDraw: {mOpenGLUsage = GL_DYNAMIC_DRAW; break; }
        default: {  CR_CORE_ERROR("Unrecognized Usage for IndexBuffer: {}", static_cast<uint32_t>(usage)); CR_DEBUGBREAK(); }
        }

        CR_ASSERT(
            type == Buffer::Type::UnsignedInt || type == Buffer::Type::UnsignedShort || type == Buffer::Type::Byte,
            "Invalid type for IndexBuffer, only UnsignedInt, UnsignedShort and Byte is premited"
        );

        glGenBuffers(1, &mID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * Buffer::sizeOfType(type), data, mOpenGLUsage);
        mCount = count;
        mData = data;
        mUsage = usage;
        mType = type;

        if (type == Buffer::Type::UnsignedInt) {
            mOpenGLType = GL_UNSIGNED_INT;
        } else if (type == Buffer::Type::UnsignedShort) {
            mOpenGLType = GL_UNSIGNED_SHORT;
        } else if (type == Buffer::Type::Byte) {
            mOpenGLType = GL_UNSIGNED_BYTE;
        }
    }

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        switch (usage) {
        case Buffer::Usage::StaticDraw: { mOpenGLUsage = GL_STATIC_DRAW; break; }
        case Buffer::Usage::DynamicDraw: {mOpenGLUsage = GL_DYNAMIC_DRAW; break; }
        default: {  CR_CORE_ERROR("Unrecognized Usage for IndexBuffer: {}", static_cast<uint32_t>(usage)); CR_DEBUGBREAK(); }
        }

        CR_ASSERT(
            type == Buffer::Type::UnsignedInt || type == Buffer::Type::UnsignedShort || type == Buffer::Type::Byte,
            "Invalid type for IndexBuffer, only UnsignedInt, UnsignedShort and Byte is premited"
        );

        glGenBuffers(1, &mID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * Buffer::sizeOfType(type), nullptr, mOpenGLUsage);

        mCount = count;
        mData = nullptr;
        mUsage = usage;
        mType = type;

        if (type == Buffer::Type::UnsignedInt) {
            mOpenGLType = GL_UNSIGNED_INT;
        } else if (type == Buffer::Type::UnsignedShort) {
            mOpenGLType = GL_UNSIGNED_SHORT;
        } else if (type == Buffer::Type::Byte) {
            mOpenGLType = GL_UNSIGNED_BYTE;
        }
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer() {
        glDeleteBuffers(1, &mID);
    }

    void OpenGLIndexBuffer::bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
    }

    void OpenGLIndexBuffer::unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void OpenGLIndexBuffer::updateData(void* data, uint32_t count, uint32_t offset) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * Buffer::sizeOfType(mType), count * Buffer::sizeOfType(mType), data);    
    }

    Ref<IndexBuffer> IndexBuffer::Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<OpenGLIndexBuffer>(data, count, usage, type);
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<OpenGLIndexBuffer>(count, usage, type);
    }
}
