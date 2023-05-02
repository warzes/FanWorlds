#pragma once

class RenderUtils : public BaseClass
{
public:
	bool Create();
	void Destroy();

	void BeginDraw(const glm::mat4& proj, const glm::mat4& view);

	void DrawBox(const glm::vec3& position, const glm::vec3& scale);

	void DrawSphere(const glm::vec3& position, const glm::vec3& scale);

	void DrawAABB(const glm::vec3& position, const glm::vec3& scale);

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;

	ModelRef m_modelBox;
	Texture2DRef m_textureBox;

	ModelRef m_modelSphere;
	Texture2DRef m_textureSphere;

	ModelRef m_modelAABB;
	Texture2DRef m_textureAABB;
};