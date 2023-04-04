#include "stdafx.h"
#include "RenderResource.h"
#include "RenderSystem.h"
#include "TranslateToGL.h"

//-----------------------------------------------------------------------------
// если какая-то структура была изменена, надо не забыть внести изменения в operator==
static_assert(sizeof(ShaderProgram) == 4, "ShaderProgram changed!!!");
static_assert(sizeof(Uniform) == 8, "Uniform changed!!!");
static_assert(sizeof(VertexBuffer) == 16, "VertexBuffer changed!!!");
static_assert(sizeof(IndexBuffer) == 16, "IndexBuffer changed!!!");
static_assert(sizeof(VertexArray) == 48, "VertexArray changed!!!");
static_assert(sizeof(Texture2D) == 16, "Texture2D changed!!!");
//-----------------------------------------------------------------------------
bool operator==(ShaderProgramRef Left, ShaderProgramRef Right) noexcept
{
	return Left->id == Right->id;
}
//-----------------------------------------------------------------------------
bool operator==(const Uniform& Left, const Uniform& Right) noexcept
{
	return Left.location == Right.location && Left.programId == Right.programId;
}
//-----------------------------------------------------------------------------
bool operator==(VertexBufferRef Left, VertexBufferRef Right) noexcept
{
	return Left->count == Right->count && Left->id == Right->id && Left->size == Right->size && Left->usage == Right->usage;
}
//-----------------------------------------------------------------------------
bool operator==(IndexBufferRef Left, IndexBufferRef Right) noexcept
{
	return Left->count == Right->count && Left->id == Right->id && Left->size == Right->size && Left->usage == Right->usage;
}
//-----------------------------------------------------------------------------
bool operator==(VertexArrayRef Left, VertexArrayRef Right) noexcept
{
	return Left->attribsCount == Right->attribsCount && Left->ibo == Right->ibo && Left->id == Right->id && Left->vbo == Right->vbo;
}
//-----------------------------------------------------------------------------
bool operator==(Texture2DRef Left, Texture2DRef Right) noexcept
{
	return Left->format == Right->format && Left->height == Right->height && Left->id == Right->id && Left->width == Right->width;
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
std::shared_ptr<VertexBuffer> RenderSystem::CreateVertexBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	std::shared_ptr<VertexBuffer> resource(new VertexBuffer(usage, vertexCount, vertexSize));
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
std::shared_ptr<VertexArray> RenderSystem::CreateVertexArray(std::shared_ptr<VertexBuffer> vbo, std::shared_ptr<IndexBuffer> ibo, const std::vector<VertexAttribute>& attribs)
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
	Bind(resource->vbo);
	for (size_t i = 0; i < attribs.size(); i++)
	{
		Bind(attribs[i]);
	}
	Bind(resource->ibo);

	glBindVertexArray(m_cache.CurrentVAO); // restore VAO
	glBindVertexArray(m_cache.CurrentVBO); // restore VBO
	glBindVertexArray(m_cache.CurrentIBO); // restore IBO

	return resource;
}
//-----------------------------------------------------------------------------
std::shared_ptr<VertexArray> RenderSystem::CreateVertexArray(std::shared_ptr<VertexBuffer> vbo, std::shared_ptr<IndexBuffer> ibo, std::shared_ptr<ShaderProgram> shaders)
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
std::shared_ptr<GeometryBuffer> RenderSystem::CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexBufferFormat indexFormat, const void* indexData, std::shared_ptr<ShaderProgram> shaders)
{
	assert(IsValid(shaders));

	std::shared_ptr<GeometryBuffer> geom(new GeometryBuffer());

	geom->vb = CreateVertexBuffer(usage, vertexCount, vertexSize, vertexData);
	if (!IsValid(geom->vb))
	{
		Error("GeometryBuffer::VertexBuffer create failed!!");
		return {};
	}

	const bool isIndexBuffer = indexCount > 0;
	if (isIndexBuffer)
	{
		geom->ib = CreateIndexBuffer(usage, indexCount, indexFormat, indexData);
		if (!IsValid(geom->ib))
		{
			Error("GeometryBuffer::IndexBuffer create failed!!");
			return {};
		}
	}

	geom->vao = CreateVertexArray(geom->vb, geom->ib, shaders);
	if (!IsValid(geom->vao))
	{
		Error("GeometryBuffer::VertexArray create failed!!");
		return {};
	}
	return geom;
}
//-----------------------------------------------------------------------------
std::shared_ptr<GeometryBuffer> RenderSystem::CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexBufferFormat indexFormat, const void* indexData, const std::vector<VertexAttribute>& attribs)
{
	std::shared_ptr<GeometryBuffer> geom(new GeometryBuffer());

	geom->vb = CreateVertexBuffer(usage, vertexCount, vertexSize, vertexData);
	if (!IsValid(geom->vb))
	{
		Error("GeometryBuffer::VertexBuffer create failed!!");
		return {};
	}

	const bool isIndexBuffer = indexCount > 0;
	if (isIndexBuffer)
	{
		geom->ib = CreateIndexBuffer(usage, indexCount, indexFormat, indexData);
		if (!IsValid(geom->ib))
		{
			Error("GeometryBuffer::IndexBuffer create failed!!");
			return {};
		}
	}

	geom->vao = CreateVertexArray(geom->vb, geom->ib, attribs);
	if (!IsValid(geom->vao))
	{
		Error("GeometryBuffer::VertexArray create failed!!");
		return {};
	}
	return geom;
}
//-----------------------------------------------------------------------------
std::shared_ptr<GeometryBuffer> RenderSystem::CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, std::shared_ptr<ShaderProgram> shaders)
{
	return CreateGeometryBuffer(usage, vertexCount, vertexSize, vertexData, 0, {}, nullptr, shaders);
}
//-----------------------------------------------------------------------------
std::shared_ptr<GeometryBuffer> RenderSystem::CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const std::vector<VertexAttribute>& attribs)
{
	return CreateGeometryBuffer(usage, vertexCount, vertexSize, vertexData, 0, {}, nullptr, attribs);
}
//-----------------------------------------------------------------------------
std::shared_ptr<Texture2D> RenderSystem::CreateTexture2D(const char* fileName, bool useCache, const Texture2DInfo& textureInfo)
{
	if( useCache )
	{
		auto it = m_cacheFileTextures2D.find(fileName);
		if( it != m_cacheFileTextures2D.end() )
			return it->second;
	}

	Print("Load texture: " + std::string(fileName));

	//stbi_set_flip_vertically_on_load(verticallyFlip ? 1 : 0);
	const int desiredСhannels = STBI_default;
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	stbi_uc* pixelData = stbi_load(fileName, &width, &height, &nrChannels, desiredСhannels);
	if( !pixelData || nrChannels < STBI_grey || nrChannels > STBI_rgb_alpha || width == 0 || height == 0 )
	{
		Error("Image loading failed! Filename='" + std::string(fileName) + "'");
		stbi_image_free((void*)pixelData);
		return {};
	}
	Texture2DCreateInfo createInfo;
	{
		if( nrChannels == STBI_grey ) createInfo.format = TexelsFormat::R_U8;
		else if( nrChannels == STBI_grey_alpha ) createInfo.format = TexelsFormat::RG_U8;
		else if( nrChannels == STBI_rgb ) createInfo.format = TexelsFormat::RGB_U8;
		else if( nrChannels == STBI_rgb_alpha ) createInfo.format = TexelsFormat::RGBA_U8;

		createInfo.width = static_cast<uint16_t>(width);
		createInfo.height = static_cast<uint16_t>(height);
		createInfo.pixelData = pixelData;
	}
	auto texture = CreateTexture2D(createInfo, textureInfo);

	stbi_image_free((void*)pixelData);
	m_cacheFileTextures2D[fileName] = texture;
	return texture;
}
//-----------------------------------------------------------------------------
std::shared_ptr<Texture2D> RenderSystem::CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo)
{
	std::shared_ptr<Texture2D> resource(new Texture2D(createInfo.width, createInfo.height, createInfo.format));
	// gen texture res
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, resource->id);

	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TranslateToGL(textureInfo.wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TranslateToGL(textureInfo.wrapT));

	// set texture filtering parameters
	TextureMinFilter minFilter = textureInfo.minFilter;
	if( !textureInfo.mipmap )
	{
		if( textureInfo.minFilter == TextureMinFilter::NearestMipmapNearest ) minFilter = TextureMinFilter::Nearest;
		else if( textureInfo.minFilter != TextureMinFilter::Nearest ) minFilter = TextureMinFilter::Linear;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TranslateToGL(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TranslateToGL(textureInfo.magFilter));

	// set texture format
	GLenum format = GL_RGB;
	GLint internalFormat = GL_RGB;
	GLenum oglType = GL_UNSIGNED_BYTE;
	if( !GetTextureFormatType(createInfo.format, GL_TEXTURE_2D, format, internalFormat, oglType) )
	{
		resource.reset();
		glBindTexture(GL_TEXTURE_2D, m_cache.CurrentTexture2D[0]);
		return {};
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, (GLsizei)resource->width, (GLsizei)resource->height, 0, format, oglType, createInfo.pixelData);

	if( textureInfo.mipmap )
		glGenerateMipmap(GL_TEXTURE_2D);

	// restore prev state
	glBindTexture(GL_TEXTURE_2D, m_cache.CurrentTexture2D[0]);

	return resource;
}
//-----------------------------------------------------------------------------
bool RenderSystem::IsReadyUniform(const Uniform& uniform)
{
	return IsValid(uniform) && uniform.programId == m_cache.CurrentShaderProgram;
}
//-----------------------------------------------------------------------------
std::vector<ShaderAttribInfo> RenderSystem::GetAttribInfo(std::shared_ptr<ShaderProgram> resource)
{
	if( !IsValid(resource) ) return {};

	int activeAttribs = 0;
	glGetProgramiv(resource->id, GL_ACTIVE_ATTRIBUTES, &activeAttribs);
	int maxLength = 0;
	glGetProgramiv(resource->id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

	std::string name;
	name.resize(static_cast<size_t>(maxLength));

	std::vector<ShaderAttribInfo> attribs(static_cast<size_t>(activeAttribs));
	for( int i = 0; i < activeAttribs; i++ )
	{
		GLint size;
		GLenum type = 0;
		glGetActiveAttrib(resource->id, (GLuint)i, maxLength, nullptr, &size, &type, name.data());

		attribs[i] = {
			.typeId = type,
			.name = name,
			.location = glGetAttribLocation(resource->id, name.c_str())
		};
	}

	std::sort(attribs.begin(), attribs.end(), [](const ShaderAttribInfo& a, const ShaderAttribInfo& b) {return a.location < b.location; });

	return attribs;
}
//-----------------------------------------------------------------------------
Uniform RenderSystem::GetUniform(std::shared_ptr<ShaderProgram> program, const char* uniformName)
{
	if( !IsValid(program) || uniformName == nullptr ) return {};

	if( m_cache.CurrentShaderProgram != program->id ) glUseProgram(program->id);
	Uniform uniform;
	uniform.location = glGetUniformLocation(program->id, uniformName);
	uniform.programId = program->id;
	glUseProgram(m_cache.CurrentShaderProgram); // restore prev shader program
	return uniform;
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, int value)
{
	assert(IsReadyUniform(uniform));
	glUniform1i(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, float value)
{
	assert(IsReadyUniform(uniform));
	glUniform1f(uniform.location, value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec2& value)
{
	assert(IsReadyUniform(uniform));
	glUniform2fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec3& value)
{
	assert(IsReadyUniform(uniform));
	glUniform3fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::vec4& value)
{
	assert(IsReadyUniform(uniform));
	glUniform4fv(uniform.location, 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::mat3& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const Uniform& uniform, const glm::mat4& value)
{
	assert(IsReadyUniform(uniform));
	glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::UpdateVertexBuffer(std::shared_ptr<VertexBuffer> vbo, unsigned offset, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	assert(IsValid(vbo));

	glBindBuffer(GL_ARRAY_BUFFER, vbo->id);

	if( vbo->count != vertexCount || vbo->size != vertexSize || vbo->usage != ResourceUsage::Dynamic )
	{
		glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, data, TranslateToGL(ResourceUsage::Dynamic));
		vbo->usage = ResourceUsage::Dynamic;
	}
	else
	{
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount * vertexSize, data);
	}
	vbo->count = vertexCount;
	vbo->size = vertexSize;

	glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO); // restore current vb
}
//-----------------------------------------------------------------------------
void RenderSystem::UpdateIndexBuffer(std::shared_ptr<IndexBuffer> ibo, unsigned offset, unsigned indexCount, unsigned indexSize, const void* data)
{
	assert(IsValid(ibo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->id);
	if( ibo->count != indexCount || ibo->size != indexSize || ibo->usage != ResourceUsage::Dynamic )
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, TranslateToGL(ResourceUsage::Dynamic));
		ibo->usage = ResourceUsage::Dynamic;
	}
	else
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, indexCount * indexSize, data);
	}
	ibo->count = indexCount;
	ibo->size = indexSize;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO); // restore current ib
}
//-----------------------------------------------------------------------------
void RenderSystem::ResetState(ResourceType type)
{
	if( type == ResourceType::ShaderProgram )
	{
		m_cache.CurrentShaderProgram = 0;
		glUseProgram(0);
	}
	else if( type == ResourceType::VertexBuffer )
	{
		m_cache.CurrentVBO = 0;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if( type == ResourceType::IndexBuffer )
	{
		m_cache.CurrentIBO = 0;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else if( type == ResourceType::VertexArray )
	{
		ResetState(ResourceType::VertexBuffer);
		ResetState(ResourceType::IndexBuffer);
		m_cache.CurrentVAO = 0;
		glBindVertexArray(0);
	}
	else if( type == ResourceType::Texture2D )
	{
		for( unsigned i = 0; i < MaxBindingTextures; i++ )
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
			m_cache.CurrentTexture2D[i] = 0;
		}
		glActiveTexture(GL_TEXTURE0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(std::shared_ptr<ShaderProgram> resource)
{
	assert(IsValid(resource));
	if( m_cache.CurrentShaderProgram == resource->id ) return;
	m_cache.CurrentShaderProgram = resource->id;
	glUseProgram(resource->id);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(std::shared_ptr<VertexBuffer> resource)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentVBO == resource->id ) return;
	m_cache.CurrentVBO = resource->id;
	glBindBuffer(GL_ARRAY_BUFFER, resource->id);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(std::shared_ptr<IndexBuffer> resource)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentIBO == resource->id ) return;
	m_cache.CurrentIBO = resource->id;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, resource->id);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(const VertexAttribute& attribute)
{
	const GLuint oglLocation = static_cast<GLuint>(attribute.location);
	glEnableVertexAttribArray(oglLocation);
	glVertexAttribPointer(
		oglLocation,
		attribute.size,
		GL_FLOAT,
		(GLboolean)(attribute.normalized ? GL_TRUE : GL_FALSE),
		attribute.stride,
		attribute.offset);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(std::shared_ptr<Texture2D> resource, unsigned slot)
{
	if (!resource) return;
	assert(IsValid(resource));
	if( m_cache.CurrentTexture2D[slot] == resource->id ) return;
	m_cache.CurrentTexture2D[slot] = resource->id;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, resource->id);
}
//-----------------------------------------------------------------------------
void RenderSystem::Draw(std::shared_ptr<VertexArray> vao, PrimitiveTopology primitive)
{
	assert(IsValid(vao));
	if( m_cache.CurrentVAO != vao->id )
	{
		m_cache.CurrentVAO = vao->id;
		m_cache.CurrentVBO = 0;
		m_cache.CurrentIBO = 0;
		glBindVertexArray(vao->id);
		Bind(vao->vbo);
		Bind(vao->ibo);
	}

	if( vao->ibo )
	{
		const GLenum indexSizeType = (GLenum)(vao->ibo->size == sizeof(uint32_t) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT);
		glDrawElements(TranslateToGL(primitive), (GLsizei)vao->ibo->count, indexSizeType, nullptr);
	}
	else
	{
		glDrawArrays(TranslateToGL(primitive), 0, (GLsizei)vao->vbo->count);
	}
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