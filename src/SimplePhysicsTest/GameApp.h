#pragma once

#include "RenderUtils.h"
#include "CameraUtils.h"

class GameApp final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	RenderUtils m_renderUtils;
	CameraUtils m_cameraUtils;
};