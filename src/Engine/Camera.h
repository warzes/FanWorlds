#pragma once

class Camera
{
public:
	void MoveBy(float distance); // перемещение вперед/назад
	void StrafeBy(float distance); // перемещение влево/вправо
	void RotateLeftRight(float angleInDegrees); // поворот влево/вправо
	void RotateUpDown(float angleInDegrees); // поворот вверх/вниз

	glm::vec3 GetNormalizedViewVector() const;
	glm::mat4 GetViewMatrix() const;

	glm::vec3 GetForwardVector() const; // TODO: проверить
	glm::vec3 GetRightVector() const; // TODO: проверить
	glm::vec3 GetUpVector() const; // TODO: проверить

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
};