#pragma once

#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default camera values
const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 5.0f;
const float DEFAULT_SENSITIVITY = 10.0f;
const float DEFAULT_FOV = 45.0f;
const float DEFAULT_ORTHOGRAPHIC_FACTOR = 5.0f;

// 
const float MIN_FOV = 1.0f;
const float MAX_FOV = 100.0f;

class Camera
{
public:

	// OPTIONS
	float MouseSensitivity = 10.0f;
	float MouseSensitivityDrag = 3.0f;
	float MovementSpeed = 5.0f;

	// Perspective
	float FOV = 45.0f;

	// Orthographic
	float OrthographicFactor;

	bool IsPerspective;

	// EULER ANGLES (should not be directly modified)
	float Pitch = 0.0f;
	float Yaw = -90.0f;

	// VECTORS
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH, bool isPerspective = true)
		: MovementSpeed(DEFAULT_SPEED), MouseSensitivity(DEFAULT_SENSITIVITY), FOV(DEFAULT_FOV), OrthographicFactor(DEFAULT_ORTHOGRAPHIC_FACTOR) {
		Position = position;
		WorldUp = worldUp;
		Yaw = yaw;
		Pitch = pitch;
		IsPerspective = isPerspective;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() {
		if (IsPerspective) {
			return glm::lookAt(Position, Position + Front, WorldUp);
		}
		else {
			return glm::lookAt(Position, Position + Front, WorldUp);
		}
	}

	void processKeyboard(const CameraMovement movement, const double deltaTime) {
		float cameraSpeedAdjusted = MovementSpeed * deltaTime;

		switch (movement)
		{
		case CameraMovement::FORWARD:
			Position += Front * cameraSpeedAdjusted;
			break;
		case CameraMovement::BACKWARD:
			Position -= Front * cameraSpeedAdjusted;
			break;
		case CameraMovement::LEFT:
			Position -= Right * cameraSpeedAdjusted;
			break;
		case CameraMovement::RIGHT:
			Position += Right * cameraSpeedAdjusted;
			break;
		case CameraMovement::UP:
			Position += WorldUp * cameraSpeedAdjusted;
			break;
		case CameraMovement::DOWN:
			Position -= WorldUp * cameraSpeedAdjusted;
			break;
		}
	}

	void processMouseMovement(const double deltaTime, const float deltaX, const float deltaY) {
		Pitch += deltaY * MouseSensitivity * deltaTime;
		Yaw += deltaX * MouseSensitivity * deltaTime;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		if (Yaw > 180.0f)
			Yaw -= 360.0f;
		if (Yaw < -180.0f)
			Yaw += 360.0f;

		updateCameraVectors();
	}

	void processMouseMovement2(const double deltaTime, const float deltaX, const float deltaY) {
		// Negate because the drag movement is in the opposite direction
		Position -= Right * (float)(deltaX * MouseSensitivityDrag * deltaTime);
		Position -= Up * (float)(deltaY * MouseSensitivityDrag * deltaTime);
	}

	void processMouseScroll(const double yoffset) {
		if (IsPerspective) {
			if (FOV >= MIN_FOV && FOV <= MAX_FOV)
				FOV -= yoffset;

			if (FOV < MIN_FOV)
				FOV = MIN_FOV;
			else if (FOV > MAX_FOV)
				FOV = MAX_FOV;
		}
		else {
			OrthographicFactor -= yoffset / 5;
		}
	}

private:
	// Calculates the front (and right and up) vectors from the camera's updated Euler Angles
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Update right and up
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};

