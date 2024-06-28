#pragma once

#include "Car/Core/Core.hpp"

#include "Car/Scene/Scene.hpp"
#include "Car/Scene/Componets.hpp"

#include <entt/entt.hpp>


namespace Car {
    class Entity {
	public:
		Entity() = default;
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args) {
			CR_ASSERT(!hasComponent<T>(), "Entity already has component!");
			T& component = mScene->mRegistry.emplace<T>(mEntityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T, typename... Args>
		T& addOrReplaceComponent(Args&&... args) {
			T& component = mScene->mRegistry.emplace_or_replace<T>(mEntityHandle, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& getComponent() {
			CR_ASSERT(hasComponent<T>(), "Entity does not have component!");
			return mScene->mRegistry.get<T>(mEntityHandle);
		}

		template<typename T>
		bool hasComponent() {
			return mScene->mRegistry.any_of<T>(mEntityHandle);
		}

		template<typename T>
		void removeComponent() {
			CR_ASSERT(hasComponent<T>(), "Entity does not have component!");
			mScene->mRegistry.remove<T>(mEntityHandle);
		}

		operator bool() const { return mEntityHandle != entt::null; }
		operator entt::entity() const { return mEntityHandle; }
		operator uint32_t() const { return (uint32_t)mEntityHandle; }

		const std::string& getName() { return getComponent<TagComponent>().tag; }

		bool operator==(const Entity& other) const { return mEntityHandle == other.mEntityHandle && mScene == other.mScene; }
		bool operator!=(const Entity& other) const { return mEntityHandle != other.mEntityHandle && mScene != other.mScene; }
    private:
		Entity(entt::entity handle, Scene* scene) : mEntityHandle(handle), mScene(scene) {}
	private:
		entt::entity mEntityHandle{ entt::null };
		Scene* mScene = nullptr;
	
	friend class Scene;
	};
}
