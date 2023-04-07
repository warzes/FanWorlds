#pragma once

#include "BaseClass.h"
#include "OpenGLUtil.h"
#include "RenderResource.h"
#include "Capabilities.h"

constexpr int MaxBindingTextures = 16;

struct RenderCreateInfo
{
	glm::vec3 clearColor = glm::vec3{ 0.2f, 0.4f, 0.9f };
};

// TODO: очистка ресурсов из кеша

class RenderSystem : public BaseClass
{
	friend class EngineDevice;
public:
	//-------------------------------------------------------------------------
	// Core
	//-------------------------------------------------------------------------
	void Init(const RenderCreateInfo& createInfo);
	void Close();
	const Capabilities& GetCapabilities() const { return m_capabilities; }

	void BeginFrame();
	void EndFrame();

	//-------------------------------------------------------------------------
	// Current Frame Set
	//-------------------------------------------------------------------------

	void SetClearColor(const glm::vec3& color);
	void Clear();
	void SetViewport(int width, int height);

	//-------------------------------------------------------------------------
	// Create Render Resource
	//-------------------------------------------------------------------------
	ShaderProgramRef CreateShaderProgram(const ShaderSource& vertexShaderSource, const ShaderSource& fragmentShaderSource);
	VertexBufferRef CreateVertexBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data);
	IndexBufferRef CreateIndexBuffer(ResourceUsage usage, unsigned indexCount, IndexType indexFormat, const void* data);
	VertexArrayRef CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, const std::vector<VertexAttribute>& attribs);
	VertexArrayRef CreateVertexArray(VertexBufferRef vbo, IndexBufferRef ibo, ShaderProgramRef shaders);

	GeometryBufferRef CreateGeometryBuffer(ResourceUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexType indexFormat, const void* indexData,
		ShaderProgramRef shaders);
	GeometryBufferRef CreateGeometryBuffer(ResourceUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexType indexFormat, const void* indexData,
		const std::vector<VertexAttribute>& attribs);
	GeometryBufferRef CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, ShaderProgramRef shaders);
	GeometryBufferRef CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const std::vector<VertexAttribute>& attribs);

	Texture2DRef CreateTexture2D(const char* fileName, bool useCache = true, const Texture2DInfo& textureInfo = {});
	Texture2DRef CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo = {});

	FramebufferRef CreateFramebuffer(FramebufferAttachment attachment, Texture2DRef texture);

	//-------------------------------------------------------------------------
	// Validation
	//-------------------------------------------------------------------------
	inline bool IsValid(ShaderRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(ShaderProgramRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(const Uniform& uniform) const { return uniform.location >= 0; }
	inline bool IsValid(VertexBufferRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(IndexBufferRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(VertexArrayRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(Texture2DRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(GeometryBufferRef resource) const { return IsValid(resource->vao); }
	inline bool IsValid(FramebufferRef resource) const { return resource && resource->IsValid(); }
	bool IsReadyUniform(const Uniform& uniform) const;

	//-------------------------------------------------------------------------
	// Shader Op
	//-------------------------------------------------------------------------

	std::vector<ShaderAttributeInfo> GetAttributesInfo(ShaderProgramRef resource) const;
	Uniform GetUniform(ShaderProgramRef program, const char* uniformName) const;

	void SetUniform(const Uniform& uniform, int value);
	void SetUniform(const Uniform& uniform, float value);
	void SetUniform(const Uniform& uniform, const glm::vec2& value);
	void SetUniform(const Uniform& uniform, const glm::vec3& value);
	void SetUniform(const Uniform& uniform, const glm::vec4& value);
	void SetUniform(const Uniform& uniform, const glm::mat3& value);
	void SetUniform(const Uniform& uniform, const glm::mat4& value);

	// не рекомендуется - только для быстрого теста
	void SetUniform(const std::string& uniformName, int value);
	void SetUniform(const std::string& uniformName, float value);
	void SetUniform(const std::string& uniformName, const glm::vec2& value);
	void SetUniform(const std::string& uniformName, const glm::vec3& value);
	void SetUniform(const std::string& uniformName, const glm::vec4& value);
	void SetUniform(const std::string& uniformName, const glm::mat3& value);
	void SetUniform(const std::string& uniformName, const glm::mat4& value);

	//-------------------------------------------------------------------------
	// Buffer Op
	//-------------------------------------------------------------------------
	void UpdateBuffer(VertexBufferRef vbo, unsigned offset, unsigned vertexCount, unsigned vertexSize, const void* data);
	void UpdateBuffer(IndexBufferRef ibo, unsigned offset, unsigned indexCount, unsigned indexSize, const void* data);

	//-------------------------------------------------------------------------
	// Current State Set
	//-------------------------------------------------------------------------
	void ResetState(ResourceType type);
	void Bind(ShaderProgramRef resource);
	void Bind(VertexBufferRef resource);
	void Bind(IndexBufferRef resource);
	void Bind(const VertexAttribute& Attribute);
	void Bind(Texture2DRef resource, unsigned slot = 0);
	void Bind(FramebufferRef resource);

	//-------------------------------------------------------------------------
	// Draw
	//-------------------------------------------------------------------------
	void Draw(VertexArrayRef vao, PrimitiveTopology primitive = PrimitiveTopology::Triangles);

private:
	RenderSystem() = default;
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;

	void initializeCapabilities(bool print);

	ShaderRef compileShader(ShaderType type, const std::string& source);

	int m_mainFramebufferWidth = 0;
	int m_mainFramebufferHeight = 0;

	Capabilities m_capabilities;

	struct
	{
		unsigned CurrentShaderProgram = 0;
		unsigned CurrentVBO = 0;
		unsigned CurrentIBO = 0;
		unsigned CurrentVAO = 0;
		unsigned CurrentTexture2D[MaxBindingTextures] = { 0 };
		unsigned CurrentFramebuffer = 0;
	} m_cache;
	std::unordered_map<std::string, Texture2DRef> m_cacheFileTextures2D;
};