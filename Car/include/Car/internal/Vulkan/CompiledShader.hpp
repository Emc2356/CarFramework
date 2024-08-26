#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Core/Log.hpp"
#include <sstream>

namespace Car {
    enum class DescriptorType : uint8_t {
        NONE = 0,
        UniformBuffer = 1,
        Sampler2D = 2,
    };

    enum class DescriptorStage : uint8_t {
        VertexShader = BIT(1),
        FragmeantShader = BIT(2),
        Combined = VertexShader | FragmeantShader
    };

    struct Descriptor {
        uint8_t binding;
        Car::DescriptorType descriptorType;
        Car::DescriptorStage stageFlags;
    };

    // .crss format
    // setCount: u8
    //     descriptorCount: u8
    //         binding: u8
    //         descriptorType: u8
    // shaderLen: u32
    // shaderCode: char[shaderLen]
    struct SingleCompiledShader {
        std::vector<std::vector<Descriptor>> sets;
        std::string shader;

        std::vector<uint8_t> toBytes() {
            uint32_t totalSize = sizeof(uint8_t);
            for (const auto& set : sets) {
                totalSize += sizeof(uint8_t);
                for (const auto& descriptor : set) {
                    UNUSED(descriptor);
                    totalSize += sizeof(uint8_t) * 2;
                }
            }

            totalSize += sizeof(uint32_t);
            totalSize += shader.size();

            std::vector<uint8_t> bytes(totalSize);

            uint32_t pos = 0;
            bytes[pos++] = (uint8_t)sets.size();

            for (const auto& set : sets) {
                bytes[pos++] = (uint8_t)set.size();
                for (const auto& descriptor : set) {
                    bytes[pos++] = (uint8_t)descriptor.binding;
                    bytes[pos++] = (uint8_t)descriptor.descriptorType;
                }
            }
            *(uint32_t*)(&bytes.data()[pos]) = shader.size();
            pos += sizeof(uint32_t);
            std::memcpy(&bytes[pos], shader.data(), shader.size());

            return bytes;
        }

        static SingleCompiledShader fromBytes(std::vector<uint8_t> bytes) {
            SingleCompiledShader ret;
            uint32_t pos = 0;

            ret.sets.resize(bytes[pos++]);
            for (uint32_t i = 0; i < ret.sets.size(); i++) {
                ret.sets[i].resize(bytes[pos++]);
                for (uint32_t j = 0; j < ret.sets[i].size(); j++) {
                    ret.sets[i][j].binding = bytes[pos++];
                    ret.sets[i][j].descriptorType = (Car::DescriptorType)bytes[pos++];
                }
            }
            
            uint32_t codeSize = *(uint32_t*)&bytes[pos];
            pos += 4;
            ret.shader.resize(codeSize);
            std::memcpy(ret.shader.data(), &bytes[pos], codeSize);

            return ret;
        }
    };

    struct CompiledShader {
        std::vector<std::vector<Descriptor>> sets;
        std::string vertexShader;
        std::string fragmeantShader;
    };
} // namespace Car