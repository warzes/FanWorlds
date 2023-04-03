#include "stdafx.h"
#include "RenderResource.h"
#include "RenderSystem.h"
//-----------------------------------------------------------------------------
// если какая-то структура была изменена, надо не забыть внести изменения в operator==
static_assert(sizeof(ShaderProgram) == 4, "ShaderProgram changed!!!");
static_assert(sizeof(Uniform) == 8, "Uniform changed!!!");
static_assert(sizeof(VertexBuffer) == 16, "VertexBuffer changed!!!");
static_assert(sizeof(IndexBuffer) == 16, "IndexBuffer changed!!!");
static_assert(sizeof(VertexArray) == 32, "VertexArray changed!!!");
static_assert(sizeof(Texture2D) == 16, "Texture2D changed!!!");
//-----------------------------------------------------------------------------
bool operator==(const ShaderProgram& Left, const ShaderProgram& Right) noexcept
{
	return Left.id == Right.id;
}
//-----------------------------------------------------------------------------
bool operator==(const Uniform& Left, const Uniform& Right) noexcept
{
	return Left.location == Right.location && Left.programId == Right.programId;
}
//-----------------------------------------------------------------------------
bool operator==(const VertexBuffer& Left, const VertexBuffer& Right) noexcept
{

	return Left.count == Right.count && Left.id == Right.id && Left.size == Right.size && Left.usage == Right.usage;
}
//-----------------------------------------------------------------------------
bool operator==(const IndexBuffer& Left, const IndexBuffer& Right) noexcept
{
	return Left.count == Right.count && Left.id == Right.id && Left.size == Right.size && Left.usage == Right.usage;
}
//-----------------------------------------------------------------------------
bool operator==(const VertexArray& Left, const VertexArray& Right) noexcept
{
	return Left.attribsCount == Right.attribsCount && Left.ibo == Right.ibo && Left.id == Right.id && Left.vbo == Right.vbo;
}
//-----------------------------------------------------------------------------
bool operator==(const Texture2D& Left, const Texture2D& Right) noexcept
{
	return Left.format == Right.format && Left.height == Right.height && Left.id == Right.id && Left.width == Right.width;
}
//-----------------------------------------------------------------------------
std::shared_ptr<ShaderProgram> RenderSystem::CreateShaderProgram(const std::string& vertexShaderMemory, const std::string& fragmentShaderMemory)
{
	std::shared_ptr<ShaderProgram> resource;
	const GLuint glShaderVertex = createShader(GL_VERTEX_SHADER, vertexShaderMemory);
	const GLuint glShaderFragment = createShader(GL_FRAGMENT_SHADER, fragmentShaderMemory);

	if( glShaderVertex > 0 && glShaderFragment > 0 )
	{
		resource.reset(new ShaderProgram());
		glAttachShader(resource->id, glShaderVertex);
		glAttachShader(resource->id, glShaderFragment);
		glLinkProgram(resource->id);

		GLint linkStatus = 0;
		glGetProgramiv(resource->id, GL_LINK_STATUS, &linkStatus);
		if( linkStatus == GL_FALSE )
		{
			GLint errorMsgLen;
			glGetProgramiv(resource->id, GL_INFO_LOG_LENGTH, &errorMsgLen);

			std::vector<GLchar> errorInfo(static_cast<size_t>(errorMsgLen));
			glGetProgramInfoLog(resource->id, errorMsgLen, nullptr, &errorInfo[0]);
			Error("OPENGL: Shader program linking failed: " + std::string(&errorInfo[0]));
			resource.reset();
		}
		glDetachShader(resource->id, glShaderVertex);
		glDetachShader(resource->id, glShaderFragment);
	}
	glDeleteShader(glShaderVertex);
	glDeleteShader(glShaderFragment);

	return resource;
}
//-----------------------------------------------------------------------------
unsigned RenderSystem::createShader(GLenum openGLshaderType, const std::string& source)
{
	const char* shaderText = source.data();
	const GLint lenShaderText = static_cast<GLint>(source.size());
	const GLuint shaderId = glCreateShader(openGLshaderType);
	glShaderSource(shaderId, 1, &shaderText, &lenShaderText);
	glCompileShader(shaderId);

	GLint compileStatus = GL_FALSE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
	if( compileStatus == GL_FALSE )
	{
		GLint infoLogSize;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogSize);
		std::vector<GLchar> errorInfo(static_cast<size_t>(infoLogSize));
		glGetShaderInfoLog(shaderId, (GLsizei)errorInfo.size(), nullptr, &errorInfo[0]);
		glDeleteShader(shaderId);

		std::string shaderName;
		switch( openGLshaderType )
		{
		case GL_VERTEX_SHADER: shaderName = "Vertex "; break;
		case GL_FRAGMENT_SHADER: shaderName = "Fragment "; break;
		case GL_GEOMETRY_SHADER: shaderName = "Geometry "; break;
		}
		Error(shaderName + "Shader compilation failed : " + std::string(&errorInfo[0]) + ", Source: " + source);
		return 0;
	}

	return shaderId;
}
//-----------------------------------------------------------------------------