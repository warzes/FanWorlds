#include "stdafx.h"
#include "002_Quad.h"

��� �������� ������ ������� �������


https ://www.materialmaker.org
https://www.dhpoware.com/demos/glslAttenuation.html

��� ������

��� �����
������� ������ - ���������
������� ��������� ��������� - ���� ��� ������ ���� - ����� �� ��� �������
������� �������� - ��� ��� ������� ����� ��� ������� ������ ��������
���� - ������� ����������� ������� ����� �������, ����� ��� �������� �������






�� ����������
����� �� ������ ��� ���� ��������.� �� ������� - ����� ��� ������, ������ ���� - � ������� ������ � ������ �� �����

�����
- ����� ���� ����� - ����� ��������������� � ��� y.������������� �� 0 ����������.� ������� ������ ���� ����� ������� ������ �� y.� ���� ������ ����� � ������ - �� ����� ������ ������ �����.����� �������� ������ � ������ - ��������� �� ���������
���� � ������ ���� - �� � ������� ������ � ���� �������(���� ������ XYZ - �� ������� �� ���� ���)


������ ����� ����� �� rpg in box

��� � �������� ��������� �����
https ://www.youtube.com/watch?v=rIatfICEzDE
(�������� � ����������)


� �������� ����� ����� ��� ������ ��� �������� �����


����
https ://www.youtube.com/watch?v=qgDPSnZPGMA&t=16s




�������� ����� �������� ��� - https ://www.materialmaker.org


	����� ���� ������ �� �������� � ����� ���������� ����� �� ������ ��� ����������� �������
	https ://www.youtube.com/watch?v=2Kzo68liq78

������ ���������� ���������
https ://www.youtube.com/watch?v=yKO1Q5QRZic
https://assetstore.unity.com/packages/3d/environments/dungeons/voxel-dungeons-212790




https://github.com/OpenGL-Graphics/first-person-shooter
RuinIsland_GLSL_Demo


� �������� ������� ��������� ����� ������ ����� ����� ��� ������
�� ���� ��� ����� ��������� ��� �������.��� ���� ����� � ��� ����� �������������� �������(������� �� ����)

������� ��������� ���������

https ://github.com/TheTophatDemon/Total-Editor-3
https://steamcommunity.com/sharedfiles/filedetails/?l=norwegian&id=2075248129
https://github.com/jdah/microcraft - ������ (�� ���� ����� �� � 3�


������ ����� - ������������� ��� ����.����� ������ �� ����� � ������� ������ ����.�� ��� � ������ ���������

��� �������� �� ������.������ ����� 1 �����.
������� ��������� �� x � z ���������� �� 0 � �� �������������.�� y ����� ���� ������������, � ���� - ��� ������� ����(������������ - ��� ����������, ������������ - ����)

������ ���������� � ����.�������� �� 100 �� 100 ������.

�������� �������� � �������
- ����� ���� - ��������� ��������� ����(��� ����� ����� ������ �������� ������� ��� ����� - �� ���� ����������� � �������)
- ����� ������ - ����� ��������� / ������� ������ � ������� ����(���� �������� ������ �� ������� - ������� ����� ����)
�������� ��� - ������, �� ������� ������� ������ - ��������������.���� ��� ��� �������� ������ �� �������� �������������� � ��� �� ����� �������
- ����� � ������ ������ ����� ������ ������ ������ � ���� ������
- ����� ������ - ��������� �������� ���� �� 8 ������ ������ � �������������� �� ������


����� ��������� ������
- ������� ������ ����� ���� ����� 100 � 100 � 100. � ����� ������� ��������.
�� ������ ���� ������� �� �������� ���������� ������.� ������ ������ ������ � ������� ���� �������.
����� ����� �� 100�100�100 ������.� ����� ������.
����� ����������� ����� ������ - �� ����� �������� ������ �� ���������.
����� ����� ������������ - �������� ������ ���������� ������� ����� ���������


https ://docs.gl/gl3/glActiveTexture
������� ������ - ������ �� �������

http ://www.hyzgame.org.cn/OpenGL/man3/bottom.php
https://registry.khronos.org/OpenGL/specs/gl/glspec33.core.pdf
https://github.com/mini3d/mini3d_physics



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