#pragma once

class Camera
{
public:
	void MoveBy(float distance); // ����������� ������/�����
	void StrafeBy(float distance); // ����������� �����/������
	void RotateLeftRight(float angleInDegrees); // ������� �����/������
	void RotateUpDown(float angleInDegrees); // ������� �����/����

	glm::vec3 GetNormalizedViewVector() const;
	glm::mat4 GetViewMatrix() const;

	glm::vec3 position;
	glm::vec3 viewPoint;
	glm::vec3 upVector;
};