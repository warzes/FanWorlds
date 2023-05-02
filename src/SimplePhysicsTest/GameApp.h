#pragma once

#include "RenderUtils.h"

class GameApp final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;
		
	Camera m_camera;
	glm::mat4 m_perspective;
	RenderUtils m_renderUtils;
};