#include "Car/internal/OpenGL/VertexBuffer.hpp"
#include "Car/Core/Log.hpp"
#include "Car/Renderer/BufferLayout.hpp"

#include <glad/gl.h>


namespace Car {
    static uint32_t BufferLayoutDataTypeToOpenGLBaseType(BufferLayout::DataType type) {
        switch (type) {
        case BufferLayout::DataType::Float:
        case BufferLayout::DataType::Float2:
        case BufferLayout::DataType::Float3:
        case BufferLayout::DataType::Float4:
        case BufferLayout::DataType::Mat3:
        case BufferLayout::DataType::Mat4:
            return GL_FLOAT;
        case BufferLayout::DataType::Int:
        case BufferLayout::DataType::Int2:
        case BufferLayout::DataType::Int3:
        case BufferLayout::DataType::Int4:
            return GL_INT;
        default:
            CR_CORE_ERROR("Unrecognized BufferLayout::DataType: {0}", static_cast<uint32_t>(type));
            CR_DEBUGBREAK();
            return 0;
        }
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        switch (usage) {
        case Buffer::Usage::StaticDraw: { mOpenGLUsage = GL_STATIC_DRAW; break; }
        case Buffer::Usage::DynamicDraw: {mOpenGLUsage = GL_DYNAMIC_DRAW; break; }
        default: {  CR_CORE_ERROR("Unrecognized Usage for VertexBuffer: {}", static_cast<uint32_t>(usage)); CR_DEBUGBREAK(); }
        }

        glGenBuffers(1, &mID);
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        glBufferData(GL_ARRAY_BUFFER, count * Buffer::sizeOfType(type), data, mOpenGLUsage);

        mCount = count;
        mData = data;
        mUsage = usage;
        mType = type;
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        switch (usage) {
        case Buffer::Usage::StaticDraw: { mOpenGLUsage = GL_STATIC_DRAW; break; }
        case Buffer::Usage::DynamicDraw: {mOpenGLUsage = GL_DYNAMIC_DRAW; break; }
        default: {  CR_CORE_ERROR("Unrecognized Usage for VertexBuffer: {}", static_cast<uint32_t>(usage)); CR_DEBUGBREAK(); }
        }

        glGenBuffers(1, &mID);
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        glBufferData(GL_ARRAY_BUFFER, count * Buffer::sizeOfType(type), nullptr, mOpenGLUsage);

        mCount = count;
        mData = nullptr;
        mUsage = usage;
        mType = type;
    }

    void OpenGLVertexBuffer::setLayout(const BufferLayout& layout) {
        mLayout = layout;
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() {
        glDeleteBuffers(1, &mID);
    }

    void OpenGLVertexBuffer::bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, mID);
    }

    void OpenGLVertexBuffer::unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void OpenGLVertexBuffer::applyLayout() const {
        const std::vector<BufferLayout::Element>& elements = mLayout.getElements();
        for (uint32_t i = 0; i < elements.size(); i++) {
            const BufferLayout::Element& element = elements[i];

            glEnableVertexAttribArray(i);
			glVertexAttribPointer(
                i,
				element.getComponentCount(),
				BufferLayoutDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				mLayout.getStride(),
				(const void*)(uint64_t)element.offset
            );
        }
    }

    void OpenGLVertexBuffer::updateData(void* data, uint32_t count, uint32_t offset) {
        glBufferSubData(GL_ARRAY_BUFFER, offset * Buffer::sizeOfType(mType), count * Buffer::sizeOfType(mType), data);    
    }

    Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<OpenGLVertexBuffer>(data, count, usage, type);
    }

    Ref<VertexBuffer> VertexBuffer::Create(uint32_t count, Buffer::Usage usage, Buffer::Type type) {
        return createRef<OpenGLVertexBuffer>(count, usage, type);
    }
}
