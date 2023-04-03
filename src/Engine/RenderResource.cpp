#include "stdafx.h"
#include "RenderResource.h"
#include "RenderSystem.h"
#include "TranslateToGL.h"

идея игры такая
- мир из тайлов и блоков, с возможностью модить мир в игре (для меня)
- глянуть текстуры в уе5 (бридж)
- визуально для мира - https://www.youtube.com/watch?v=tAr8zYDdI8s
- визуально для персов - https://www.youtube.com/@AIAdev/videos
- атмосфера - соулсы, хир энд хантер и берсерк
- dw - sample - framework

//-----------------------------------------------------------------------------
// если какая-то структура была изменена, надо не забыть внести изменения в operator==
static_assert(sizeof(ShaderProgram) == 4, "ShaderProgram changed!!!");
static_assert(sizeof(Uniform) == 8, "Uniform changed!!!");
static_assert(sizeof(GPUBuffer) == 16, "VertexBuffer changed!!!");
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
bool operator==(const GPUBuffer& Left, const GPUBuffer& Right) noexcept
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
	const GLuint glShaderVertex = createShader(GL_VERTEX_SHADER, vertexShaderMemory);
	const GLuint glShaderFragment = createShader(GL_FRAGMENT_SHADER, fragmentShaderMemory);

	std::shared_ptr<ShaderProgram> resource;
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
std::shared_ptr<GPUBuffer> RenderSystem::CreateVertexBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	std::shared_ptr<GPUBuffer> resource(new GPUBuffer(usage, vertexCount, vertexSize));
	if (!IsValid(resource))
	{
		Error("VertexBuffer create failed!");
		return {};
	}
	glBindBuffer(GL_ARRAY_BUFFER, resource->id);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO); // restore current vb
	return resource;
}
//-----------------------------------------------------------------------------
std::shared_ptr<IndexBuffer> RenderSystem::CreateIndexBuffer(ResourceUsage usage, unsigned indexCount, IndexBufferFormat indexFormat, const void * data)
{
	const unsigned indexSize = SizeIndexBuffer(indexFormat);
	std::shared_ptr<IndexBuffer> resource(new IndexBuffer(usage, indexCount, indexSize));
	if (!IsValid(resource))
	{
		Error("IndexBuffer create failed!");
		return {};
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO); // restore current ib
	return resource;
}
//-----------------------------------------------------------------------------
std::shared_ptr<VertexArray> RenderSystem::CreateVertexArray(std::shared_ptr<GPUBuffer> vbo, std::shared_ptr<IndexBuffer> ibo, const std::vector<VertexAttribute>& attribs)
{
	if (vbo == nullptr || attribs.size() == 0)
	{
		Error("VertexBuffer is null");
		return {};
	}

	std::shared_ptr<VertexArray> resource(new VertexArray(vbo, ibo, static_cast<unsigned>(attribs.size())));
	if (!IsValid(resource))
	{
		Error("VertexArray create failed!");
		return {};
	}

	glBindVertexArray(resource->id);

	Bind(resource.vbo);
	for (size_t i = 0; i < attribs.size(); i++)
	{
		Bind(attribs[i]);
	}

	if (resource.ibo) Bind(resource.ibo);

	glBindVertexArray(m_cache.CurrentVAO); // restore VAO
	glBindVertexArray(m_cache.CurrentVBO); // restore VBO
	glBindVertexArray(m_cache.CurrentIBO); // restore IBO

	return resource;
}
//-----------------------------------------------------------------------------
std::shared_ptr<VertexArray> RenderSystem::CreateVertexArray(std::shared_ptr<GPUBuffer> vbo, std::shared_ptr<IndexBuffer> ibo, std::shared_ptr<ShaderProgram> shaders)
{
	assert(IsValid(shaders));
	if (!IsValid(shaders)) return {};

	auto attribInfo = GetAttribInfo(shaders);
	if (attribInfo.empty()) return {};

	size_t offset = 0;
	std::vector<VertexAttribute> attribs(attribInfo.size());
	for (size_t i = 0; i < attribInfo.size(); i++)
	{
		// TODO: gl_VertexID считается атрибутом, но при этом почему-то location у него равен -1
		// в будущем пофиксить, а пока не юзать
		assert(attribInfo[i].location > -1);
		attribs[i].location = (unsigned)attribInfo[i].location; // TODO: attribInfo[i].location может быть -1, тогда как в attribs[i].location unsigned. нужно исправить
		attribs[i].normalized = false;

		size_t sizeType = 0;
		switch (attribInfo[i].typeId)
		{
		case GL_FLOAT:
			attribs[i].size = 1;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC2:
			attribs[i].size = 2;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC3:
			attribs[i].size = 3;
			sizeType = attribs[i].size * sizeof(float);
			break;
		case GL_FLOAT_VEC4:
			attribs[i].size = 4;
			sizeType = attribs[i].size * sizeof(float);
			break;

		default:
			Error("Shader attribute type: " + attribInfo[i].name + " not support!");
			return {};
		}

		attribs[i].offset = (void*)+offset;
		offset += sizeType;
	}
	for (size_t i = 0; i < attribs.size(); i++)
	{
		attribs[i].stride = (int)offset;
	}

	return CreateVertexArray(vbo, ibo, attribs);
}
//-----------------------------------------------------------------------------
std::shared_ptr<Texture2D> RenderSystem::CreateTexture2D(const char* fileName, bool useCache, const Texture2DInfo& textureInfo)
{
	return std::shared_ptr<Texture2D>();
}
//-----------------------------------------------------------------------------
bool RenderSystem::IsReadyUniform(const Uniform& uniform)
{
	return IsValid(uniform) && uniform.programId == m_cache.CurrentShaderProgram;
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