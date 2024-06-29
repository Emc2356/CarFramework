#pragma once

#include <Car/Core/Core.hpp>

#include <Car/Renderer/Texture2D.hpp>

#include <Car/Geometry/Rect.hpp>
#include "Car/Scene/UUID.hpp"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>


/*
struct TransformData {
	vec3 translation;
	float __padding0;
	vec3 rotation;
	float __padding1;
	vec3 scale;
	float __padding2;
};
*/


namespace Car {
	struct IDComponent {
		UUID uuid;
	};

	struct TransformComponent {
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const glm::vec3& translation)
			: translation(translation) {}

		glm::mat4 GetTransform() const {
			glm::mat4 rotation = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation) * rotation * glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct TagComponent {
		std::string tag;
	};


	struct SpriteComponent {
    public:
		SpriteComponent() = default;
        SpriteComponent(const Ref<Texture2D>& texture, uint32_t x, uint32_t y, uint32_t w, uint32_t h)  : texture(texture), rect(x, y, w, h) {}
        SpriteComponent(const Ref<Texture2D>& texture, const Rect& r) : texture(texture), rect(r) {}
		SpriteComponent(const Ref<Texture2D>& texture) : texture(texture), rect(0, 0, texture->getWidth(), texture->getHeight()) {}
    public:
        Ref<Texture2D> texture;
        Rect rect;
	};
}
