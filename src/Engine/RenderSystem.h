#pragma once

#include "BaseClass.h"
#include "Shader.h"

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


	std::shared_ptr<ShaderProgram> CreateShaderProgram();


private:
	int m_mainFramebufferWidth = 0;
	int m_mainFramebufferHeight = 0;
};