#pragma once

#include "Car/Core/Core.hpp"
#include "Car/Scene/UUID.hpp"

#include <entt/entt.hpp>


namespace Car {
	class Entity;

	class Scene {
	public:
		static Ref<Scene> copy(Ref<Scene> other);

		Entity createEntity(const std::string& name=std::string());
		Entity createEntityWithUUID(UUID uuid, const std::string& name=std::string());

		Entity queryEntityByName(const std::string& name);
		Entity queryEntityByUUID(UUID uuid);

		Entity duplicateEntity(Entity entity);

		template<typename... Components>
		auto getAllEntitiesWith() { return mRegistry.view<Components...>(); }

		static Ref<Scene> Create();
		~Scene();
	private:
		Scene();
	private:
		entt::registry mRegistry;

		std::unordered_map<UUID, entt::entity> mEntityMap;

		friend class Entity;
	};

}