#pragma once

#include "Car/Core/Core.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Car {
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top) 
			: mProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), mViewMatrix(1.0f) {
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
		}
		OrthographicCamera(float aspectRatio, float zoomLevel=1.0f)
			: mProjectionMatrix(glm::ortho(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1.0f, 1.0f)), mViewMatrix(1.0f) {
			mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
		}

		void setProjection(float left, float right, float bottom, float top) {
			mProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
			mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
		}

		void setProjection(float aspectRatio, float zoomLevel=1.0f) {
			mProjectionMatrix = (glm::ortho(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, -1.0f, 1.0f));
			mViewMatrix = glm::mat4(1.0f);
			mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
		}

		const glm::vec3& getPosition() const { return mPosition; }
		void setPosition(const glm::vec3& position) { mPosition = position; recalculateViewMatrix(); }

		float getRotation() const { return mRotation; }
		void setRotation(float rotation) { mRotation = rotation; recalculateViewMatrix(); }

		const glm::mat4& getProjectionMatrix() const { return mProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return mViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return mViewProjectionMatrix; }
	private:
		void recalculateViewMatrix() {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), mPosition) *
				glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0, 0, 1));

			mViewMatrix = glm::inverse(transform);
			mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
		}
	private:
		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;
		glm::mat4 mViewProjectionMatrix;

		glm::vec3 mPosition = { 0.0f, 0.0f, 0.0f };
		float mRotation = 0.0f;
	};
}
