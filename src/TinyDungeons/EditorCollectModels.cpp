#include "stdafx.h"
#include "EditorCollectModels.h"
#include "EditorWorldObject.h"
//-----------------------------------------------------------------------------
bool EditorCollectModels::Create(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem)
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
	//fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2));
}
)";

	m_shader = renderSystem.CreateShaderProgram({ vertexShaderText }, { fragmentShaderText });
	m_uniformProjectionMatrix = renderSystem.GetUniform(m_shader, "uProjection");
	m_uniformViewMatrix = renderSystem.GetUniform(m_shader, "uView");
	m_uniformWorldMatrix = renderSystem.GetUniform(m_shader, "uWorld");

	m_modelFileName = "../TinyDungeonsData/models/model.obj";
	m_model = graphicsSystem.CreateModel("../TinyDungeonsData/models/model.obj", "../TinyDungeonsData/models/");

	return true;
}
//-----------------------------------------------------------------------------
void EditorCollectModels::Destroy()
{
	m_shader.reset();
	m_model.reset();
}
//-----------------------------------------------------------------------------
void EditorCollectModels::DrawPreview(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const glm::vec3& centerPos)
{
	Mesh& mesh = m_model->subMeshes[m_currentMesh];
	const glm::vec3 wpos = mesh.GetWorldPos();
	const glm::vec3 expos = mesh.globalAABB.GetExtents();

	glm::vec3 mpos = -wpos + centerPos;
	mpos.y += expos.y;
	m_currentPos = mpos;

	const glm::mat4 world = glm::translate(glm::mat4(1.0f), mpos);

	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, proj);
	renderSystem.SetUniform(m_uniformViewMatrix, view);
	renderSystem.SetUniform(m_uniformWorldMatrix, world);
	renderSystem.SetUniform("DiffuseTexture", 0);
	graphicsSystem.Draw(mesh);
}
//-----------------------------------------------------------------------------
void EditorCollectModels::DrawMap(RenderSystem& renderSystem, GraphicsSystem& graphicsSystem, const glm::mat4& proj, const glm::mat4& view, const EditorMap& map)
{
	renderSystem.Bind(m_shader);
	renderSystem.SetUniform(m_uniformProjectionMatrix, proj);
	renderSystem.SetUniform(m_uniformViewMatrix, view);
	renderSystem.SetUniform("DiffuseTexture", 0);

	Mesh* mesh = nullptr;
	glm::mat4 world;
	for( size_t i = 0; i < map.object.size(); i++ )
	{
		const EditorMapObject& object = map.object[i].object;
		mesh = &m_model->subMeshes[object.meshIndex];
		world = glm::translate(glm::mat4(1.0f), object.position);
		renderSystem.SetUniform(m_uniformWorldMatrix, world);
		graphicsSystem.Draw(*mesh);
	}
}
//-----------------------------------------------------------------------------
void EditorCollectModels::NextMesh()
{
	if( m_currentMesh < m_model->subMeshes.size() - 1 )
		m_currentMesh++;
}
//-----------------------------------------------------------------------------
void EditorCollectModels::PrevMesh()
{
	if( m_currentMesh > 0 ) m_currentMesh--;
}
//-----------------------------------------------------------------------------