#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/BufferLayout.hpp"
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
            POINT_LIST,
            LINE_LIST,
            LINE_STRIP,
            TRIANGLE_LIST,
            TRIANGLE_STRIP,
            TRIANGLE_FAN,
            LINE_LIST_WITH_ADJACENCY,
            LINE_STRIP_WITH_ADJACENCY,
            TRIANGLE_LIST_WITH_ADJACENCY,
            TRIANGLE_STRIP_WITH_ADJACENCY,
            PATCH_LIST
        };

        enum class ColorComponent : uint8_t {
            R = BIT(1),
            G = BIT(2),
            B = BIT(3),
            A = BIT(4),

            RG = R | G,
            RB = R | B,
            RA = R | A,
            GB = G | B,
            GA = G | A,
            BA = B | A,

            RGB = R | G | B,
            GBA = G | B | A,
            RBA = R | B | A,
            RGA = R | G | A,

            RGBA = R | G | B | A,
        };

        enum class BlendFactor {
            ZERO,
            ONE,
            SRC_COLOR,
            ONE_MINUS_SRC_COLOR,
            DST_COLOR,
            ONE_MINUS_DST_COLOR,
            SRC_ALPHA,
            ONE_MINUS_SRC_ALPHA,
            DST_ALPHA,
            ONE_MINUS_DST_ALPHA,
            CONSTANT_COLOR,
            ONE_MINUS_CONSTANT_COLOR,
            CONSTANT_ALPHA,
            ONE_MINUS_CONSTANT_ALPHA,
            SRC_ALPHA_SATURATE,
            SRC1_COLOR,
            ONE_MINUS_SRC1_COLOR,
            SRC1_ALPHA,
            ONE_MINUS_SRC1_ALPHA,
        };

        enum class BlendOp {
            ADD,
            SUBTRACT,
            REVERSE_SUBTRACT,
            MIN,
            MAX,
        };

        enum class LogicOp {
            CLEAR,
            AND,
            AND_REVERSE,
            COPY,
            AND_INVERTED,
            NO_OP,
            XOR,
            OR,
            NOR,
            EQUIVALENT,
            INVERT,
            OR_REVERSE,
            COPY_INVERTED,
            OR_INVERTED,
            NAND,
            SET,
        };

        struct ColorBlendAttachmeant {
            bool enable = true;
            ColorComponent writeMask = ColorComponent::RGBA;
            BlendFactor srcColorBlendFactor = BlendFactor::SRC_ALPHA;
            BlendFactor dstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
            BlendOp colorBlendOp = BlendOp::ADD;
            BlendOp alphaBlendOp = BlendOp::ADD;
            BlendFactor srcAlphaBlendFactor = BlendFactor::ONE;
            BlendFactor dstAlphaBlendFactor = BlendFactor::ZERO;
            LogicOp logicOp = LogicOp::COPY;
            bool enableLogicOp = false;
            glm::vec4 blendConstant = glm::vec4(0.0f);
        };

        struct Specification {
            VertexInputRate vertexInputRate = VertexInputRate::VERTEX;
            PolygonMode polygonMode = PolygonMode::FILL;
            CullMode cullMode = CullMode::BACK;
            FrontFace frontFace = FrontFace::CLOCKWISE;
            PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
            bool primitiveRestartEnable = false;
            float minDepth = -1.0f;
            float maxDepth = 1.0f;
            ColorBlendAttachmeant colorBlendAttachmeant;
            std::string vertexShaderEntryName = "main";
            std::string fragmentShaderEntryName = "main";
        };

    public:
        virtual ~Shader() = default;

        virtual void bind() const = 0;

        virtual const ShaderLayoutInput& getInputLayout() const = 0;

        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<UniformBuffer> ub) = 0;
        virtual void setInput(uint32_t set, uint32_t binding, bool applyToAll, Ref<Texture2D> texture) = 0;

        static Ref<Shader> Create(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath,
                                  const ShaderLayoutInput& inputLayout, const Specification* pSpec);
    };
} // namespace Car
