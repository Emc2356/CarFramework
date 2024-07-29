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
        case BufferLayout::DataType::UInt:
        case BufferLayout::DataType::UInt2:
        case BufferLayout::DataType::UInt3:
        case BufferLayout::DataType::UInt4:
            return GL_UNSIGNED_INT;
        default:
            CR_CORE_ERROR("Unrecognized BufferLayout::DataType: {0}", static_cast<uint32_t>(type));
            CR_DEBUGBREAK();
            return 0;
        }
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage) {
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

        glGenBuffers(1, &mID);
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        glBufferData(GL_ARRAY_BUFFER, size, data, mOpenGLUsage);
        
        mLayout = layout;
        mUsage = usage;
        mSize = size;
        mUsage = usage;
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer() { glDeleteBuffers(1, &mID); }

    void OpenGLVertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, mID); }

    void OpenGLVertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

    void OpenGLVertexBuffer::applyLayout() const {
        const std::vector<BufferLayout::Element>& elements = mLayout.getElements();
        for (uint32_t i = 0; i < elements.size(); i++) {
            const BufferLayout::Element& element = elements[i];

            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, element.getComponentCount(), BufferLayoutDataTypeToOpenGLBaseType(element.type),
                                  element.normalized ? GL_TRUE : GL_FALSE, mLayout.getStride(),
                                  (const void*)(uint64_t)element.offset);
        }
    }

    void OpenGLVertexBuffer::updateData(void* data, uint32_t size, uint32_t offset) {
        glBindBuffer(GL_ARRAY_BUFFER, mID);
        // just update the buffer without relocating the block of memory
        if (size <= mSize) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        } else {
            // since the offset is 0 we dont need to save any data
            if (offset == 0) {
                glBufferData(GL_ARRAY_BUFFER, size, nullptr, mOpenGLUsage);
                mSize = size;
                // here we need to save the data from 0 to offset
            } else {
                // read the buffer
                void* mapped = glMapBufferRange(GL_ARRAY_BUFFER, 0, offset, GL_MAP_READ_BIT);

                CR_ASSERT(mapped, "Failed to map buffer, OpenGL");

                void* temp = malloc(offset + size);
                // save up to offset
                memcpy(temp, mapped, offset);
                memcpy((uint8_t*)mapped + offset, data, size);

                glUnmapBuffer(GL_ARRAY_BUFFER);

                glBufferData(GL_ARRAY_BUFFER, size + offset, mapped, mOpenGLUsage);

                free(temp);

                mSize = size + offset;
            }
        }
    }

    Ref<VertexBuffer> VertexBuffer::Create(void* data, uint32_t size, BufferLayout layout, Buffer::Usage usage) {
        return createRef<OpenGLVertexBuffer>(data, size, layout, usage);
    }
} // namespace Car
