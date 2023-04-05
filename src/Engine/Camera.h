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

	glm::vec3 GetForwardVector() const; // TODO: ���������
	glm::vec3 GetRightVector() const; // TODO: ���������
	glm::vec3 GetUpVector() const; // TODO: ���������

	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
};