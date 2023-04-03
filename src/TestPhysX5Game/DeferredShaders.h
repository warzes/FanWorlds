#pragma once

#include "Shader.h"

class DeferredShaderShadowPass : public OldShaderProgram
{
public:
	MOVE_ONLY(DeferredShaderShadowPass);

	DeferredShaderShadowPass(RenderSystem& renderSystem);

	void SetModel(const glm::mat4 &model);

private:
	GLint m_modelLocation = -1;
};

class DeferredShaderBase : public OldShaderProgram
{
public:
	MOVE_ONLY(DeferredShaderBase);

	DeferredShaderBase(RenderSystem& renderSystem);

	void SetModel(const glm::mat4 &model);

private:
	GLint m_modelLocation = -1;
};

class DeferredShaderGPass : public OldShaderProgram
{
public:
	MOVE_ONLY(DeferredShaderGPass);

	DeferredShaderGPass(RenderSystem& renderSystem);
};

class DeferredShaderLines : public OldShaderProgram
{
public:
	MOVE_ONLY(DeferredShaderLines);

	DeferredShaderLines(RenderSystem& renderSystem);
};

class DeferredShaderSkybox : public OldShaderProgram
{
public:
	MOVE_ONLY(DeferredShaderSkybox);

	DeferredShaderSkybox(RenderSystem& renderSystem);
};