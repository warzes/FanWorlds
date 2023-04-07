#include "stdafx.h"
#include "002_Quad.h"
//-----------------------------------------------------------------------------
bool _002Quad::Create()
{
	Print("002_Quad Create");

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
	}
	vert[] =
	{
		{{  0.5f,  0.5f, 4.0f}, {1.0f, 0.0f, 0.0f}}, // top right
		{{  0.5f, -0.5f, 4.0f}, {0.0f, 1.0f, 0.0f}}, // bottom right
		{{ -0.5f, -0.5f, 4.0f}, {0.0f, 0.0f, 1.0f}}, // bottom left
		{{ -0.5f,  0.5f, 4.0f}, {0.0f, 1.0f, 1.0f}}, // top left
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	//glEnable(GL_CULL_FACE); // ��� ����� - ���� ���� ������ ����� �������

	auto& renderSystem = GetRenderSystem();

	m_shader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "projectionMatrix");

	m_geom = renderSystem.CreateGeometryBuffer(ResourceUsage::Static, Countof(vert), sizeof(testVertex), vert, Countof(indices), IndexBufferFormat::Uint32, indices, m_shader);

	return true;
}
//-----------------------------------------------------------------------------
void _002Quad::Destroy()
{
	m_shader.reset();
	m_geom.reset();

	Print("002_Quad Destroy");
}
//-----------------------------------------------------------------------------
void _002Quad::Render()
{
	auto& renderSystem = GetRenderSystem();

	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	renderSystem.Clear();
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
	renderSystem.Draw(m_geom->vao);
}
//-----------------------------------------------------------------------------
void _002Quad::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}
}
//-----------------------------------------------------------------------------