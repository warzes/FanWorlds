#pragma once

enum class FramebufferDepthOption 
{
	NoDepth,
	DepthIsRenderBuffer,
	DepthIsTexture
};

class Framebuffer
{
public:
	Framebuffer() = default;

	Framebuffer(const glm::ivec2& size, const std::initializer_list<GLenum>& formats, FramebufferDepthOption depthOption);

	Framebuffer(const glm::ivec2& size, GLenum format, FramebufferDepthOption depthOption)
		: Framebuffer(size, { format }, depthOption) {}

	~Framebuffer();

	[[nodiscard]] const glm::ivec2& Size() const { return m_size; }

	void Bind();

	static void BindDefault();

	void Clear();

	void BindColorTexture(GLuint unit, GLuint attachment);

	void BindDepthStencilTexture(GLuint unit);

	void BindAllTextures();

	void UnbindAllTextures();

	[[nodiscard]] int GetTextureCount() const {
		return m_depthOption == FramebufferDepthOption::DepthIsTexture ? m_numColorAttachments + 1 : m_numColorAttachments;
	}

	void BlitDepthStencilToScreen(const glm::ivec2& screenSize);

private:
	void CreateColorAttachment(GLenum format);

	void CreateColorAttachments(const std::initializer_list<GLenum>& formats);

	void CreateDepthStencilAttachment();

	glm::ivec2 m_size;
	GLuint m_fbo;

	static constexpr int MAX_NUM_COLOR_ATTACHMENTS = 32;
	using ColorAttachmentsArray = std::array<GLuint, MAX_NUM_COLOR_ATTACHMENTS>;

	GLsizei m_numColorAttachments;
	ColorAttachmentsArray m_colorAttachments;

	FramebufferDepthOption m_depthOption;
	GLuint m_depthStencilAttachment;
};