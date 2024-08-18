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
        enum class VertexInputRate {
            VERTEX,
            // INSTANCE
        };
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

        struct Specification {
            VertexInputRate vertexInputRate = VertexInputRate::VERTEX;
            PolygonMode polygonMode = PolygonMode::FILL;
            CullMode cullMode = CullMode::BACK;
            FrontFace frontFace = FrontFace::CLOCKWISE;
            PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
            bool primitiveRestartEnable = false;
            float minDepth = -1.0f;
            float maxDepth = 1.0f;
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
