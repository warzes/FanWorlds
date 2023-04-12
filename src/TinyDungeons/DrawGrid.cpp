#include "stdafx.h"
#include "DrawGrid.h"
//-----------------------------------------------------------------------------
bool DrawGrid::Create(RenderSystem& renderSystem)
{
	constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uWorld;
uniform mat4 uViewProjection;
uniform float uGridSize;

out vec2 fTexCoord;

void main()
{
	gl_Position = uViewProjection * uWorld * vec4(aPos * uGridSize, 1.0);
	fTexCoord = aTexCoord * uGridSize;
}
)";

	constexpr const char* fragmentShaderText = R"(
#version 330 core

in vec2 fTexCoord;

uniform vec3 uColor;

uniform sampler2D Texture;

out vec4 fragColor;

void main()
{
	vec4 textureClr = texture(Texture, fTexCoord);
	if (textureClr.a < 0.02) discard;
	fragColor = textureClr * vec4(uColor, 1.0);
}
)";

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	if (!m_shader) return false;
	m_uniformViewProjectionMatrix = renderSystem.GetUniform(m_shader, "uViewProjection");
	m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "uWorld");
	m_uniformSizeGrid = renderSystem.GetUniform(m_shader, "uGridSize");
	m_uniformColor = renderSystem.GetUniform(m_shader, "uColor");

	struct testVertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
	}
	vert[] =
	{
		{{ -0.5f, 0.0f,  0.5f}, {0.0f, 0.0f}}, // top left
		{{  0.5f, 0.0f,  0.5f}, {1.0f, 0.0f}}, // top right
		{{  0.5f, 0.0f, -0.5f}, {1.0f, 1.0f}}, // bottom right
		{{ -0.5f, 0.0f, -0.5f}, {0.0f, 1.0f}}, // bottom left
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 3, 2,   // first triangle
		2, 1, 0    // second triangle
	};

	m_geom = renderSystem.CreateGeometryBuffer(BufferUsage::StaticDraw, Countof(vert), sizeof(testVertex), vert, Countof(indices), IndexType::Uint32, indices, m_shader);


	Texture2DInfo texInfo;
	texInfo.mipmap = true;
	m_texture = renderSystem.CreateTexture2D("../TinyDungeonsData/textures/gridCell.png", true, texInfo);

	return true;
}
//-----------------------------------------------------------------------------
void DrawGrid::Destroy()
{
	m_shader.reset();
}
//-----------------------------------------------------------------------------
void DrawGrid::Draw(RenderSystem& renderSystem, const glm::mat4& vpMat, const glm::vec3& pos, float size)
{
	const glm::mat4 world = glm::translate(glm::mat4(1.0f), { pos.x + size / 2, pos.y, pos.z + size / 2 });

	renderSystem.Bind(m_texture);
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformViewProjectionMatrix, vpMat);
	renderSystem.SetUniform(m_uniformWorldMatrix, world);
	renderSystem.SetUniform(m_uniformSizeGrid, size);
	renderSystem.SetUniform(m_uniformColor, glm::vec3(1.0f, 1.0f, 1.0f));
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------