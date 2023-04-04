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

	glm::vec3 position;
	glm::vec3 viewPoint;
	glm::vec3 upVector;
};