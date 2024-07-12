#pragma once

#include "Car/Core/Core.hpp"


namespace Car {
    class BufferLayout {
    public:
        enum class DataType {
            None,
            Float, Float2, Float3, Float4,
            Int, Int2, Int3, Int4,
            UInt, UInt2, UInt3, UInt4,
            Mat3, Mat4,
        };
        struct Element {
            std::string name;
            DataType type;
            uint32_t size;
            uint32_t offset;
            bool normalized;

            Element(const std::string& name, DataType type, bool normalized=false) :
                name(name), type(type), size((sizeOfDataType(type))), offset(0), normalized(normalized) {}
            Element() : name(""), type(DataType::None), size(0), offset(0) {}

            uint32_t getComponentCount() const {
                switch (type) {
                case DataType::Float:
                case DataType::UInt:
                case DataType::Int: return 1;
                case DataType::Float2:
                case DataType::UInt2:
                case DataType::Int2: return 2;
                case DataType::Float3:
                case DataType::UInt3:
                case DataType::Int3: return 3;
                case DataType::Float4:
                case DataType::UInt4:
                case DataType::Int4: return 4;
                case DataType::Mat3: return 4 * 3;
                case DataType::Mat4: return 4 * 4;
                default:
                    throw std::runtime_error("Unrecognized datatype: " + std::to_string((int)type));
                }
            }
        };
    public:
        BufferLayout(const std::initializer_list<Element>& elements) : mElements(elements), mStride(0) { calculateOffsetsAndStrides(); }
        BufferLayout() : mElements(0), mStride(0) {}

        static uint32_t sizeOfDataType(DataType type) {
            switch (type) {
            case DataType::Float: return 4 * 1;
            case DataType::Float2: return 4 * 2;
            case DataType::Float3: return 4 * 3;
            case DataType::Float4: return 4 * 4;
            case DataType::Int: return 4 * 1;
            case DataType::Int2: return 4 * 2;
            case DataType::Int3: return 4 * 3;
            case DataType::Int4: return 4 * 4;
            case DataType::UInt: return 4 * 1;
            case DataType::UInt2: return 4 * 2;
            case DataType::UInt3: return 4 * 3;
            case DataType::UInt4: return 4 * 4;
            case DataType::Mat3: return 4 * 3 * 3;
            case DataType::Mat4: return 4 * 4 * 4;
            default:
                throw std::runtime_error("Unrecognized datatype: " + std::to_string((int)type));
            }
        }

        uint32_t getStride() const { return mStride; }
        uint32_t getTotalSize() const {
            uint32_t size = 0;
            for (const auto& element : mElements) {
                size += sizeOfDataType(element.type);
            }
            return size;
        }

        std::vector<BufferLayout::Element>::iterator begin() { return mElements.begin(); }
        std::vector<BufferLayout::Element>::iterator end() { return mElements.end(); }

        const std::vector<Element>& getElements() const { return mElements; }
    private:
        void calculateOffsetsAndStrides() {
            uint32_t offset = 0;

            for (size_t i = 0; i < mElements.size(); i++) {
                Element& element = mElements[i];

                element.offset += offset;
                mStride += element.size;

                offset += element.size;
            }
        }
    private:
        std::vector<Element> mElements;
        uint32_t mStride;

    };
}
