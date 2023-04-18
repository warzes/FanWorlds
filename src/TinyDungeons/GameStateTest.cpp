#include "stdafx.h"
#include "GameStateTest.h"
#include "SpaceMath.h"
#include "Ray.h"

bool GameStateTest::OnCreate()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	if( !m_gridDrawer.Create(renderSystem) )
		return false;

	m_camera.position = glm::vec3(4.0f, 5.0f, -3.0f);
	m_camera.target = glm::vec3(4.0f, 4.0f, -2.0f);

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

	m_textureModel = renderSystem.CreateTexture2D("../ExampleData/textures/container2.png");

	m_model = graphicsSystem.CreateModel("../ExampleData/models/model.obj", "../ExampleData/models/");

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
	}

	return true;
}

void GameStateTest::OnDestroy()
{
	GetInput().SetMouseLock(false);
	m_shader.reset();
	m_model.reset();
	m_gridDrawer.Destroy();
}

void GameStateTest::OnActive()
{
}

unsigned currentMesh = 0;

void GameStateTest::OnUpdate(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

	if( GetInput().GetMouseWheelMove() > 0.0f && currentMesh < m_model->subMeshes.size() - 1 )
		currentMesh++;
	if( GetInput().GetMouseWheelMove() < 0.0f && currentMesh > 0 )
		currentMesh--;

	if( GetInput().IsMouseButtonDown(1) )
	{
		m_freeLook = true;
		GetInput().SetMouseLock(true);
	}
	else
	{
		if( m_freeLook ) GetInput().SetMouseLock(false);
		m_freeLook = false;
	}

	const float mouseSensitivity = 30.0f * deltaTime;
	const float moveSpeed = 16.0f * deltaTime;
	const glm::vec3 oldCameraPos = m_camera.position;

	if( GetInput().IsKeyDown(Input::KEY_W) ) m_camera.MoveBy(moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_S) ) m_camera.MoveBy(-moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_A) ) m_camera.StrafeBy(moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_D) ) m_camera.StrafeBy(-moveSpeed);
	if( GetInput().IsKeyDown(Input::KEY_Q) ) m_camera.RotateLeftRight(-moveSpeed * 12.0f);
	if( GetInput().IsKeyDown(Input::KEY_E) ) m_camera.RotateLeftRight(moveSpeed * 12.0f);

	if( m_freeLook )
	{
		glm::vec2 delta = GetInput().GetMouseDeltaPosition();
		if( delta.x != 0.0f )  m_camera.RotateLeftRight(delta.x * mouseSensitivity);
		if( delta.y != 0.0f )  m_camera.RotateUpDown(-delta.y * mouseSensitivity);
	}
}

void GameStateTest::OnFrame()
{
	auto& renderSystem = GetRenderSystem();
	auto& graphicsSystem = GetGraphicsSystem();

	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
		m_perspective = glm::perspective(glm::radians(45.0f), GetWindowSizeAspect(), 0.01f, 1000.f);
		renderSystem.SetViewport(m_windowWidth, m_windowHeight);
	}

	renderSystem.ClearFrame();

	if( !m_freeLook )
	{
		// draw cursor line
		{
			renderSystem.Bind(m_shaderLine);
			renderSystem.SetUniform("uWVP", m_perspective * m_camera.GetViewMatrix());

			glm::vec2 mousePos = GetInput().GetMousePosition();

			Ray ray = GetMouseRay(mousePos, m_windowWidth, m_windowHeight, m_perspective, m_camera);
			RayCollision rayCol = GetRayCollisionQuad(ray, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3(1000.0f, 0.0f, 1000.0f), glm::vec3(1000.0f, 0.0f, 0.0f));

			glm::vec3 lineData[] =
			{
				{m_camera.position.x, 0.0f, m_camera.position.z},
				rayCol.point
			};
			m_3dCursorPos = rayCol.point;

			renderSystem.UpdateBuffer(m_vboLine, 0, 2, sizeof(lineData), lineData);
			renderSystem.Draw(m_vaoLine, PrimitiveTopology::Lines);
		}

		// draw cube
		{
			glm::vec3 cubePos = glm::floor(m_3dCursorPos + 0.5f);
			cubePos.y = 0;
			glm::mat4 world = glm::translate(glm::mat4(1.0f), cubePos);

			renderSystem.Bind(m_textureModel); // TODO: нужен метод для установки материала в модель
			renderSystem.Bind(m_shader);
			renderSystem.SetUniform(m_uniformProjectionMatrix, m_perspective);
			renderSystem.SetUniform(m_uniformViewMatrix, m_camera.GetViewMatrix());
			renderSystem.SetUniform(m_uniformWorldMatrix, world);
			renderSystem.SetUniform("DiffuseTexture", 0);
			//graphicsSystem.Draw(m_model);

			Mesh& mesh = m_model->subMeshes[currentMesh];
			glm::vec3 wpos = mesh.GetMeshPos();
			glm::vec3 expos = mesh.globalAABB.GetExtents();
			glm::vec3 mpos = -wpos + cubePos;
			mpos.y += expos.y;
			world = glm::translate(glm::mat4(1.0f), mpos);
			renderSystem.SetUniform(m_uniformWorldMatrix, world);
			graphicsSystem.Draw(mesh);

			std::string text = std::to_string(mpos.x) + ":" + std::to_string(mpos.z);
			puts(text.c_str());
		}
	}

	m_gridDrawer.Draw(renderSystem, m_perspective * m_camera.GetViewMatrix(), { 0.0f, 0.0f, 0.0f }, 1.0f, 1000.0f);
}