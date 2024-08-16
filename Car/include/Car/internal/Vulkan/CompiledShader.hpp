#pragma once

#include "Car/Core/Core.hpp"

namespace Car {
    enum class DescriptorType : uint8_t {
        NONE = 0,
        UniformBuffer = 1,
        Sampler2D= 2,
    };

    enum class DescriptorStage : uint8_t {
        VertexShader = BIT(1),
        FragmeantShader = BIT(2),
        Combined = VertexShader | FragmeantShader
    };

    struct Binding {
        uint8_t binding;
        Car::DescriptorType descriptorType;
        Car::DescriptorStage stageFlags;
    };

    // .crsh file format
    // setCount: u8
    //     bindingCount: u8
    //         binding: u8
    //         descriptorType: u8
    //         stageFlags: u8
    // vertexShaderSize: u32
    // fragmeantShaderSize: u32
    // vertexShaderCode: char[]
    // fragmeantShaderCode: char[]
    struct CompiledShader {
        std::vector<std::vector<Binding>> sets;
        std::string vertexShader;
        std::string fragmeantShader;

        std::string toString() {
            std::stringstream stream;

            stream << (uint8_t)sets.size();

            for (uint32_t i = 0; i < sets.size(); i++) {
                stream << (uint8_t)sets[i].size();

                for (uint32_t j = 0; j < sets[i].size(); j++) {
                    stream << (uint8_t)sets[i][j].binding;
                    stream << (uint8_t)sets[i][j].descriptorType;
                    stream << (uint8_t)sets[i][j].stageFlags;
                }
            }

            stream << (uint32_t)vertexShader.size();
            stream << (uint32_t)fragmeantShader.size();

            stream.write(vertexShader.data(), vertexShader.size());
            stream.write(fragmeantShader.data(), fragmeantShader.size());

            return stream.str();
        }

        static CompiledShader fromString(const std::string& str) {
            CompiledShader ret;

            std::stringstream stream(str);
            uint8_t setCount;

            stream >> setCount;
            ret.sets.resize(setCount);

            for (uint32_t i = 0; i < setCount; i++) {
                uint8_t bindingCount;
                stream >> bindingCount;
                if (bindingCount == 0) {
                    // error goes here
                }
                ret.sets[i].resize(bindingCount);
                for (uint32_t j = 0; j < bindingCount; j++) {
                    uint8_t descriptorType;
                    uint8_t stageFlags;
                    stream >> ret.sets[i][j].binding;
                    stream >> descriptorType;
                    stream >> stageFlags;
                    ret.sets[i][j].descriptorType = (Car::DescriptorType)descriptorType;
                    ret.sets[i][j].stageFlags = (Car::DescriptorStage)stageFlags;
                }
            }
            uint32_t vertexShaderSize;
            uint32_t fragmeantShaderSize;
            stream >> vertexShaderSize;
            stream >> fragmeantShaderSize;

            ret.vertexShader.resize(vertexShaderSize);
            ret.fragmeantShader.resize(fragmeantShaderSize);
            stream.get(ret.vertexShader.data(), vertexShaderSize);
            stream.get(ret.fragmeantShader.data(), fragmeantShaderSize);

            return ret;
        }
    };
} // namespace Car