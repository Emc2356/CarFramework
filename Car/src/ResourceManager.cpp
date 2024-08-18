#include "Car/ResourceManager.hpp"
#include "Car/Core/Log.hpp"
#include <unordered_map>

struct ResourceManagerData {
    std::filesystem::path resourceDirectory = "./resources";
    std::filesystem::path imagesSubdirectory = "images";
    std::filesystem::path shadersSubdirectory = "shaders";
    std::filesystem::path fontsSubdirectory = "fonts";
};

#define CR_RM_NEED_INITIALIZATION_RET_SPECIAL(ret)                                                                     \
    CR_IF (sData == nullptr) {                                                                                         \
        CR_CORE_ERROR("ResourceManager Not Initialized");                                                              \
        CR_DEBUGBREAK();                                                                                               \
        return ret;                                                                                                    \
    }
#define CR_RM_NEED_INITIALIZATION_RET_VOID()                                                                           \
    CR_IF (sData == nullptr) {                                                                                         \
        CR_CORE_ERROR("ResourceManager Not Initialized");                                                              \
        CR_DEBUGBREAK();                                                                                               \
        return;                                                                                                        \
    }

namespace Car::ResourceManager {
    static ResourceManagerData* sData = nullptr;

    void Init() {
        CR_IF (sData != nullptr) {
            CR_CORE_ERROR("ResourceManager already initialized");
            return;
        }

        sData = new ResourceManagerData();

        CR_CORE_DEBUG("ResourceManager Initialized");
    }

    void Shutdown() {
        CR_RM_NEED_INITIALIZATION_RET_VOID();

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
} // namespace Car::ResourceManager
