#pragma once

#include "MoveOnly.h"

class RenderSystem;

class ShadowMap
{
public:
	MOVE_ONLY(ShadowMap);

	ShadowMap() = delete;
	ShadowMap(RenderSystem& renderSystem, int size);
	~ShadowMap();

	[[nodiscard]] int Size() const { return m_size; }

	void Bind();

	void BindDepthTexture(GLuint unit);

private:
	RenderSystem& m_renderSystem;
	MoveOnly<int> m_size;
	MoveOnly<GLuint> m_fbo;
	MoveOnly<GLuint> m_depthTexture;
};