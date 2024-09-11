#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/UniformBuffer.hpp"

namespace Car {
    class Shader {
    public:
        enum class PolygonMode { FILL, LINE, POINT };
        enum class CullMode { NONE, FRONT, BACK, FRONT_AND_BACK };
        enum class FrontFace { CLOCKWISE, COUNTER_CLOCKWISE };
        enum class VertexInputRate { VERTEX, /* ,INSTANCE */ };
        enum class PrimitiveTopology {
            POINT_LIST, TRIANGLE_LIST, LINE_LIST, TRIANGLE_LIST_WITH_ADJACENCY, LINE_LIST_WITH_ADJACENCY, PATCH_LIST,
            TRIANGLE_STRIP, LINE_STRIP, TRIANGLE_STRIP_WITH_ADJACENCY, LINE_STRIP_WITH_ADJACENCY,
            TRIANGLE_FAN,
        };
        enum class ColorComponent : uint8_t {
            R = BIT(1), G = BIT(2), B = BIT(3), A = BIT(4),
            RG = R | G, RB = R | B, RA = R | A, GB = G | B, GA = G | A, BA = B | A,
            RGB = R | G | B, GBA = G | B | A, RBA = R | B | A, RGA = R | G | A,
            RGBA = R | G | B | A,
        };
        enum class BlendFactor {
            ZERO, ONE, SRC_COLOR, ONE_MINUS_SRC_COLOR, DST_COLOR, ONE_MINUS_DST_COLOR, SRC_ALPHA,
            ONE_MINUS_SRC_ALPHA, DST_ALPHA, ONE_MINUS_DST_ALPHA, CONSTANT_COLOR, ONE_MINUS_CONSTANT_COLOR, CONSTANT_ALPHA,
            ONE_MINUS_CONSTANT_ALPHA, SRC_ALPHA_SATURATE, SRC1_COLOR, ONE_MINUS_SRC1_COLOR, SRC1_ALPHA, ONE_MINUS_SRC1_ALPHA,
        };
        enum class BlendOp { ADD, SUBTRACT, REVERSE_SUBTRACT, MIN, MAX };
        enum class LogicOp {
            CLEAR, AND, AND_REVERSE, COPY, AND_INVERTED, NO_OP, XOR, OR, NOR,
            EQUIVALENT, INVERT, OR_REVERSE, COPY_INVERTED, OR_INVERTED, NAND, SET
        };
        
        class VertexInputLayout {
        public:
            enum class DataType {
                None,
                Float, Float2, Float3, Float4,
                Int, Int2, Int3, Int4,
                UInt, UInt2, UInt3, UInt4,
                NormByte, NormByte2, NormByte3, NormByte4
            };
            CR_FORCE_INLINE uint32_t sizeofDataType(DataType type) {
                switch (type) {
                    case DataType::Float:
                    case DataType::UInt:
                    case DataType::Int:
                        return 4 * 1;
                    case DataType::Float2:
                    case DataType::UInt2:
                    case DataType::Int2:
                        return 4 * 2;
                    case DataType::Float3:
                    case DataType::UInt3:
                    case DataType::Int3:
                        return 4 * 3;
                    case DataType::Float4:
                    case DataType::UInt4:
                    case DataType::Int4:
                        return 3 * 4;
                    case DataType::NormByte:
                        return 1;
                    case DataType::NormByte2:
                        return 2;
                    case DataType::NormByte3:
                        return 3;
                    case DataType::NormByte4:
                        return 4;
                    default:
                        throw std::runtime_error("Unrecognized datatype: " + std::to_string((int)type));
                    }
            }
            struct Element {
                std::string name;
                DataType type;
                uint32_t size;
                uint32_t offset; 
    
                Element(const std::string& name, DataType type)
                    : name(name), type(type), size((sizeofDataType(type))), offset(0) {}
                Element() : name(""), type(DataType::None), size(0), offset(0) {}
            };
        public:
            VertexInputLayout(const std::initializer_list<Element>& elements) : mElements(elements) {
                calculateOffsetsAndStrides();
            }
            VertexInputLayout() : mElements(0) {}
            
            uint32_t getStride() const { return mStride; }
            uint32_t getTotalSize() const {
                uint32_t size = 0;
                for (const auto& element : mElements) {
                    size += sizeofDataType(element.type);
                }
                return size;
            }
    
            std::vector<Element>::iterator begin() { return mElements.begin(); }
            std::vector<Element>::iterator end() { return mElements.end(); }
    
            const std::vector<Element>& getElements() const { return mElements; }
        private:
            void calculateOffsetsAndStrides() {
                uint32_t offset = 0;
                mStride = 0;
                
                for (auto& element : mElements) {
                    element.offset = offset;
                    mStride += element.size;
    
                    offset += element.size;
                }
            }
        private:
            std::vector<Element> mElements;
            uint32_t mStride;
        };

        struct ColorBlendAttachmeant {
            bool enable = true;
            ColorComponent writeMask = ColorComponent::RGBA;
            BlendFactor srcColorBlendFactor = BlendFactor::SRC_ALPHA;
            BlendFactor dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
            BlendOp colorBlendOp = BlendOp::ADD;
            BlendFactor srcAlphaBlendFactor = BlendFactor::ONE;
            BlendFactor dstAlphaBlendFactor = BlendFactor::ZERO;
            BlendOp alphaBlendOp = BlendOp::ADD;
            LogicOp logicOp = LogicOp::COPY;
            bool enableLogicOp = false;
            glm::vec4 blendConstant = glm::vec4(0.0f);
        };
        
        struct PushConstantLayout {
            bool useInVertexShader = false;
            bool useInFragmentShader = false;
            uint32_t size = 0;
        };

        struct Specification {
            VertexInputLayout vertexInputLayout;
            PushConstantLayout pushConstantLayout;
            VertexInputRate vertexInputRate = VertexInputRate::VERTEX;
            PolygonMode polygonMode = PolygonMode::FILL;
            CullMode cullMode = CullMode::BACK;
            FrontFace frontFace = FrontFace::CLOCKWISE;
            PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
            bool primitiveRestartEnable = false;
            ColorBlendAttachmeant colorBlendAttachmeant;
            std::string vertexShaderEntryName = "main";
            std::string fragmentShaderEntryName = "main";
        };

    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;

        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<UniformBuffer> ub) = 0;
        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<Texture2D> texture) = 0;

        static Ref<Shader> Create(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath,
                                  const Specification* pSpec);
    };
} // namespace Car
