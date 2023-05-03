#include "stdafx.h"
#include "RenderUtils.h"
//-----------------------------------------------------------------------------
bool RenderUtils::Create()
{
	constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uColor;

out vec3 fNormal;
out vec3 fColor;
out vec2 fTexCoord;

void main()
{
	gl_Position = uProjection * uView * uWorld * vec4(aPos, 1.0);
	fNormal = aNormal;
	fColor = aColor*uColor;
	fTexCoord = aTexCoord;
}
)";

	constexpr const char* fragmentShaderText = R"(
#version 330 core

in vec3 fNormal;
in vec3 fColor;
in vec2 fTexCoord;

uniform sampler2D DiffuseTexture;

out vec4 fragColor;

void main()
{
	vec4 textureClr = texture(DiffuseTexture, fTexCoord) * vec4(fColor, 1.0);
	if (textureClr.a < 0.02) discard;
	fragColor = textureClr;
}
)";

	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "uProjection");
	m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "uView");
	m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "uWorld");
	m_uniformColor = renderSystem.GetUniform(m_shader, "uColor");

	m_modelBox = graphicsSystem.CreateModel("../ExampleData/models/crate.obj", "../ExampleData/models/");
	m_textureBox = renderSystem.CreateTexture2D("../ExampleData/textures/container2.png");

	m_modelSphere = graphicsSystem.CreateModel("../ExampleData/models/sphere2.obj", "../ExampleData/models/");
	m_textureSphere = renderSystem.CreateTexture2D("../ExampleData/textures/1mx1m.png");

	m_modelAABB = graphicsSystem.CreateModel("../ExampleData/models/selBox.obj", "../ExampleData/models/");
	m_textureAABB = renderSystem.CreateTexture2D("../ExampleData/textures/gridCell.png");

	m_modelAABB->subMeshes[0].material.diffuseTexture = m_textureAABB;

	return true;
}
//-----------------------------------------------------------------------------
void RenderUtils::Destroy()
{
	m_shader.reset();
	m_modelBox.reset();
	m_modelSphere.reset();
	m_textureBox.reset();
	m_textureSphere.reset();
	m_modelAABB.reset();
	m_textureAABB.reset();
}
//-----------------------------------------------------------------------------
void RenderUtils::BeginDraw(const glm::mat4& proj, const glm::mat4& view)
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	renderSystem.Bind(m_shader);
	renderSystem.SetUniform("DiffuseTexture", 0);
	renderSystem.SetUniform(m_uniformProjectionMatrix, proj);
	renderSystem.SetUniform(m_uniformViewMatrix, view);
}
//-----------------------------------------------------------------------------
void RenderUtils::DrawBox(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color)
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	glm::mat4 mtranslate = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 mscale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 world = mtranslate * mscale;

	renderSystem.Bind(m_textureBox, 0);
	renderSystem.SetUniform(m_uniformWorldMatrix, world);
	renderSystem.SetUniform(m_uniformColor, color);
	graphicsSystem.Draw(m_modelBox);
}
//-----------------------------------------------------------------------------
void RenderUtils::DrawSphere(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color)
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	glm::mat4 mtranslate = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 mscale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 world = mtranslate * mscale;

	renderSystem.Bind(m_textureSphere, 0);
	renderSystem.SetUniform(m_uniformWorldMatrix, world);
	renderSystem.SetUniform(m_uniformColor, color);
	graphicsSystem.Draw(m_modelSphere);
}
//-----------------------------------------------------------------------------
void RenderUtils::DrawAABB(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color)
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	glm::mat4 mtranslate = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 mscale = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 world = mtranslate * mscale;

	renderSystem.Bind(m_textureAABB, 0);
	renderSystem.SetUniform(m_uniformWorldMatrix, world);
	renderSystem.SetUniform(m_uniformColor, color);
	graphicsSystem.Draw(m_modelAABB);
}
//-----------------------------------------------------------------------------