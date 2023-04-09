#include "stdafx.h"
#include "003_BufferMapping.h"
//-----------------------------------------------------------------------------
struct testVertex
{
	glm::vec3 pos;
	glm::vec3 color;
};
//-----------------------------------------------------------------------------
static float pos = 0.0f;
static bool invert = false;
//-----------------------------------------------------------------------------
bool _003BufferMapping::Create()
{
	Print("003BufferMapping Create");

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

	glEnable(GL_CULL_FACE); // ��� ����� - ����������� ���� ������ ����� �������

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");

	testVertex vert[] =
	{
		{{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}},
		{{ 1.0f, -1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}},
		{{ 0.0f,  1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}}
	};
	m_vb = renderSystem.CreateVertexBuffer(BufferUsage::DynamicDraw, Countof(vert), sizeof(testVertex), vert);
	m_vao = renderSystem.CreateVertexArray(m_vb, nullptr, m_shader);

	return true;
}
//-----------------------------------------------------------------------------
void _003BufferMapping::Destroy()
{
	m_shader.reset();
	m_vb.reset();
	m_vao.reset();

	Print("003BufferMapping Destroy");
}
//-----------------------------------------------------------------------------
void _003BufferMapping::Render()
{
	auto& renderSystem = GetRenderSystem();

	if (m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight())
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	{
		testVertex* data = (testVertex*)renderSystem.MapBuffer(m_vb);

		data[2].pos.x = pos;

		renderSystem.UnmapBuffer(m_vb);
	}

	renderSystem.Clear();
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.Draw(m_vao);
}
//-----------------------------------------------------------------------------
void _003BufferMapping::Update(float deltaTime)
{
	pos += deltaTime * (invert ? -1.0f : 1.0f);

	if (pos < -1.0f || pos > 1.0f)
		invert = !invert;

	if (GetInput().IsKeyDown(Input::KEY_ESCAPE))
	{
		BaseClass::ExitRequest();
		return;
	}
}
//-----------------------------------------------------------------------------