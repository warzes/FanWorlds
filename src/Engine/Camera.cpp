#include "stdafx.h"
#include "Camera.h"

void Camera::MoveBy(float distance)
{
	const glm::vec3 vOffset = GetNormalizedViewVector() * distance;
	position += vOffset;
	viewPoint += vOffset;
}

void Camera::StrafeBy(float distance)
{
	const glm::vec3 strafeVector = glm::normalize(glm::cross(GetNormalizedViewVector(), upVector)) * distance;
	position += strafeVector;
	viewPoint += strafeVector;
}

void Camera::RotateLeftRight(float angleInDegrees)
{
	const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::vec4 rotatedViewVector = rotationMatrix * glm::vec4(GetNormalizedViewVector(), 0.0f);
	viewPoint = position + glm::vec3(rotatedViewVector);
}

void Camera::RotateUpDown(float angleInDegrees)
{
	const glm::vec3 viewVector = GetNormalizedViewVector();
	const glm::vec3 viewVectorNoY = glm::normalize(glm::vec3(viewVector.x, 0.0f, viewVector.z));

	float currentAngleDegrees = glm::degrees(acos(glm::dot(viewVectorNoY, viewVector)));
	if (viewVector.y < 0.0f) currentAngleDegrees = -currentAngleDegrees;

	const float newAngleDegrees = currentAngleDegrees + angleInDegrees;
	if (newAngleDegrees > -85.0f && newAngleDegrees < 85.0f)
	{
		glm::vec3 rotationAxis = glm::cross(GetNormalizedViewVector(), upVector);
		rotationAxis = glm::normalize(rotationAxis);

		const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees), rotationAxis);
		const glm::vec4 rotatedViewVector = rotationMatrix * glm::vec4(GetNormalizedViewVector(), 0.0f);

		viewPoint = position + glm::vec3(rotatedViewVector);
	}
}

glm::vec3 Camera::GetNormalizedViewVector() const
{
	return glm::normalize(viewPoint - position);
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(position, viewPoint, upVector);
}
