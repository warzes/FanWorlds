#include "stdafx.h"
#include "DeferredShaders.h"
//-----------------------------------------------------------------------------
DeferredShaderShadowPass::DeferredShaderShadowPass(RenderSystem& renderSystem) 
	: OldShaderProgram(OldShaderProgram::FromFile(
		renderSystem,
		"shaders/shared.glsl",
		"shaders/shadow_pass.vert",
		"shaders/shadow_pass.geom",
		"shaders/shadow_pass.frag"
))
{
	m_modelLocation = GetUniformLocation("uModel");
}
//-----------------------------------------------------------------------------
void DeferredShaderShadowPass::SetModel(const glm::mat4 &model)
{
	SetUniform(m_modelLocation, model);
}
//-----------------------------------------------------------------------------
DeferredShaderBase::DeferredShaderBase(RenderSystem& renderSystem) 
	: OldShaderProgram(OldShaderProgram::FromFile(
		renderSystem,
		"shaders/shared.glsl",
		"shaders/base.vert",
		"shaders/base.frag"
))
{
	m_modelLocation = GetUniformLocation("uModel");
}
//-----------------------------------------------------------------------------
void DeferredShaderBase::SetModel(const glm::mat4 &model)
{
	SetUniform(m_modelLocation, model);
}
//-----------------------------------------------------------------------------
DeferredShaderGPass::DeferredShaderGPass(RenderSystem& renderSystem) 
	: OldShaderProgram(OldShaderProgram::FromFile(
		renderSystem,
		"shaders/shared.glsl",
		"shaders/g_pass.vert",
		"shaders/g_pass.frag"
))
{
}
//-----------------------------------------------------------------------------
DeferredShaderLines::DeferredShaderLines(RenderSystem& renderSystem) 
	: OldShaderProgram(OldShaderProgram::FromFile(
		renderSystem,
		"shaders/shared.glsl",
		"shaders/lines.vert",
		"shaders/lines.frag"
))
{
}
//-----------------------------------------------------------------------------
DeferredShaderSkybox::DeferredShaderSkybox(RenderSystem& renderSystem)
	: OldShaderProgram(OldShaderProgram::FromFile(
		renderSystem,
		"shaders/shared.glsl",
		"shaders/skybox.vert",
		"shaders/skybox.frag"
))
{
}
//-----------------------------------------------------------------------------