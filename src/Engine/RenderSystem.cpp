#include "stdafx.h"
#include "RenderSystem.h"
#include "TranslateToGL.h"
//-----------------------------------------------------------------------------
// Use discrete GPU by default.
extern "C" 
{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

	// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
	__declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
}
//-----------------------------------------------------------------------------
#if defined(_DEBUG)
std::string glDebugInfoText;
void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param) noexcept
{
	std::string msgSource;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             msgSource = "GL_DEBUG_SOURCE_API";             break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: msgSource = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     msgSource = "GL_DEBUG_SOURCE_THIRD_PARTY";     break;
	case GL_DEBUG_SOURCE_APPLICATION:     msgSource = "GL_DEBUG_SOURCE_APPLICATION";     break;
	case GL_DEBUG_SOURCE_OTHER:           msgSource = "GL_DEBUG_SOURCE_OTHER";           break;
	}

	std::string msgType;
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               msgType = "GL_DEBUG_TYPE_ERROR";               break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: msgType = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  msgType = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";  break;
	case GL_DEBUG_TYPE_PORTABILITY:         msgType = "GL_DEBUG_TYPE_PORTABILITY";         break;
	case GL_DEBUG_TYPE_PERFORMANCE:         msgType = "GL_DEBUG_TYPE_PERFORMANCE";         break;
	case GL_DEBUG_TYPE_OTHER:               msgType = "GL_DEBUG_TYPE_OTHER";               break;
	}

	std::string msgSeverity = "DEFAULT";
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_LOW:    msgSeverity = "GL_DEBUG_SEVERITY_LOW";    break;
	case GL_DEBUG_SEVERITY_MEDIUM: msgSeverity = "GL_DEBUG_SEVERITY_MEDIUM"; break;
	case GL_DEBUG_SEVERITY_HIGH:   msgSeverity = "GL_DEBUG_SEVERITY_HIGH";   break;
	}

	std::string logMsg = "glDebugMessage: " + std::string(message) + ", type = " + msgType + ", source = " + msgSource + ", severity = " + msgSeverity;

	if (type == GL_DEBUG_TYPE_ERROR) glDebugInfoText = "WARNING: " + logMsg;
	else                             glDebugInfoText = "ERROR: " + logMsg;
}
#endif // _DEBUG
//-----------------------------------------------------------------------------
void RenderSystem::Init(const RenderCreateInfo& createInfo)
{
#if defined(_DEBUG)
	glDebugMessageCallback(glDebugCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
#endif

	Print("OpenGL: OpenGL device information:");
	Print("    > Vendor:   " + std::string((const char*)glGetString(GL_VENDOR)));
	Print("    > Renderer: " + std::string((const char*)glGetString(GL_RENDERER)));
	Print("    > Version:  " + std::string((const char*)glGetString(GL_VERSION)));
	Print("    > GLSL:     " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

	initializeCapabilities(true);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(createInfo.clearColor.x, createInfo.clearColor.y, createInfo.clearColor.z, 1.0f);
}
//-----------------------------------------------------------------------------
void RenderSystem::Close()
{
	m_cacheFileTextures2D.clear();
}
//-----------------------------------------------------------------------------
void RenderSystem::BeginFrame()
{
#if defined(_DEBUG)
	if (!glDebugInfoText.empty())
	{
		Print(glDebugInfoText);
		glDebugInfoText.clear();
	}
#endif

	if (m_mainFramebufferWidth != GetWindowWidth() || m_mainFramebufferHeight != GetWindowHeight())
	{
		m_mainFramebufferWidth = GetWindowWidth();
		m_mainFramebufferHeight = GetWindowHeight();
		//glViewport(0, 0, m_mainFramebufferWidth, m_mainFramebufferHeight);
	}

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//-----------------------------------------------------------------------------
void RenderSystem::EndFrame()
{
}
//-----------------------------------------------------------------------------
void RenderSystem::SetClearColor(const glm::vec3& color)
{
	glClearColor(color.x, color.y, color.z, 1.0f);
}
//-----------------------------------------------------------------------------
void RenderSystem::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetViewport(int width, int height)
{
	glViewport(0, 0, width, height);
}
//-----------------------------------------------------------------------------
bool RenderSystem::IsReadyUniform(const Uniform& uniform) const
{
	return IsValid(uniform) && uniform.programId == m_cache.CurrentShaderProgram;
}
//-----------------------------------------------------------------------------
std::vector<ShaderAttributeInfo> RenderSystem::GetAttributesInfo(ShaderProgramRef resource) const
{
	if( !IsValid(resource) ) return {};

	int activeAttribsCount = 0;
	glGetProgramiv(*resource, GL_ACTIVE_ATTRIBUTES, &activeAttribsCount);
	int maxNameLength = 0;
	glGetProgramiv(*resource, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLength);

	std::string name;
	name.resize(static_cast<size_t>(maxNameLength));

	std::vector<ShaderAttributeInfo> attribs(static_cast<size_t>(activeAttribsCount));
	for( int i = 0; i < activeAttribsCount; i++ )
	{
		GLint size;
		GLenum type = 0;
		glGetActiveAttrib(*resource, (GLuint)i, maxNameLength, nullptr, &size, &type, name.data());

		attribs[i] = {
			.typeId = type,
			.type = GetAttributeType(type),
			.numType = GetAttributeSize(type),
			.name = name,
			.location = glGetAttribLocation(*resource, name.c_str())
		};
	}

	std::sort(attribs.begin(), attribs.end(), [](const ShaderAttributeInfo& a, const ShaderAttributeInfo& b) {return a.location < b.location; });

	return attribs;
}
//-----------------------------------------------------------------------------
Uniform RenderSystem::GetUniform(ShaderProgramRef program, const char* uniformName) const
{
	if( !IsValid(program) || uniformName == nullptr ) return {};

	if( m_cache.CurrentShaderProgram != *program ) glUseProgram(*program);
	Uniform uniform;
	uniform.location = glGetUniformLocation(*program, uniformName);
	uniform.programId = *program;
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
void RenderSystem::SetUniform(const std::string& uniformName, int value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1i(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, float value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform1f(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), value);
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec2& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform2fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec3& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform3fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::vec4& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniform4fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::mat3& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniformMatrix3fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::SetUniform(const std::string& uniformName, const glm::mat4& value)
{
	assert(m_cache.CurrentShaderProgram > 0);
	glUniformMatrix4fv(glGetUniformLocation(m_cache.CurrentShaderProgram, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
//-----------------------------------------------------------------------------
void RenderSystem::UpdateBuffer(VertexBufferRef vbo, unsigned offset, unsigned vertexCount, unsigned vertexSize, const void* data)
{
	assert(IsValid(vbo));

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);

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
void RenderSystem::UpdateBuffer(IndexBufferRef ibo, unsigned offset, unsigned indexCount, unsigned indexSize, const void* data)
{
	assert(IsValid(ibo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
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
	else if( type == ResourceType::Framebuffer )
	{
		m_cache.CurrentFramebuffer = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(ShaderProgramRef resource)
{
	assert(IsValid(resource));
	if( m_cache.CurrentShaderProgram == *resource ) return;
	m_cache.CurrentShaderProgram = *resource;
	glUseProgram(*resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(VertexBufferRef resource)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentVBO == *resource ) return;
	m_cache.CurrentVBO = *resource;
	glBindBuffer(GL_ARRAY_BUFFER, *resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(IndexBufferRef resource)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentIBO == *resource ) return;
	m_cache.CurrentIBO = *resource;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *resource);
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
void RenderSystem::Bind(Texture2DRef resource, unsigned slot)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentTexture2D[slot] == *resource ) return;
	m_cache.CurrentTexture2D[slot] = *resource;
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, *resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Bind(FramebufferRef resource)
{
	if( !resource ) return;
	assert(IsValid(resource));
	if( m_cache.CurrentFramebuffer == *resource ) return;
	m_cache.CurrentFramebuffer = *resource;
	glBindFramebuffer(GL_FRAMEBUFFER, *resource);
}
//-----------------------------------------------------------------------------
void RenderSystem::Draw(VertexArrayRef vao, PrimitiveTopology primitive)
{
	assert(IsValid(vao));

	if( m_cache.CurrentVAO != *vao )
	{
		m_cache.CurrentVAO = *vao;
		m_cache.CurrentVBO = 0;
		m_cache.CurrentIBO = 0;
		glBindVertexArray(*vao);
		Bind(vao->vbo);
		Bind(vao->ibo);
	}

	if( vao->ibo )
	{
		glDrawElements(TranslateToGL(primitive), (GLsizei)vao->ibo->count, SizeIndexType(vao->ibo->size), nullptr);
	}
	else
	{
		glDrawArrays(TranslateToGL(primitive), 0, (GLsizei)vao->vbo->count);
	}
}
//-----------------------------------------------------------------------------
void RenderSystem::initializeCapabilities(bool print)
{
	if( print ) Print("OpenGL: Capabilities information:");

	GLint openGLValue = 0;
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &openGLValue);
	m_capabilities.maximumTextureDimension = static_cast<uint32_t>(openGLValue);
	if( print ) Print("    > Maximum Texture Dimension = " + std::to_string(m_capabilities.maximumTextureDimension));

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &openGLValue);
	m_capabilities.maximumTextureUnitCount = static_cast<uint32_t>(openGLValue);
	if( print ) Print("    > Maximum Texture Unit Count = " + std::to_string(m_capabilities.maximumTextureUnitCount));

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &openGLValue);
	m_capabilities.maximumUniformBufferSize = static_cast<uint32_t>(openGLValue);
	if( print ) Print("    > Maximum Uniform Buffer Size = " + std::to_string(m_capabilities.maximumUniformBufferSize));
}
//-----------------------------------------------------------------------------