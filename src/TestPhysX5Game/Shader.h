#pragma once

#include "MoveOnly.h"

class RenderSystem;

class OldShaderProgram
{
public:
	MOVE_ONLY(OldShaderProgram);

	OldShaderProgram() = delete;
	OldShaderProgram(
		RenderSystem& renderSystem,
		const std::string &sharedSource,
		const std::string &vertexSource,
		const std::string &fragmentSource
	);
	OldShaderProgram(
		RenderSystem& renderSystem,
		const std::string &sharedSource,
		const std::string &vertexSource,
		const std::string &geometrySource,
		const std::string &fragmentSource
	);
	~OldShaderProgram();

	static OldShaderProgram FromFile(
		RenderSystem& renderSystem,
		const std::string &sharedFilename,
		const std::string &vertexFilename,
		const std::string &fragmentFilename
	);

	static OldShaderProgram FromFile(
		RenderSystem& renderSystem,
		const std::string &sharedFilename,
		const std::string &vertexFilename,
		const std::string &geometryFilename,
		const std::string &fragmentFilename
	);

	void Bind() const;

	[[nodiscard]] GLint GetUniformLocation(const std::string& name) const;

	void SetUniform(GLint location, float value);
	void SetUniform(GLint location, const glm::vec2& value);
	void SetUniform(GLint location, const glm::vec3& value);
	void SetUniform(GLint location, const glm::vec4& value);
	void SetUniform(GLint location, const glm::mat4& value);

private:
	RenderSystem& m_renderSystem;
	MoveOnly<GLuint> m_program;
};