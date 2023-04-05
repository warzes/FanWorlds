#pragma once

#include "MoveOnly.h"

class RenderSystem;

enum class FramebufferDepthOption 
{
	NoDepth,
	DepthIsRenderBuffer,
	DepthIsTexture
};

class Framebuffer2
{
public:
	MOVE_ONLY(Framebuffer2);

	Framebuffer2() = delete;
	Framebuffer2(RenderSystem& renderSystem, const glm::ivec2& size, const std::initializer_list<GLenum>& formats, FramebufferDepthOption depthOption);
	Framebuffer2(RenderSystem& renderSystem, const glm::ivec2& size, GLenum format, FramebufferDepthOption depthOption)
		: Framebuffer2(renderSystem, size, { format }, depthOption) {}
	~Framebuffer2();

	[[nodiscard]] const glm::ivec2& Size() const { return m_size; }

	void Bind();

	static void BindDefault();

	void Clear();

	void BindColorTexture(GLuint unit, GLuint attachment);

	void BindDepthStencilTexture(GLuint unit);

	void BindAllTextures();

	void UnbindAllTextures();

	[[nodiscard]] int GetTextureCount() const 
	{
		return m_depthOption == FramebufferDepthOption::DepthIsTexture ? m_numColorAttachments + 1 : m_numColorAttachments;
	}

	void BlitDepthStencilToScreen(const glm::ivec2& screenSize);

private:
	void createColorAttachment(GLenum format);
	void createColorAttachments(const std::initializer_list<GLenum>& formats);
	void createDepthStencilAttachment();

	RenderSystem& m_renderSystem;

	MoveOnly<glm::ivec2> m_size;
	MoveOnly<GLuint> m_fbo;

	static constexpr int MAX_NUM_COLOR_ATTACHMENTS = 32;
	using ColorAttachmentsArray = std::array<MoveOnly<GLuint>, MAX_NUM_COLOR_ATTACHMENTS>;

	MoveOnly<GLsizei> m_numColorAttachments;
	ColorAttachmentsArray m_colorAttachments;

	MoveOnly<FramebufferDepthOption> m_depthOption;
	MoveOnly<GLuint> m_depthStencilAttachment;
};