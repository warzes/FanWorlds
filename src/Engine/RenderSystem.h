#pragma once

#include "BaseClass.h"
#include "RenderResource.h"

struct RenderCreateInfo
{

};

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
	std::shared_ptr<IndexBuffer> CreateIndexBuffer(ResourceUsage usage, unsigned indexCount, unsigned indexSize, const void* data);
	std::shared_ptr<VertexArray> CreateVertexArray(std::shared_ptr<VertexBuffer> vbo, std::shared_ptr<IndexBuffer> ibo, const std::vector<VertexAttribute>& attribs);
	std::shared_ptr<VertexArray> CreateVertexArray(std::shared_ptr<VertexBuffer> vbo, IndexBuffer* ibo, const ShaderProgram& shaders);
	std::shared_ptr<Texture2D> CreateTexture2D(const char* fileName, bool useCache = true, const Texture2DInfo& textureInfo = {});
	std::shared_ptr<Texture2D> CreateTexture2D(const Texture2DCreateInfo& createInfo, const Texture2DInfo& textureInfo = {});

	void DestroyResource(ShaderProgram& resource);
	void DestroyResource(VertexBuffer& resource);
	void DestroyResource(IndexBuffer& resource);
	void DestroyResource(VertexArray& resource);
	void DestroyResource(Texture2D& resource);

	std::vector<ShaderAttribInfo> GetAttribInfo(const ShaderProgram& resource);
	Uniform GetUniform(const ShaderProgram& program, const char* uniformName);

	void SetUniform(const Uniform& uniform, int value);
	void SetUniform(const Uniform& uniform, float value);
	void SetUniform(const Uniform& uniform, const glm::vec2& value);
	void SetUniform(const Uniform& uniform, const glm::vec3& value);
	void SetUniform(const Uniform& uniform, const glm::vec4& value);
	void SetUniform(const Uniform& uniform, const glm::mat3& value);
	void SetUniform(const Uniform& uniform, const glm::mat4& value);

	void UpdateVertexBuffer(VertexBuffer& vbo, unsigned offset, unsigned vertexCount, unsigned vertexSize, const void* data);
	void UpdateIndexBuffer(IndexBuffer& ibo, unsigned offset, unsigned indexCount, unsigned indexSize, const void* data);

	void ResetState(ResourceType type);
	void ResetState(ResourceType type, unsigned slot);
	void Bind(const ShaderProgram& resource);
	void Bind(const VertexBuffer& resource);
	void Bind(const IndexBuffer& resource);
	void Bind(const VertexAttribute& Attribute);
	void Bind(const Texture2D& resource, unsigned slot = 0);

	void Draw(const VertexArray& vao, PrimitiveDraw primitive = PrimitiveDraw::Triangles);

private:
	unsigned createShader(GLenum openGLshaderType, const std::string& source);
	int m_mainFramebufferWidth = 0;
	int m_mainFramebufferHeight = 0;

	struct
	{
		unsigned CurrentVBO = 0;
		unsigned CurrentIBO = 0;
	} m_cache;
};