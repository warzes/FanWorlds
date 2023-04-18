#include "stdafx.h"
#include "EditorCursor.h"
//-----------------------------------------------------------------------------
bool EditorCursor::Create(RenderSystem& renderSystem)
{
	constexpr const char* vertexShaderLineText = R"(
#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uWVP;

void main()
{
	gl_Position = uWVP * vec4(aPos, 1.0);
}
)";

	constexpr const char* fragmentShaderLineText = R"(
#version 330 core

out vec4 fragColor;

void main()
{
	fragColor = vec4(1.0, 0.8, 0.6, 1.0);
}
)";

	m_shaderLine = renderSystem.CreateShaderProgram({ vertexShaderLineText }, { fragmentShaderLineText });
	m_vboLine = renderSystem.CreateVertexBuffer(BufferUsage::DynamicDraw, 1, sizeof(float) * 3, nullptr);
	m_vaoLine = renderSystem.CreateVertexArray(m_vboLine, nullptr, m_shaderLine);

	return true;
}
//-----------------------------------------------------------------------------
void EditorCursor::Destroy()
{
	m_shaderLine.reset();
	m_vboLine.reset();
	m_vaoLine.reset();
}
//-----------------------------------------------------------------------------
void EditorCursor::Update(Input& input, const glm::mat4& proj, const Camera& camera, float gridHeight, float sizeMap, float sizeCell)
{
	const glm::vec2 mousePos = input.GetMousePosition();
	const Ray ray = GetMouseRay(mousePos, input.GetWindowWidth(), input.GetWindowHeight(), proj, camera);
	m_rayColToGrid = GetRayCollisionQuad(ray,
		glm::vec3(0.0f,    gridHeight, 0.0f),
		glm::vec3(0.0f,    gridHeight, sizeMap),
		glm::vec3(sizeMap, gridHeight, sizeMap),
		glm::vec3(sizeMap, gridHeight, 0.0f));

	// позиция курсора в 3д мире с учетом сетки
	if( input.IsKeyDown(Input::KEY_LEFT_SHIFT) )
		m_objPosition = { GetPosX(), gridHeight, GetPosZ() };
	else
		m_objPosition = 
		{
			floor((GetPosX() + sizeCell * 0.5f) / sizeCell) * sizeCell,
			gridHeight,
			floor((GetPosZ() + sizeCell * 0.5f) / sizeCell) * sizeCell
		};
}
//-----------------------------------------------------------------------------
void EditorCursor::Draw(RenderSystem& renderSystem, const glm::mat4& projView, const glm::vec3& startPos, float currentGridHeight)
{
	const glm::vec3 lineData[] =
	{
		{startPos.x, currentGridHeight + 0.1f, startPos.z},
		m_rayColToGrid.point
	};

	renderSystem.Bind(m_shaderLine);
	renderSystem.SetUniform("uWVP", projView);
	renderSystem.UpdateBuffer(m_vboLine, 0, Countof(lineData), sizeof(glm::vec3), lineData);
	renderSystem.Draw(m_vaoLine, PrimitiveTopology::Lines);
}
//-----------------------------------------------------------------------------