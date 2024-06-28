#include "Car/Scene/Scene.hpp"
#include "Car/Scene/Componets.hpp"
#include "Car/Scene/Entity.hpp"
#include "Car/Scene/UUID.hpp"


namespace Car {
    Scene::Scene() {

    }

    Scene::~Scene() {

    }

    Ref<Scene> Scene::copy(Ref<Scene> scene) {
        return scene;
    }

    Entity Scene::createEntity(const std::string& name) {
        return createEntityWithUUID(UUID(), name);
    }

    Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name) {
        // Entity ent(handle, this);
        // ent.addComponent<TagComponent>(name);
        // ent.addComponent<UUIDComponent>(uuid);

        Entity entity = { mRegistry.create(), this };

		entity.addComponent<IDComponent>(uuid);
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;

		mEntityMap[uuid] = entity;

		return entity;
    }

    Entity Scene::queryEntityByName(const std::string& name) {
        auto view = mRegistry.view<TagComponent>();
        for (auto entity : view) {
            TagComponent tag = view.get<TagComponent>(entity);
            if (tag.tag == name) {
                return { entity, this };
            }
        }

        CR_CORE_WARN("No entity was found with the tag `{0}`", name);

        return {};
    }

    Entity Scene::queryEntityByUUID(UUID uuid) {
        CR_ASSERT(mEntityMap.find(uuid) == mEntityMap.end(), "an Entity with this uuid wasnt created");
        return { mEntityMap[uuid], this };
    }
}

/*
class Scene {
	public:
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
*/