#pragma once

struct _008LightClass
{
	glm::vec4 diffuseColor;
	glm::vec3 direction;
	glm::vec4 ambientLight;
	glm::vec4 specularColor;
	float specularPower;
};

class _008SpecularLighting final : public IApp
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
	Uniform m_uniformLightDiffuseColor;
	Uniform m_uniformLightAmbientColor;
	Uniform m_uniformLightSpecularPower;
	Uniform m_uniformLightSpecularColor;

	glm::mat4 m_perspective;

	GeometryBufferRef m_geom;
	Texture2DRef m_texture;

	_008LightClass m_light;

	Camera m_camera;

	float m_rotation = 360.0f;
};