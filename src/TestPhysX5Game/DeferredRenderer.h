#pragma once

#include "DeferredShaders.h"
#include "Framebuffer.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshPositionColor.h"
#include "Renderer.h"
#include "ShadowMap.h"
#include "ShadowMatrixCalculator.h"
#include "Texture.h"
#include "UniformBuffer.h"

class DeferredRenderer final : public Renderer
{
	struct ShaderGlobals
	{
		glm::mat4 View{ 1.0f };
		glm::mat4 Projection{ 1.0f };
		float Time = 0.0f;
	};

	struct PointLightData
	{
		glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
		float Linear = 0.0f;
		glm::vec3 Color{ 0.0f, 0.0f, 0.0f };
		float Quadratic = 0.0f;
	};

	struct LightGlobals
	{
		glm::vec3 DirectionalLight;
		float Padding0;
		glm::vec3 DirectionalColor;
		float Padding1;
		glm::vec3 AmbientColor;
		float Padding2;
		glm::vec3 CascadeShadowMapSplits;
		float Padding3;
		glm::mat4 ShadowMatrices[4];
		PointLightData PointLightData[32];
	};

public:
	NO_MOVE_OR_COPY(DeferredRenderer);
	DeferredRenderer() = delete;

	DeferredRenderer(RenderSystem& renderSystem);

	~DeferredRenderer() override;

	void OnResize(const glm::ivec2 &size) override;

	void Update(float deltaTime) override;

	void SetCameraInfo(const glm::mat4 &view, float fov, float near, float far) override;

	void SetWorldLight(const glm::vec3 &lightDirection, const glm::vec3 &lightColor, const glm::vec3 &ambientColor) override;

	void SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) override;

	void BeginDraw() override;

	void EndDraw() override;

	void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float linear, float quadratic) override;

	void DebugDrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &color) override;

	void DrawMesh(const MeshBase &mesh, const glm::mat4 &model, const Material2 *material) override;

private:
	void UpdateUniformBuffers();

	void DrawToShadowMap();

	void DrawToGBuffers();

	void DrawForwardPass();

	ShadowMap m_shadowMap{ m_renderSystem, 2048 };
	Framebuffer2* m_gBuffers = nullptr;

	UniformBuffer<ShaderGlobals> m_shaderGlobals;
	UniformBuffer<LightGlobals> m_lightGlobals;

	float m_time = 0.0f;
	ShadowMatrixCalculator m_shadowMatrixCalculator;

	DeferredShaderShadowPass* m_shadowShader = nullptr;
	DeferredShaderBase* m_baseShader = nullptr;
	DeferredShaderLines* m_linesShader = nullptr;
	DeferredShaderGPass* m_gPassShader = nullptr;
	DeferredShaderSkybox* m_skyboxShader = nullptr;

	MeshPositionOnly m_fullscreenQuad;
	MeshPositionOnly m_skyboxCube;

	Texture* m_defaultDiffuseTexture = nullptr;
	Texture* m_defaultEmissiveTexture = nullptr;
	Material2 m_defaultMaterial{
		m_defaultDiffuseTexture,
		m_defaultEmissiveTexture
	};

	std::vector<PointLightData> m_pendingPointLightData;

	std::vector<VertexPositionColor> m_pendingDebugLines;
	MeshPositionColor m_debugLinesMesh{ GL_LINES };

	struct DrawCallBase
	{
		const MeshBase &Mesh;
		glm::mat4 Model{ 1.0f };
		Material2 material;
	};

	std::vector<DrawCallBase> m_pendingBaseDrawCalls;
};