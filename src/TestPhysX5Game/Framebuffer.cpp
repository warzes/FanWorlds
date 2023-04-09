#include "stdafx.h"
#include "Framebuffer.h"
//-----------------------------------------------------------------------------
Framebuffer2::Framebuffer2(
	RenderSystem& renderSystem,
	const glm::ivec2& size,
	const std::initializer_list<GLenum>& formats,
	const FramebufferDepthOption depthOption
) 
	: m_renderSystem(renderSystem)
	, m_size(size)
	, m_depthOption(depthOption)
{
	glCreateFramebuffers(1, &m_fbo);
	createColorAttachments(formats);
	createDepthStencilAttachment();

	if (glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) 
	{
		m_renderSystem.Error("Framebuffer incomplete");
	}
}
//-----------------------------------------------------------------------------
void Framebuffer2::createColorAttachment(const GLenum format) 
{
	if (m_numColorAttachments >= MAX_NUM_COLOR_ATTACHMENTS) 
	{
		m_renderSystem.Error("Cannot use more than " + std::to_string(MAX_NUM_COLOR_ATTACHMENTS) + " color attachments in a framebuffer");
		return;
	}

	auto &texture = m_colorAttachments[m_numColorAttachments];
	m_numColorAttachments = m_numColorAttachments + 1;

	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, format, m_size->x, m_size->y);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
//-----------------------------------------------------------------------------
void Framebuffer2::createColorAttachments(const std::initializer_list<GLenum>& formats) 
{
	for( const GLenum format : formats )
	{
		createColorAttachment(format);
	}

	std::array<GLenum, MAX_NUM_COLOR_ATTACHMENTS> attachments{};
	for( int i = 0; i < m_numColorAttachments; i++ )
	{
		glNamedFramebufferTexture(m_fbo, GL_COLOR_ATTACHMENT0 + i, m_colorAttachments[i], 0);
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glNamedFramebufferDrawBuffers(m_fbo, m_numColorAttachments, attachments.data());
}
//-----------------------------------------------------------------------------
void Framebuffer2::createDepthStencilAttachment() 
{
	switch( m_depthOption )
	{
	case FramebufferDepthOption::NoDepth:
		break;
	case FramebufferDepthOption::DepthIsRenderBuffer:
		glCreateRenderbuffers(1, &m_depthStencilAttachment);
		glNamedRenderbufferStorage(m_depthStencilAttachment, GL_DEPTH24_STENCIL8, m_size->x, m_size->y);
		glNamedFramebufferRenderbuffer(m_fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilAttachment);
		break;
	case FramebufferDepthOption::DepthIsTexture:
		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthStencilAttachment);
		glTextureStorage2D(m_depthStencilAttachment, 1, GL_DEPTH24_STENCIL8, m_size->x, m_size->y);
		glTextureParameteri(m_depthStencilAttachment, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_depthStencilAttachment, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_depthStencilAttachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_depthStencilAttachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(m_fbo, GL_DEPTH_STENCIL_ATTACHMENT, m_depthStencilAttachment, 0);
		break;
	}
}
//-----------------------------------------------------------------------------
Framebuffer2::~Framebuffer2() 
{
	if( m_fbo )
	{
		glDeleteFramebuffers(1, &m_fbo);
	}
	for( auto &texture : m_colorAttachments )
	{
		if( texture )
		{
			glDeleteTextures(1, &texture);
		}
	}
	switch( m_depthOption )
	{
	case FramebufferDepthOption::NoDepth:
		break;
	case FramebufferDepthOption::DepthIsRenderBuffer:
		glDeleteRenderbuffers(1, &m_depthStencilAttachment);
		break;
	case FramebufferDepthOption::DepthIsTexture:
		glDeleteTextures(1, &m_depthStencilAttachment);
		break;
	}
}
//-----------------------------------------------------------------------------
void Framebuffer2::Bind() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}
//-----------------------------------------------------------------------------
void Framebuffer2::BindDefault() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//-----------------------------------------------------------------------------
void Framebuffer2::ClearFrame() 
{
	static constexpr GLfloat CLEAR_COLOR[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < m_numColorAttachments; i++) 
	{
		glClearNamedFramebufferfv(m_fbo, GL_COLOR, i, CLEAR_COLOR);
	}
	glClearNamedFramebufferfi(m_fbo, GL_DEPTH_STENCIL, 0, 1.0f, 0);
}
//-----------------------------------------------------------------------------
void Framebuffer2::BindColorTexture(const GLuint unit, const GLuint attachment)
{
	glBindTextureUnit(unit, m_colorAttachments[attachment]);
}
//-----------------------------------------------------------------------------
void Framebuffer2::BindDepthStencilTexture(const GLuint unit)
{
	if (m_depthOption != FramebufferDepthOption::DepthIsTexture) return;
	glBindTextureUnit(unit, m_depthStencilAttachment);
}
//-----------------------------------------------------------------------------
void Framebuffer2::BindAllTextures()
{
	for (int i = 0; i < m_numColorAttachments; i++)
	{
		BindColorTexture(i, i);
	}
	BindDepthStencilTexture(m_numColorAttachments);
}
//-----------------------------------------------------------------------------
void Framebuffer2::UnbindAllTextures() 
{
	for (int i = 0; i < m_numColorAttachments; i++) 
	{
		glBindTextureUnit(i, 0);
	}
	if (m_depthOption != FramebufferDepthOption::DepthIsTexture) return;
	glBindTextureUnit(m_numColorAttachments, 0);
}
//-----------------------------------------------------------------------------
void Framebuffer2::BlitDepthStencilToScreen(const glm::ivec2& screenSize) 
{
	glBlitNamedFramebuffer(
		m_fbo,
		0,
		0, 0, m_size->x, m_size->y,
		0, 0, screenSize.x, screenSize.y,
		GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
		GL_NEAREST
	);
}
//-----------------------------------------------------------------------------