#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/Renderer/Font.hpp"


namespace Car::ResourceManager {
    void Init();
    void Shutdown();
    
    void ClearAll();
    void ClearTexture2Ds();
    void ClearShaders();
    void ClearFonts();
    
    void setResourceDirectory(const std::string& resourceDirectoryName);
    void setImagesSubdirectory(const std::string& imagesSubdirectoryName);
    void setShadersSubdirectory(const std::string& shadersSubdirectoryName);
    void setFontsSubdirectory(const std::string& fontsSubdirectoryName);
    
    std::string getResourceDirectory();
    std::string getImagesSubdirectory();
    std::string getShadersSubdirectory();
    std::string getFontsSubdirectory();
    
    Ref<Texture2D> LoadTexture2D(
        const std::string& name,
        bool flipped=false,
        Texture2D::Format format=Texture2D::Format::RGBA,
        Texture2D::Format internalFormat=Texture2D::Format::RGBA8,
        Texture2D::Type type=Texture2D::Type::Byte
    );
    Ref<Texture2D> LoadOrOverideTexture2D(
        const std::string& name,
        bool flipped=false,
        Texture2D::Format format=Texture2D::Format::RGBA,
        Texture2D::Format internalFormat=Texture2D::Format::RGBA8,
        Texture2D::Type type=Texture2D::Type::Byte
    );
    Ref<Texture2D> GetTexture2D(const std::string& name);
    bool Texture2DExists(const std::string& name);
    
    Ref<Shader> LoadShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
    Ref<Shader> LoadOrOverideShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
    Ref<Shader> GetShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
    bool ShaderExists(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath);
    
    Ref<Font> LoadFont(const std::string& name, uint32_t height, const std::string& charsToLoad=CR_DEFAULT_CHARS);
    Ref<Font> LoadOrOverrideFont(const std::string& name, uint32_t height, const std::string& charsToLoad=CR_DEFAULT_CHARS);
    Ref<Font> GetFont(const std::string& name, uint32_t height);
    bool FontExists(const std::string& name, uint32_t height);
}
