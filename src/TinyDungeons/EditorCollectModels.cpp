#include "stdafx.h"
#include "EditorCollectModels.h"
#include "EditorWorldObject.h"
//-----------------------------------------------------------------------------
bool EditorCollectModels::Create(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem)
{
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

out vec3 fNormal;
out vec3 fColor;
out vec2 fTexCoord;
out float visibility;

const float density = 0.03;
const float gradient = 1.5;

void main()
{
	vec4 vertPosRelativeToCamera = uView * uWorld * vec4(aPos, 1.0);
	float dist = length(vertPosRelativeToCamera.xyz);
	visibility = exp(-pow((dist * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	gl_Position = uProjection * uView * uWorld * vec4(aPos, 1.0);
	fNormal = mat3(transpose(inverse(uWorld))) * aNormal;
	fColor = aColor;
	fTexCoord = aTexCoord;
}
)";

		constexpr const char* fragmentShaderText = R"(
#version 330 core

in vec3 fNormal;
in vec3 fColor;
in vec2 fTexCoord;
in float visibility;

uniform sampler2D DiffuseTexture;

out vec4 fragColor;

void main()
{
	vec4 texture =  texture(DiffuseTexture, fTexCoord);
	//if (texture.a < 0.02) discard;


	vec3 fogColor = vec3(0.4, 0.5, 0.4);
	vec3 lightDirection = vec3(0.0, 0.8, -0.2);
	vec3 norm = normalize(fNormal);
	float shadow = dot(norm, lightDirection);
	if(shadow <= 0.0)
	{
		shadow = 0.0;
	}

	vec3 objectColor = texture.rgb * fColor;
	//objectColor = fColor;

	fragColor = (vec4(objectColor, 1.0) * (shadow + 0.3)) * 0.7;
	fragColor = mix(vec4(fogColor, 1.0), fragColor, visibility);
	fragColor.a = texture.a;

	// HDR tonemapping
	//fragColor.rgb = fragColor.rgb / (fragColor.rgb + vec3(1.0));
	// gamma correct
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2));
}
)";

		m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
		m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "uProjection");
		m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "uView");
		m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "uWorld");
	}
	
	{
		constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uWVP;

void main()
{
	gl_Position = uWVP * vec4(aPos, 1.0);
}
)";

		constexpr const char* fragmentShaderText = R"(
#version 330 core

out vec4 fragColor;

void main()
{
	fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
)";

		m_shaderSelBox = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
		m_uniformSelBoxWVPMatrix = renderSystem.GetUniform(m_shaderSelBox, "uWVP");
	}

	m_modelFileName = "../TinyDungeonsData/models/model.obj";
	 ModelRef model = graphicsSystem.CreateModel("../TinyDungeonsData/models/myModel.obj", "../TinyDungeonsData/models/");
	m_meshLib.Create(renderSystem, model);
	
	m_modelSelBox = graphicsSystem.CreateModel("../TinyDungeonsData/models/selBox.obj", "../TinyDungeonsData/models/");

	m_tempTexture = renderSystem.CreateTexture2D("../\TinyDungeonsData/textures/temp.png");

	for (size_t i = 0; i < m_meshLib.meshes.size(); i++)
	{
		Mesh& mesh = m_meshLib.meshes[i];
		mesh.material.diffuseTexture = m_tempTexture;
	}


	return true;
}
//-----------------------------------------------------------------------------
void EditorCollectModels::Destroy()
{
	m_shader.reset();
	m_meshLib.Destroy();
	m_modelSelBox.reset();
	m_shaderSelBox.reset();
}
//-----------------------------------------------------------------------------
void EditorCollectModels::DrawPreview(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& centerPos)
{
	Mesh& mesh = m_meshLib.meshes[m_currentMesh];
	
	const float extent = mesh.globalAABB.GetExtents().y; // ����������� ������ �� ����� �� �������� ������ ����
	m_currentWorldPos = centerPos;
	m_currentWorldPos.y += extent;

	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, proj);
	renderSystem.SetUniform(m_uniformViewMatrix, view);
	renderSystem.SetUniform(m_uniformWorldMatrix, computeCurrentTransform());
	renderSystem.SetUniform("DiffuseTexture", 0);
	graphicsSystem.Draw(mesh);
}
//-----------------------------------------------------------------------------
void EditorCollectModels::DrawSelBox(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& pos, const glm::vec3& eulerRot, const glm::vec3& scales)
{
	const glm::mat4 one = glm::mat4(1.0f);
	const glm::mat4 translate = glm::translate(one, pos);
	const glm::mat4 scale = glm::scale(one, scales);
	const glm::mat4 transformX = glm::rotate(one, glm::radians(eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(one, glm::radians(eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(one, glm::radians(eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	const glm::mat4 rotaionMatrix = transformY * transformX * transformZ;
	const glm::mat4 world = translate * rotaionMatrix * scale;

	renderSystem.Bind(m_shaderSelBox);
	renderSystem.SetUniform(m_uniformSelBoxWVPMatrix, proj * view * world);
	graphicsSystem.Draw(m_modelSelBox);
}
//-----------------------------------------------------------------------------
void EditorCollectModels::DrawMap(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const EditorMap& map)
{
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, proj);
	renderSystem.SetUniform(m_uniformViewMatrix, view);
	renderSystem.SetUniform("DiffuseTexture", 0);

	constexpr glm::mat4 init = glm::mat4(1.0f);
	glm::mat4 world;
	for( size_t i = 0; i < map.object.size(); i++ )
	{
		const EditorMapObject& object = map.object[i].object;
		Mesh& mesh = m_meshLib.meshes[object.meshIndex];
		computeTransform(object, world);
		renderSystem.SetUniform(m_uniformWorldMatrix, world);
		graphicsSystem.Draw(mesh);
	}
}
//-----------------------------------------------------------------------------
void EditorCollectModels::NextMesh()
{
	resetTransform();
	if( m_currentMesh < m_meshLib.meshes.size() - 1 )
		m_currentMesh++;
}
//-----------------------------------------------------------------------------
void EditorCollectModels::PrevMesh()
{
	resetTransform();
	if( m_currentMesh > 0 ) m_currentMesh--;
}
//-----------------------------------------------------------------------------
void EditorCollectModels::resetTransform()
{
	m_currentScale = glm::vec3(1.0f);
	m_currentEulerRot = { 0.0f, 0.0f, 0.0f };
}
//-----------------------------------------------------------------------------
glm::mat4 EditorCollectModels::computeCurrentTransform()
{
	const glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_currentWorldPos);
	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_currentEulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_currentEulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_currentEulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	const glm::mat4 rotaionMatrix = transformY * transformX * transformZ;
	return translate * rotaionMatrix /** scale*/;
}
//-----------------------------------------------------------------------------
void EditorCollectModels::computeTransform(const EditorMapObject& object, glm::mat4& world)
{
	const glm::mat4 translate = glm::translate(glm::mat4(1.0f), object.worldPosition);
	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(object.eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(object.eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(object.eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));
	const glm::mat4 rotaionMatrix = transformY * transformX * transformZ;
	world = translate * rotaionMatrix /** scale*/;
}
//-----------------------------------------------------------------------------