#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Renderer/Texture2D.hpp"
#include "Car/Renderer/Shader.hpp"
#include "Car/Renderer/Font.hpp"

namespace Car::ResourceManager {
    void Init();
    void Shutdown();

    void setResourceDirectory(const std::string& resourceDirectoryName);
    void setImagesSubdirectory(const std::string& imagesSubdirectoryName);
    void setShadersSubdirectory(const std::string& shadersSubdirectoryName);
    void setFontsSubdirectory(const std::string& fontsSubdirectoryName);

    std::string getResourceDirectory();
    std::string getImagesSubdirectory();
    std::string getShadersSubdirectory();
    std::string getFontsSubdirectory();
} // namespace Car::ResourceManager
