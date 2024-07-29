#pragma once

#include <memory>

namespace Car {
    template <typename T> using Scope = std::unique_ptr<T>;
    template <typename T, typename... Args> constexpr Scope<T> createScope(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T> using Ref = std::shared_ptr<T>;
    template <typename T, typename... Args> constexpr Ref<T> createRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
    template <typename T, typename... Args>
    constexpr Ref<T> staticCastRef(Args&&... args) {
        return std::static_pointer_cast<T>(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    constexpr Ref<T> dynamCastRef(Args&&... args) {
        return std::dynamic_pointer_cast<T>(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    constexpr Ref<T> constCastRef(Args&&... args) {
        return std::const_pointer_cast<T>(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    constexpr Ref<T> reinterpretCastRef(Args&&... args) {
        return std::reinterpret_pointer_cast<T>(std::forward<Args>(args)...);
    }
} // namespace Car
