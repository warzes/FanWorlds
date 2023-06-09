#pragma once

struct _010LightClass
{
	glm::vec4 diffuseColor;
	glm::vec3 direction;
	glm::vec4 ambientLight;
};

class _010AmbientLighting final : public IApp
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
	Uniform m_uniformLightDirection;
	Uniform m_uniformLightDiffuseColor;
	Uniform m_uniformLightAmbientColor;

	glm::mat4 m_perspective;

	GeometryBufferRef m_geom;
	Texture2DRef m_texture;

	_010LightClass m_light;

	Camera m_camera;

	float m_rotation = 360.0f;
};