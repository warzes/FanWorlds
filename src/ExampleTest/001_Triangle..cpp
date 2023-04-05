#include "stdafx.h"
#include "001_Triangle.h"
//-----------------------------------------------------------------------------
bool _001Triangle::Create()
{
	Print("001Triangle Create");

	const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 projectionMatrix;

out vec3 fragmentColor;

void main()
{
	gl_Position   = projectionMatrix * vec4(position, 1.0);
	fragmentColor = color;
}
)";

	const char* fragmentShaderText = R"(
#version 330 core

in vec3 fragmentColor;
out vec4 color;

void main()
{
	color = vec4(fragmentColor, 1.0);
}
)";

	struct testVertex
	{
		glm::vec3 pos;
		glm::vec3 color;
	};

	testVertex vert[] =
	{
		{{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.0f,  1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 1.0f, -1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}}
	};

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");
	m_vb = renderSystem.CreateVertexBuffer(ResourceUsage::Static, 3, sizeof(testVertex), vert);
	m_vao = renderSystem.CreateVertexArray(m_vb, nullptr, m_shader);

	return true;
}
//-----------------------------------------------------------------------------
void _001Triangle::Destroy()
{
	m_shader.reset();
	m_vb.reset();
	m_vao.reset();

	Print("001Triangle Destroy");
}
//-----------------------------------------------------------------------------
void _001Triangle::Render()
{
	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
	}

	auto& renderSystem = GetRenderSystem();

	renderSystem.Bind(m_shader);
	glm::mat4 mat = glm::perspective(glm::radians(45.0f), (float)GetWindowWidth() / (float)GetWindowHeight(), 0.01f, 1000.f);
	renderSystem.SetUniform(m_uniformProjectionMatrix, mat);
	renderSystem.Draw(m_vao);
}
//-----------------------------------------------------------------------------
void _001Triangle::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}
}
//-----------------------------------------------------------------------------