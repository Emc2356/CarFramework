#include "Car/ResourceManager.hpp"
#include "Car/Core/Log.hpp"
#include <unordered_map>

struct ResourceManagerData {
    std::unordered_map<std::string, Car::Ref<Car::Texture2D>> textures;
    std::unordered_map<std::string, Car::Ref<Car::Shader>> shaders;
    std::unordered_map<std::string, Car::Ref<Car::Font>> fonts;
    std::filesystem::path resourceDirectory = "./resources";
    std::filesystem::path imagesSubdirectory = "images";
    std::filesystem::path shadersSubdirectory = "shaders";
    std::filesystem::path fontsSubdirectory = "fonts";
};

#define CR_RM_NEED_INITIALIZATION_RET_SPECIAL(ret)                                                                     \
    if (sData == nullptr) {                                                                                            \
        CR_CORE_ERROR("ResourceManager Not Initialized");                                                              \
        CR_DEBUGBREAK();                                                                                               \
        return ret;                                                                                                    \
    }
#define CR_RM_NEED_INITIALIZATION_RET_VOID()                                                                           \
    if (sData == nullptr) {                                                                                            \
        CR_CORE_ERROR("ResourceManager Not Initialized");                                                              \
        CR_DEBUGBREAK();                                                                                               \
        return;                                                                                                        \
    }

namespace Car::ResourceManager {
    static ResourceManagerData* sData = nullptr;

    void Init() {
        if (sData != nullptr) {
            CR_CORE_ERROR("ResourceManager already initialized");
            return;
        }

        sData = new ResourceManagerData();

        CR_CORE_DEBUG("ResourceManager Initialized");
    }

    void Shutdown() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();

        ClearAll();

        delete sData;
        sData = nullptr;

        CR_CORE_DEBUG("ResourceManager shutdown");
    }

    void setResourceDirectory(const std::string& resourceDirectoryName) {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        sData->resourceDirectory = resourceDirectoryName;
    }

    void setImagesSubdirectory(const std::string& imagesSubdirectoryName) {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        sData->imagesSubdirectory = imagesSubdirectoryName;
    }

    void setShadersSubdirectory(const std::string& shadersSubdirectoryName) {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        sData->shadersSubdirectory = shadersSubdirectoryName;
    }

    void setFontsSubdirectory(const std::string& fontsSubdirectoryName) {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        sData->fontsSubdirectory = fontsSubdirectoryName;
    }

    std::string getShadersSubdirectory() {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        return sData->shadersSubdirectory;
    }

    std::string getResourceDirectory() {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        return sData->resourceDirectory;
    }

    std::string getImagesSubdirectory() {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        return sData->imagesSubdirectory;
    }

    std::string getFontsSubdirectory() {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        return sData->fontsSubdirectory;
    }

    Ref<Texture2D> LoadTexture2D(const std::string& name, bool flipped, Texture2D::Format format,
                                 Texture2D::Format internalFormat, Texture2D::Type type) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;

        if (sData->textures.find(fullname) != sData->textures.end()) {
            CR_CORE_ERROR("Texture2D {0} already loaded, see "
                          "Car::ResourceManager::GetTexture2D or "
                          "Car::ResourceManager::LoadOrOverideTexture2D",
                          fullname);
            return nullptr;
        }

        Ref<Texture2D> texture = Texture2D::Create(fullname, flipped, format, internalFormat, type);

        sData->textures[fullname] = texture;

        return texture;
    }

    Ref<Texture2D> LoadOrOverideTexture2D(const std::string& name, bool flipped, Texture2D::Format format,
                                          Texture2D::Format internalFormat, Texture2D::Type type) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;

        Ref<Texture2D> texture = Texture2D::Create(fullname, flipped, format, internalFormat, type);

        sData->textures[fullname] = texture;

        return texture;
    }

    Ref<Texture2D> GetTexture2D(const std::string& name) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;

        if (sData->textures.find(fullname) == sData->textures.end()) {
            CR_CORE_ERROR("Texture2D {0} has not been laoded, see "
                          "Car::ResourceManager::LoadTexture2D",
                          fullname);
            return nullptr;
        }

        return sData->textures[fullname];
    }

    bool Texture2DExists(const std::string& name) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(false);

        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;

        return sData->textures.find(fullname) != sData->textures.end();
    }

    Ref<Shader> LoadShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;

        if (sData->shaders.find(fullname) != sData->shaders.end()) {
            CR_CORE_ERROR("Shader {0} already loaded, see Car::ResourceManager::GetShader "
                          "or Car::ResourceManager::LoadOrOverideShader",
                          fullname);
            return nullptr;
        }

        Ref<Shader> shader = Shader::Create(vertexShaderFilepath, fragmeantShaderFilepath);

        sData->shaders[fullname] = shader;

        return shader;
    }

    Ref<Shader> LoadOrOverideShader(const std::string& vertexShaderFilepath,
                                    const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;

        Ref<Shader> shader = Shader::Create(vertexShaderFilepath, fragmeantShaderFilepath);

        sData->shaders[fullname] = shader;

        return shader;
    }

    Ref<Shader> GetShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;

        if (sData->shaders.find(fullname) == sData->shaders.end()) {
            CR_CORE_ERROR("Shader {0} has not been laoded, see "
                          "Car::ResourceManager::LoadTexture2D",
                          fullname);
            return nullptr;
        }

        return sData->shaders[fullname];
    }

    bool ShaderExists(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(false);

        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;

        return sData->shaders.find(fullname) != sData->shaders.end();
    }

    Ref<Font> LoadFont(const std::string& name, uint32_t height, const std::string& charsToLoad) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string mangledName =
            sData->resourceDirectory / sData->fontsSubdirectory / name / ("size=" + std::to_string(height));

        std::string fullname = sData->resourceDirectory / sData->fontsSubdirectory / name;

        if (sData->fonts.find(mangledName) != sData->fonts.end()) {
            CR_CORE_ERROR("Font {0} already loaded, see Car::ResourceManager::GetFont or "
                          "Car::ResourceManager::LoadOrOverideFont",
                          fullname);
            return nullptr;
        }

        Ref<Font> font = createRef<Font>(fullname, height, charsToLoad);

        sData->fonts[mangledName] = font;

        return font;
    }

    Ref<Font> LoadOrOverrideFont(const std::string& name, uint32_t height, const std::string& charsToLoad) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string mangledName =
            sData->resourceDirectory / sData->fontsSubdirectory / name / ("size=" + std::to_string(height));

        std::string fullname = sData->resourceDirectory / sData->fontsSubdirectory / name;

        Ref<Font> font = createRef<Font>(fullname, height, charsToLoad);

        sData->fonts[mangledName] = font;

        return font;
    }

    Ref<Font> GetFont(const std::string& name, uint32_t height) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);

        std::string mangledName =
            sData->resourceDirectory / sData->fontsSubdirectory / name / ("size=" + std::to_string(height));

        std::string fullname = sData->resourceDirectory / sData->fontsSubdirectory / name;

        if (sData->fonts.find(mangledName) == sData->fonts.end()) {
            CR_CORE_ERROR("Font {0} has not been laoded, see Car::ResourceManager::LoadFont", fullname);
            return nullptr;
        }

        return sData->fonts[mangledName];
    }

    bool FontExists(const std::string& name, uint32_t height) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(false);

        std::string mangledName =
            sData->resourceDirectory / sData->fontsSubdirectory / name / ("size=" + std::to_string(height));

        return sData->fonts.find(mangledName) != sData->fonts.end();
    }

    void ClearAll() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();

        ClearTexture2Ds();
        ClearShaders();
        ClearFonts();
    }

    void ClearTexture2Ds() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();

        sData->textures.clear();
    }

    void ClearShaders() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();

        sData->shaders.clear();
    }

    void ClearFonts() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();

        sData->fonts.clear();
    }
} // namespace Car::ResourceManager
