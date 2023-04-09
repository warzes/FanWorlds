#include "stdafx.h"
#include "RenderResource.h"
#include "RenderSystem.h"
#include "TranslateToGL.h"
#include "STBImageLoader.h"
//-----------------------------------------------------------------------------
Shader::Shader(ShaderType type)
{
	m_handle = glCreateShader(TranslateToGL(type));
	m_ownership = true;
}
//-----------------------------------------------------------------------------
bool ShaderSource::LoadFromFile(const std::string& file)
{
	// TODO: возможно заменить fstream на file?
	m_filename = file;
	//m_path = ; не забыть
	std::ifstream shaderFile(file);
	if( !shaderFile.is_open() )
	{
		//Error("Shader file '" + file + "' not found.");
		return false;
	}
	std::stringstream shader_string;
	shader_string << shaderFile.rdbuf();
	m_src = shader_string.str();
	// Remove the EOF from the end of the string.
	if( m_src[m_src.length() - 1] == EOF )
		m_src.pop_back();
	return true;
}
//-----------------------------------------------------------------------------
ShaderProgramRef RenderSystem::CreateShaderProgram(const ShaderSource& vertexShaderSource, const ShaderSource& fragmentShaderSource)
{
	if( !vertexShaderSource.IsValid() )
	{
		Error("You must provide vertex shader (source is blank).");
		return {};
	}

	if( !fragmentShaderSource.IsValid() )
	{
		Error("You must provide fragment shader (source is blank).");
		return {};
	}

	ShaderRef glShaderVertex   = compileShader(ShaderType::Vertex, vertexShaderSource.GetSource());
	ShaderRef glShaderFragment = compileShader(ShaderType::Fragment, fragmentShaderSource.GetSource());

	ShaderProgramRef resource;
	if( IsValid(glShaderVertex) && IsValid(glShaderFragment) )
	{
		resource.reset(new ShaderProgram());
		glAttachShader(*resource, *glShaderVertex);
		glAttachShader(*resource, *glShaderFragment);
		glLinkProgram(*resource);

		GLint linkStatus = 0;
		glGetProgramiv(*resource, GL_LINK_STATUS, &linkStatus);
		if( linkStatus == GL_FALSE )
		{
			GLint infoLogLength;
			glGetProgramiv(*resource, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::unique_ptr<GLchar> errorInfoText{ new GLchar[infoLogLength + 1] };
			glGetProgramInfoLog(*resource, infoLogLength, nullptr, errorInfoText.get());
			Error("OPENGL: Shader program linking failed: " + std::string(errorInfoText.get()));
			resource.reset();
		}
		glDetachShader(*resource, *glShaderVertex);
		glDetachShader(*resource, *glShaderFragment);
	}

	return resource;
}
//-----------------------------------------------------------------------------
GPUBufferRef RenderSystem::CreateVertexBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	GPUBufferRef resource(new GPUBuffer(BufferType::ArrayBuffer, usage, vertexCount, vertexSize));
	if (!IsValid(resource))
	{
		Error("VertexBuffer create failed!");
		return {};
	}
	glBindBuffer(GL_ARRAY_BUFFER, *resource);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ARRAY_BUFFER, m_cache.CurrentVBO); // restore current vb
	return resource;
}
//-----------------------------------------------------------------------------
GPUBufferRef RenderSystem::CreateIndexBuffer(BufferUsage usage, unsigned indexCount, IndexType indexFormat, const void * data)
{
	const unsigned indexSize = SizeIndexType(indexFormat);
	GPUBufferRef resource(new GPUBuffer(BufferType::ElementArrayBuffer, usage, indexCount, indexSize));
	if (!IsValid(resource))
	{
		Error("IndexBuffer create failed!");
		return {};
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *resource);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, data, TranslateToGL(usage));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cache.CurrentIBO); // restore current ib
	return resource;
}
//-----------------------------------------------------------------------------
VertexArrayRef RenderSystem::CreateVertexArray(GPUBufferRef vbo, GPUBufferRef ibo, const std::vector<VertexAttribute>& attribs)
{
	if (vbo == nullptr || attribs.size() == 0)
	{
		Error("VertexBuffer is null");
		return {};
	}
	if (vbo->type != BufferType::ArrayBuffer)
	{
		Error("vbo is not VertexBuffer valid!");
		return {};
	}
	if (ibo && ibo->type != BufferType::ElementArrayBuffer)
	{
		Error("шbo is not IndexBuffer valid!");
		return {};
	}

	VertexArrayRef resource(new VertexArray(vbo, ibo, static_cast<unsigned>(attribs.size())));
	if (!IsValid(resource))
	{
		Error("VertexArray create failed!");
		return {};
	}

	glBindVertexArray(*resource);
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
VertexArrayRef RenderSystem::CreateVertexArray(GPUBufferRef vbo, GPUBufferRef ibo, ShaderProgramRef shaders)
{
	auto attribInfo = GetAttributesInfo(shaders);
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
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexType indexFormat, const void* indexData, ShaderProgramRef shaders)
{
	assert(IsValid(shaders));

	GeometryBufferRef geom(new GeometryBuffer());

	GPUBufferRef vb = CreateVertexBuffer(usage, vertexCount, vertexSize, vertexData);
	if (!IsValid(vb))
	{
		Error("GeometryBuffer::VertexBuffer create failed!!");
		return {};
	}

	const bool isIndexBuffer = indexCount > 0;
	GPUBufferRef ib = nullptr;
	if (isIndexBuffer)
	{
		ib = CreateIndexBuffer(usage, indexCount, indexFormat, indexData);
		if (!IsValid(ib))
		{
			Error("GeometryBuffer::IndexBuffer create failed!!");
			return {};
		}
	}

	geom->vao = CreateVertexArray(vb, ib, shaders);
	if (!IsValid(geom->vao))
	{
		Error("GeometryBuffer::VertexArray create failed!!");
		return {};
	}
	return geom;
}
//-----------------------------------------------------------------------------
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, unsigned indexCount, IndexType indexFormat, const void* indexData, const std::vector<VertexAttribute>& attribs)
{
	GeometryBufferRef geom(new GeometryBuffer());

	GPUBufferRef vb = CreateVertexBuffer(usage, vertexCount, vertexSize, vertexData);
	if (!IsValid(vb))
	{
		Error("GeometryBuffer::VertexBuffer create failed!!");
		return {};
	}

	const bool isIndexBuffer = indexCount > 0;
	GPUBufferRef ib = nullptr;
	if (isIndexBuffer)
	{
		ib = CreateIndexBuffer(usage, indexCount, indexFormat, indexData);
		if (!IsValid(ib))
		{
			Error("GeometryBuffer::IndexBuffer create failed!!");
			return {};
		}
	}

	geom->vao = CreateVertexArray(vb, ib, attribs);
	if (!IsValid(geom->vao))
	{
		Error("GeometryBuffer::VertexArray create failed!!");
		return {};
	}
	return geom;
}
//-----------------------------------------------------------------------------
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, ShaderProgramRef shaders)
{
	return CreateGeometryBuffer(usage, vertexCount, vertexSize, vertexData, 0, {}, nullptr, shaders);
}
//-----------------------------------------------------------------------------
GeometryBufferRef RenderSystem::CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const std::vector<VertexAttribute>& attribs)
{
	return CreateGeometryBuffer(usage, vertexCount, vertexSize, vertexData, 0, {}, nullptr, attribs);
}
//-----------------------------------------------------------------------------
Texture2DRef RenderSystem::CreateTexture2D(const char* fileName, bool useCache, const Texture2DInfo& textureInfo)
{
	if( useCache )
	{
		auto it = m_cacheFileTextures2D.find(fileName);
		if( it != m_cacheFileTextures2D.end() )
			return it->second;
	}

	Print("Load texture: " + std::string(fileName));

	STBImageLoader imageLoad(fileName);
	if( imageLoad.isValid == false )
	{
		Error("Image loading failed! Filename='" + std::string(fileName) + "'");
		return {};
	}
	const Texture2DCreateInfo createInfo = {
		.format = imageLoad.imageFormat,
		.width = static_cast<uint16_t>(imageLoad.width),
		.height = static_cast<uint16_t>(imageLoad.height),
		.pixelData = imageLoad.pixelData,
	};

	m_cacheFileTextures2D[fileName] = CreateTexture2D(createInfo, textureInfo);
	return m_cacheFileTextures2D[fileName];
}
//-----------------------------------------------------------------------------
Texture2DRef RenderSystem::CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo)
{
	Texture2DRef resource(new Texture2D(createInfo.width, createInfo.height, createInfo.format));
	// gen texture res
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *resource);

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
FramebufferRef RenderSystem::CreateFramebuffer(FramebufferAttachment attachment, Texture2DRef texture)
{
	assert(IsValid(texture));

	FramebufferRef resource(new Framebuffer());
	resource->texture = texture;
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
	glFramebufferTexture2D(GL_FRAMEBUFFER, TranslateToGL(attachment), GL_TEXTURE_2D, *resource->texture, 0);
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( GL_FRAMEBUFFER_COMPLETE != result )
	{
		Error("Framebuffer is not complete.");
		return {};
	}
	// restore prev state
	glBindFramebuffer(GL_FRAMEBUFFER, m_cache.CurrentFramebuffer);
	return resource;
}
//-----------------------------------------------------------------------------
ShaderRef RenderSystem::compileShader(ShaderType type, const std::string& source)
{
	const char* shaderText = source.data();
	const GLint lenShaderText = static_cast<GLint>(source.size());

	ShaderRef shader(new Shader(type));
	glShaderSource(*shader, 1, &shaderText, &lenShaderText);
	glCompileShader(*shader);

	GLint compileStatus = GL_FALSE;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &compileStatus);
	if( compileStatus == GL_FALSE )
	{
		GLint infoLogLength;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::unique_ptr<GLchar> errorInfoText{ new GLchar[infoLogLength + 1] };
		glGetShaderInfoLog(*shader, infoLogLength, nullptr, errorInfoText.get());

		const std::string shaderName = ConvertToStr(type);
		Error(shaderName + " Shader compilation failed : " + std::string(errorInfoText.get()) + ", Source: " + source);
		return {};
	}

	return shader;
}
//-----------------------------------------------------------------------------