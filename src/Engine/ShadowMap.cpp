#include "stdafx.h"
#include "ShadowMap.h"
#include "RenderSystem.h"
//-----------------------------------------------------------------------------
ShadowMap::ShadowMap(RenderSystem& renderSystem, int size)
	: m_renderSystem(renderSystem)
{
	m_size = size;

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_depthTexture);
	glTextureStorage3D(m_depthTexture, 1, GL_DEPTH_COMPONENT32F, size, size, 4);
	glTextureParameteri(m_depthTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_depthTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_depthTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTextureParameteri(m_depthTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	constexpr GLfloat borderColor[4]{ 1.0f, 1.0f, 1.0f, 1.0f };
	glTextureParameterfv(m_depthTexture, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTextureParameteri(m_depthTexture, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTextureParameteri(m_depthTexture, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);

	glCreateFramebuffers(1, &m_fbo);
	glNamedFramebufferDrawBuffer(m_fbo, GL_NONE);
	glNamedFramebufferReadBuffer(m_fbo, GL_NONE);
	glNamedFramebufferTexture(m_fbo, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);

	if( glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
	{
		m_renderSystem.Error("Framebuffer incomplete");
	}
}
//-----------------------------------------------------------------------------
ShadowMap::~ShadowMap()
{
	if( m_fbo )
	{
		glDeleteFramebuffers(1, &m_fbo);
	}

	if( m_depthTexture )
	{
		glDeleteTextures(1, &m_depthTexture);
	}
}
//-----------------------------------------------------------------------------
void ShadowMap::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}
//-----------------------------------------------------------------------------
void ShadowMap::BindDepthTexture(GLuint unit)
{
	glBindTextureUnit(unit, m_depthTexture);
}
//-----------------------------------------------------------------------------