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

// TODO: ������� �������� �� ����

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
	// Config Current Frame Set
	//-------------------------------------------------------------------------

	void SetClearColor(const glm::vec3& color);
	void ClearFrame();
	void ClearFrame(const glm::vec3& color);
	void SetViewport(int width, int height);
	void SetViewport(int x, int y, int width, int height);
	void MainScreen();

	//-------------------------------------------------------------------------
	// Create Render Resource
	//-------------------------------------------------------------------------
	ShaderProgramRef CreateShaderProgram(const ShaderSource& vertexShaderSource, const ShaderSource& fragmentShaderSource);
	GPUBufferRef CreateVertexBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data);
	GPUBufferRef CreateIndexBuffer(BufferUsage usage, unsigned indexCount, IndexType indexFormat, const void* data);
	VertexArrayRef CreateVertexArray(GPUBufferRef vbo, GPUBufferRef ibo, const std::vector<VertexAttribute>& attribs);
	VertexArrayRef CreateVertexArray(GPUBufferRef vbo, GPUBufferRef ibo, ShaderProgramRef shaders);

	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexType indexFormat, const void* indexData,
		ShaderProgramRef shaders);
	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexType indexFormat, const void* indexData,
		const std::vector<VertexAttribute>& attribs);
	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, ShaderProgramRef shaders);
	GeometryBufferRef CreateGeometryBuffer(BufferUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const std::vector<VertexAttribute>& attribs);

	Texture2DRef CreateTexture2D(const char* fileName, bool useCache = true, const Texture2DInfo& textureInfo = {});
	Texture2DRef CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo = {});

	RenderbufferRef CreateRenderbuffer(const glm::uvec2& size, ImageFormat format, int multisample = 1);

	// ������� ����� ��������� � ������������� � ����������� - �������� ����� ����� �������

	FramebufferRef CreateFramebuffer(RenderbufferRef colorBuffer);
	FramebufferRef CreateFramebuffer(RenderbufferRef colorBuffer, RenderbufferRef depthStencilBuffer);
	FramebufferRef CreateFramebuffer(Texture2DRef colorTexture);
	FramebufferRef CreateFramebuffer(Texture2DRef colorTexture, RenderbufferRef depthStencilBuffer);
	FramebufferRef CreateFramebuffer(Texture2DRef colorTexture, Texture2DRef depthStencilTexture);

	FramebufferRef CreateFramebuffer(FramebufferAttachment attachment, Texture2DRef texture); // TODO: delete

	//-------------------------------------------------------------------------
	// Validation
	//-------------------------------------------------------------------------
	inline bool IsValid(ShaderRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(ShaderProgramRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(const Uniform& uniform) const { return uniform.location >= 0; }
	inline bool IsValid(GPUBufferRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(VertexArrayRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(Texture2DRef resource) const { return resource && resource->IsValid(); }
	inline bool IsValid(GeometryBufferRef resource) const { return IsValid(resource->vao); }
	inline bool IsValid(FramebufferRef resource) const { return resource && resource->IsValid(); }
	bool IsReadyUniform(const Uniform& uniform) const;

	//-------------------------------------------------------------------------
	// Shader Op
	//-------------------------------------------------------------------------

	std::vector<ShaderAttributeInfo> GetAttributesInfo(ShaderProgramRef program) const;
	Uniform GetUniform(ShaderProgramRef program, const char* uniformName) const;

	void SetUniform(const Uniform& uniform, int value);
	void SetUniform(const Uniform& uniform, float value);
	void SetUniform(const Uniform& uniform, const glm::vec2& value);
	void SetUniform(const Uniform& uniform, const glm::vec3& value);
	void SetUniform(const Uniform& uniform, const glm::vec4& value);
	void SetUniform(const Uniform& uniform, const glm::mat3& value);
	void SetUniform(const Uniform& uniform, const glm::mat4& value);
	void SetUniform3(const Uniform& uniform, unsigned number, float* value);
	void SetUniform4(const Uniform& uniform, unsigned number, float* value);

	// �� ������������� - ������ ��� �������� �����
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
	void UpdateBuffer(GPUBufferRef buffer, unsigned offset, unsigned count, unsigned size, const void* data);

	void* MapBuffer(GPUBufferRef buffer);
	void* MapBuffer(GPUBufferRef buffer, unsigned offset, unsigned size);
	bool UnmapBuffer(GPUBufferRef buffer);

	//-------------------------------------------------------------------------
	// Current State Set
	//-------------------------------------------------------------------------
	void ResetState(ResourceType type);
	void Bind(DepthState state);
	void Bind(StencilState state);
	void Bind(ShaderProgramRef resource);
	void Bind(GPUBufferRef buffer);
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
	void attachmentFrameBufferColor(FramebufferRef fbo, RenderbufferRef colorBuffer);
	void attachmentFrameBufferColor(FramebufferRef fbo, Texture2DRef colorTexture);
	void attachmentFrameBufferDepthStencil(FramebufferRef fbo, RenderbufferRef depthStencilBuffer);
	void attachmentFrameBufferDepthStencil(FramebufferRef fbo, Texture2DRef depthStencilTexture);
	bool checkCurrentFrameBuffer() const;

	void setClearMask(bool color, bool depth, bool stensil);

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

		DepthState CurrentDepthState{};
		StencilState CurrentStencilState{};

		GLbitfield CurrentClearMask = 0;
	} m_cache;

	unsigned& getCurrentCacheBufferFromType(BufferType type)
	{
		if (type == BufferType::ArrayBuffer) return m_cache.CurrentVBO;
		else if(type == BufferType::ElementArrayBuffer) return m_cache.CurrentIBO;
		else
		{
			assert(1 && "not impl!");
			return m_cache.CurrentVBO;
		}
	}

	std::unordered_map<std::string, Texture2DRef> m_cacheFileTextures2D;
};