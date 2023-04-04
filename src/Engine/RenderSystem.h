#pragma once

#include "BaseClass.h"
#include "RenderResource.h"

constexpr int MaxBindingTextures = 16;

struct RenderCreateInfo
{

};

// TODO: очистка ресурсов из кеша

class RenderSystem : public BaseClass
{
	friend class EngineDevice;
public:
	void Init(const RenderCreateInfo& createInfo);
	void Close();

	void BeginFrame();
	void EndFrame();

	std::shared_ptr<ShaderProgram> CreateShaderProgram(const std::string& vertexShaderMemory, const std::string& fragmentShaderMemory);
	std::shared_ptr<VertexBuffer> CreateVertexBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* data);
	std::shared_ptr<IndexBuffer> CreateIndexBuffer(ResourceUsage usage, unsigned indexCount, IndexBufferFormat indexFormat, const void* data);
	std::shared_ptr<VertexArray> CreateVertexArray(std::shared_ptr<VertexBuffer> vbo, std::shared_ptr<IndexBuffer> ibo, const std::vector<VertexAttribute>& attribs);
	std::shared_ptr<VertexArray> CreateVertexArray(std::shared_ptr<VertexBuffer> vbo, std::shared_ptr<IndexBuffer> ibo, std::shared_ptr<ShaderProgram> shaders);

	std::shared_ptr<GeometryBuffer> CreateGeometryBuffer(ResourceUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexBufferFormat indexFormat, const void* indexData,
		std::shared_ptr<ShaderProgram> shaders);
	std::shared_ptr<GeometryBuffer> CreateGeometryBuffer(ResourceUsage usage,
		/*vertex*/unsigned vertexCount, unsigned vertexSize, const void* vertexData,
		/*index*/unsigned indexCount, IndexBufferFormat indexFormat, const void* indexData,
		const std::vector<VertexAttribute>& attribs);
	std::shared_ptr<GeometryBuffer> CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, std::shared_ptr<ShaderProgram> shaders);
	std::shared_ptr<GeometryBuffer> CreateGeometryBuffer(ResourceUsage usage, unsigned vertexCount, unsigned vertexSize, const void* vertexData, const std::vector<VertexAttribute>& attribs);

	std::shared_ptr<Texture2D> CreateTexture2D(const char* fileName, bool useCache = true, const Texture2DInfo& textureInfo = {});
	std::shared_ptr<Texture2D> CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo = {});

	inline bool IsValid(std::shared_ptr<ShaderProgram> resource) const { return resource && resource->id > 0; }
	inline bool IsValid(const Uniform& uniform) const { return uniform.location >= 0; }
	inline bool IsValid(std::shared_ptr<VertexBuffer> resource) const { return resource && resource->id > 0; }
	inline bool IsValid(std::shared_ptr<IndexBuffer> resource) const { return resource && resource->id > 0; }
	inline bool IsValid(std::shared_ptr<VertexArray> resource) const { return resource && resource->id > 0; }
	inline bool IsValid(std::shared_ptr<Texture2D> resource) const { return resource && resource->id > 0; }
	inline bool IsValid(std::shared_ptr<GeometryBuffer> resource) const { return IsValid(resource->vao); }

	bool IsReadyUniform(const Uniform& uniform);

	std::vector<ShaderAttribInfo> GetAttribInfo(std::shared_ptr<ShaderProgram> resource);
	Uniform GetUniform(std::shared_ptr<ShaderProgram> program, const char* uniformName);

	void SetUniform(const Uniform& uniform, int value);
	void SetUniform(const Uniform& uniform, float value);
	void SetUniform(const Uniform& uniform, const glm::vec2& value);
	void SetUniform(const Uniform& uniform, const glm::vec3& value);
	void SetUniform(const Uniform& uniform, const glm::vec4& value);
	void SetUniform(const Uniform& uniform, const glm::mat3& value);
	void SetUniform(const Uniform& uniform, const glm::mat4& value);

	void UpdateVertexBuffer(std::shared_ptr<VertexBuffer> vbo, unsigned offset, unsigned vertexCount, unsigned vertexSize, const void* data);
	void UpdateIndexBuffer(std::shared_ptr<IndexBuffer> ibo, unsigned offset, unsigned indexCount, unsigned indexSize, const void* data);

	void ResetState(ResourceType type);
	void Bind(std::shared_ptr<ShaderProgram> resource);
	void Bind(std::shared_ptr<VertexBuffer> resource);
	void Bind(std::shared_ptr<IndexBuffer> resource);
	void Bind(const VertexAttribute& Attribute);
	void Bind(std::shared_ptr<Texture2D> resource, unsigned slot = 0);

	void Draw(std::shared_ptr<VertexArray> vao, PrimitiveTopology primitive = PrimitiveTopology::Triangles);

private:
	unsigned createShader(GLenum openGLshaderType, const std::string& source);
	int m_mainFramebufferWidth = 0;
	int m_mainFramebufferHeight = 0;

	struct
	{
		unsigned CurrentShaderProgram = 0;
		unsigned CurrentVBO = 0;
		unsigned CurrentIBO = 0;
		unsigned CurrentVAO = 0;
		unsigned CurrentTexture2D[MaxBindingTextures] = { 0 };
	} m_cache;
	std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_cacheFileTextures2D;
};