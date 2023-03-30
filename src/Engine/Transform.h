#pragma once

class Transform
{
public:
	Transform& operator+=(const Transform& other)
	{
		position += glm::quat(rotation) * other.position;
		rotation = rotation * other.rotation;
		scale *= other.scale;
		return *this;
	}

	void Set(const glm::mat4& transformMatrix)
	{
		glm::vec3 scale64Bit;
		glm::quat rotation64Bit;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(objectSpaceToWorldSpace, scale64Bit, rotation64Bit, position, skew, perspective);
		scale = scale64Bit;
		rotation = rotation64Bit;
	}

	glm::mat4 GetMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
	}

	glm::vec3 position = glm::vec3(0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};