#include "stdafx.h"
#include "EditorCursor.h"
#include "Ray.h"
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
void EditorCursor::Draw(RenderSystem& renderSystem, Input& input, const glm::mat4& proj, const Camera& camera, float currentGridHeight)
{
	renderSystem.Bind(m_shaderLine);
	renderSystem.SetUniform("uWVP", proj * camera.GetViewMatrix());

	const glm::vec2 mousePos = input.GetMousePosition();

	const Ray ray = GetMouseRay(mousePos, input.GetWindowWidth(), input.GetWindowHeight(), proj, camera);
	const RayCollision rayCol = GetRayCollisionQuad(ray,
		glm::vec3(0.0f,    currentGridHeight, 0.0f   ),
		glm::vec3(0.0f,    currentGridHeight, 1000.0f),
		glm::vec3(1000.0f, currentGridHeight, 1000.0f),
		glm::vec3(1000.0f, currentGridHeight, 0.0f   ));

	const glm::vec3 lineData[] =
	{
		{camera.position.x, currentGridHeight + 0.1f, camera.position.z},
		rayCol.point
	};
	m_pos = rayCol.point;

	renderSystem.UpdateBuffer(m_vboLine, 0, 2, sizeof(lineData), lineData);
	renderSystem.Draw(m_vaoLine, PrimitiveTopology::Lines);
}
//-----------------------------------------------------------------------------