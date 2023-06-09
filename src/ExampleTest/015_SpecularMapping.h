#pragma once

class _015SpecularMapping final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;
	Uniform m_uniformCameraPosition;

	Uniform m_uniformLightDirection;
	Uniform m_uniformDiffuseLightColor;
	Uniform m_uniformSpecularColor;
	Uniform m_uniformSpecularPower;

	glm::mat4 m_perspective;
	GeometryBufferRef m_geom;
	Texture2DRef m_texture1;
	Texture2DRef m_texture2;
	Texture2DRef m_texture3;

	Camera m_camera;
	float m_rotation = 360.0f;
};