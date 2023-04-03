#include "stdafx.h"
#include "Shader.h"
//-----------------------------------------------------------------------------
static constexpr const char* SHADER_COMMON_HEADER = R"GLSL(
#version 450 core
)GLSL";
//-----------------------------------------------------------------------------
static GLuint CreateShader(RenderSystem& renderSystem, GLenum type, const std::vector<const char*>& source)
{
	const GLuint shader = glCreateShader(type);

	glShaderSource(shader, static_cast<GLsizei>(source.size()), source.data(), nullptr);
	glCompileShader(shader);

	GLint compileStatus = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE) 
	{
		GLint infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		const auto infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, &infoLogLength, infoLog);
		renderSystem.Error("Failed to load shader: " + std::string(infoLog));
		delete[] infoLog;
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}
//-----------------------------------------------------------------------------
static GLuint CreateProgram(RenderSystem& renderSystem, const std::initializer_list<GLuint>& shaders)
{
	const GLuint program = glCreateProgram();

	for (const GLuint shader : shaders)
		glAttachShader(program, shader);
	glLinkProgram(program);

	GLint linkStatus = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) 
	{
		GLint infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		const auto infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(program, infoLogLength, &infoLogLength, infoLog);
		renderSystem.Error("Failed to link program: " + std::string(infoLog));
		delete[] infoLog;
		glDeleteProgram(program);
		return 0;
	}

	for (const GLuint shader : shaders)
		glDetachShader(program, shader);
	return program;
}
//-----------------------------------------------------------------------------
OldShaderProgram::OldShaderProgram(
	RenderSystem& renderSystem,
	const std::string& sharedSource,
	const std::string& vertexSource,
	const std::string& fragmentSource
) 
	: m_renderSystem(renderSystem)
{
	GLuint vertexShader = CreateShader(
		m_renderSystem, 
		GL_VERTEX_SHADER,
		{
			SHADER_COMMON_HEADER,
			sharedSource.c_str(),
			vertexSource.c_str()
		}
	);
	GLuint fragmentShader = CreateShader(
		m_renderSystem,
		GL_FRAGMENT_SHADER,
		{
			SHADER_COMMON_HEADER,
			sharedSource.c_str(),
			fragmentSource.c_str()
		}
	);

	m_program = CreateProgram(m_renderSystem, { vertexShader, fragmentShader });

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
//-----------------------------------------------------------------------------
OldShaderProgram::OldShaderProgram(
	RenderSystem& renderSystem,
	const std::string& sharedSource,
	const std::string& vertexSource,
	const std::string& geometrySource,
	const std::string& fragmentSource
) 
	: m_renderSystem(renderSystem)
{
	GLuint vertexShader = CreateShader(
		m_renderSystem,
		GL_VERTEX_SHADER,
		{
			SHADER_COMMON_HEADER,
			sharedSource.c_str(),
			vertexSource.c_str()
		}
	);
	GLuint geometryShader = CreateShader(
		m_renderSystem,
		GL_GEOMETRY_SHADER,
		{
			SHADER_COMMON_HEADER,
			sharedSource.c_str(),
			geometrySource.c_str()
		}
	);
	GLuint fragmentShader = CreateShader(
		m_renderSystem, 
		GL_FRAGMENT_SHADER,
		{
			SHADER_COMMON_HEADER,
			sharedSource.c_str(),
			fragmentSource.c_str()
		}
	);

	m_program = CreateProgram(m_renderSystem, { vertexShader, geometryShader, fragmentShader });

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}
//-----------------------------------------------------------------------------
OldShaderProgram OldShaderProgram::FromFile(
	RenderSystem& renderSystem,
	const std::string& sharedFilename,
	const std::string& vertexFilename,
	const std::string& fragmentFilename) 
{
	return 
	{
		renderSystem,
		renderSystem.GetFileSystem().Read(sharedFilename),
		renderSystem.GetFileSystem().Read(vertexFilename),
		renderSystem.GetFileSystem().Read(fragmentFilename)
	};
}
//-----------------------------------------------------------------------------
OldShaderProgram OldShaderProgram::FromFile(
	RenderSystem& renderSystem,
	const std::string& sharedFilename,
	const std::string& vertexFilename,
	const std::string& geometryFilename,
	const std::string& fragmentFilename) 
{
	return 
	{
		renderSystem,
		renderSystem.GetFileSystem().Read(sharedFilename),
		renderSystem.GetFileSystem().Read(vertexFilename),
		renderSystem.GetFileSystem().Read(geometryFilename),
		renderSystem.GetFileSystem().Read(fragmentFilename)
	};
}
//-----------------------------------------------------------------------------
OldShaderProgram::~OldShaderProgram() 
{
	if (m_program)
		glDeleteProgram(m_program);
}
//-----------------------------------------------------------------------------
void OldShaderProgram::Bind() const 
{
	glUseProgram(m_program);
}
//-----------------------------------------------------------------------------
GLint OldShaderProgram::GetUniformLocation(const std::string& name) const 
{
	return glGetUniformLocation(m_program, name.c_str());
}
//-----------------------------------------------------------------------------
void OldShaderProgram::SetUniform(const GLint location, const float value) 
{
	glProgramUniform1f(m_program, location, value);
}
//-----------------------------------------------------------------------------
void OldShaderProgram::SetUniform(const GLint location, const glm::vec2& value)
{
	glProgramUniform2fv(m_program, location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void OldShaderProgram::SetUniform(const GLint location, const glm::vec3& value)
{
	glProgramUniform3fv(m_program, location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void OldShaderProgram::SetUniform(const GLint location, const glm::vec4& value) 
{
	glProgramUniform4fv(m_program, location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void OldShaderProgram::SetUniform(GLint location, const glm::mat4& value) 
{
	glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------