#include "Car/ResourceManager.hpp"
#include "Car/Core/Log.hpp"


struct ResourceManagerData {
    std::unordered_map<std::string, Car::Ref<Car::Texture2D>> textures;
    std::unordered_map<std::string, Car::Ref<Car::Shader>> shaders;
    std::filesystem::path resourceDirectory = "./resources";
    std::filesystem::path imagesSubdirectory = "images";
    std::filesystem::path shadersSubdirectory = "shaders";
};


#define CR_RM_NEED_INITIALIZATION_RET_SPECIAL(ret) if (sData == nullptr) { \
                                                    CR_CORE_ERROR("ResourceManager Not Initialized"); \
                                                    CR_DEBUGBREAK(); \
                                                    return ret; }
#define CR_RM_NEED_INITIALIZATION_RET_VOID() if (sData == nullptr) { \
                                                CR_CORE_ERROR("ResourceManager Not Initialized"); \
                                                CR_DEBUGBREAK(); \
                                                return; }


namespace Car {
    static ResourceManagerData* sData = nullptr;

    void ResourceManager::Init() {
        if (sData != nullptr) {
            CR_CORE_ERROR("ResourceManager already initialized");
            return;
        }
        
        sData = new ResourceManagerData();
        
        CR_CORE_DEBUG("ResourceManager Initialized");
    }
    
    void ResourceManager::Shutdown() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        
        ResourceManager::ClearAll();
        
        delete sData;
        sData = nullptr;
        
        CR_CORE_DEBUG("ResourceManager shutdown");
    }
    
    void ResourceManager::setResourceDirectory(const std::string& resourceDirectoryName) {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        sData->resourceDirectory = resourceDirectoryName;
    }
    
    void ResourceManager::setImagesSubdirectory(const std::string& imagesSubdirectoryName) {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        sData->imagesSubdirectory = imagesSubdirectoryName;
    }
    
    void ResourceManager::setShadersSubdirectory(const std::string& shadersSubdirectoryName) {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        sData->shadersSubdirectory = shadersSubdirectoryName;
    }

    std::string ResourceManager::getShadersSubdirectory() {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        return sData->shadersSubdirectory;
    }
    
    
    std::string ResourceManager::getResourceDirectory() {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        return sData->resourceDirectory;
    }
    
    std::string ResourceManager::getImagesSubdirectory() {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        return sData->imagesSubdirectory;
    }
    
    Ref<Texture2D> ResourceManager::LoadTexture2D(const std::string& name, bool flipped, Texture2D::Format format, Texture2D::Format internalFormat, Texture2D::Type type) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        
        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;
        
        if (sData->textures.find(fullname) != sData->textures.end()) {
            CR_CORE_ERROR("Texture2D {0} already loaded, see Car::ResourceManager::GetTexture2D or Car::ResourceManager::LoadOrOverideTexture2D", fullname);
            return nullptr;
        }
        
        Ref<Texture2D> texture = Texture2D::Create(fullname, flipped, format, internalFormat, type);
        
        sData->textures[fullname] = texture;
        
        return texture;
    }
    
    Ref<Texture2D> ResourceManager::LoadOrOverideTexture2D(const std::string& name, bool flipped, Texture2D::Format format, Texture2D::Format internalFormat, Texture2D::Type type) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        
        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;

        Ref<Texture2D> texture = Texture2D::Create(fullname, flipped, format, internalFormat, type);
        
        sData->textures[fullname] = texture;
        
        return texture;
    }
    
    Ref<Texture2D> ResourceManager::GetTexture2D(const std::string& name) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        
        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;
        
        if (sData->textures.find(fullname) == sData->textures.end()) {
            CR_CORE_ERROR("Texture2D {0} has not been laoded, see Car::ResourceManager::LoadTexture2D", fullname);
            return nullptr;
        }
        
        return sData->textures[fullname];
    }
    
    bool ResourceManager::Texture2DExists(const std::string& name) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(false);
        
        std::string fullname = sData->resourceDirectory / sData->imagesSubdirectory / name;
        
        return sData->textures.find(fullname) != sData->textures.end();
    }
    
    Ref<Shader> ResourceManager::LoadShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        
        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;
        
        if (sData->shaders.find(fullname) != sData->shaders.end()) {
            CR_CORE_ERROR("Shader {0} already loaded, see Car::ResourceManager::GetShader or Car::ResourceManager::LoadOrOverideShader", fullname);
            return nullptr;
        }
        
        Ref<Shader> shader = Shader::Create(vertexShaderFilepath, fragmeantShaderFilepath);
        
        sData->shaders[fullname] = shader;
        
        return shader;
    }
    
    Ref<Shader> ResourceManager::LoadOrOverideShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        
        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;
        
        Ref<Shader> shader = Shader::Create(vertexShaderFilepath, fragmeantShaderFilepath);
        
        sData->shaders[fullname] = shader;
        
        return shader;        
    }
    
    Ref<Shader> ResourceManager::GetShader(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(nullptr);
        
        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;
        
        if (sData->shaders.find(fullname) == sData->shaders.end()) {
            CR_CORE_ERROR("Shader {0} has not been laoded, see Car::ResourceManager::LoadTexture2D", fullname);
            return nullptr;
        }
        
        return sData->shaders[fullname];
    }
    
    bool ResourceManager::ShaderExists(const std::string& vertexShaderFilepath, const std::string& fragmeantShaderFilepath) {
        CR_RM_NEED_INITIALIZATION_RET_SPECIAL(false);
        
        std::string fullname = sData->resourceDirectory / sData->shadersSubdirectory / vertexShaderFilepath /
                               sData->resourceDirectory / sData->shadersSubdirectory / fragmeantShaderFilepath;
        
        return sData->shaders.find(fullname) != sData->shaders.end();
    }
    
    void ResourceManager::ClearAll() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        
        sData->shaders.clear();
    }
    
    void ResourceManager::ClearTexture2Ds() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();
        
        sData->textures.clear();
    }
    
    void ResourceManager::ClearShaders() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();

        sData->shaders.clear();
        sData->textures.clear();
    }
    
}
