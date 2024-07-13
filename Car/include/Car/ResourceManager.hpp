#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/Renderer/Texture2D.hpp"


namespace Car {
    class ResourceManager {
    public:
        static void Init();
        static void Shutdown();
        
        static void ClearAll();
        static void ClearTexture2Ds();
        static void ClearShaders();
        
        static void setResourceDirectory(const std::string& resourceDirectoryName);
        static void setImagesSubdirectory(const std::string& imagesSubdirectoryName);
        static void setShadersSubdirectory(const std::string& shadersSubdirectoryName);
        
        static std::string getResourceDirectory();
        static std::string getImagesSubdirectory();
        static std::string getShadersSubdirectory();
        
        static Ref<Texture2D> LoadTexture2D(
            const std::string& name,
            bool flipped=false,
            Texture2D::Format format=Texture2D::Format::RGBA,
            Texture2D::Format internalFormat=Texture2D::Format::RGBA8,
            Texture2D::Type type=Texture2D::Type::Byte
        );
        static Ref<Texture2D> LoadOrOverideTexture2D(
            const std::string& name,
            bool flipped=false,
            Texture2D::Format format=Texture2D::Format::RGBA,
            Texture2D::Format internalFormat=Texture2D::Format::RGBA8,
            Texture2D::Type type=Texture2D::Type::Byte
        );
        static Ref<Texture2D> GetTexture2D(const std::string& name);
        static bool Texture2DExists(const std::string& name);
        
        static Ref<Shader> LoadShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
        static Ref<Shader> LoadOrOverideShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
        static Ref<Shader> GetShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
        static bool ShaderExists(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
    };
}
