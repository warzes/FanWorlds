#pragma once

class RenderUtils : public BaseClass
{
public:
	bool Create();
	void Destroy();

	void BeginDraw(const glm::mat4& proj, const glm::mat4& view);

	void DrawBox(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color = glm::vec3(1.0f));
	void DrawSphere(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color = glm::vec3(1.0f));
	void DrawAABB(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color = glm::vec3(1.0f));


	ModelRef GetModelBox() { return m_modelBox; }

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformProjectionMatrix;
	Uniform m_uniformViewMatrix;
	Uniform m_uniformWorldMatrix;
	Uniform m_uniformColor;

	ModelRef m_modelBox;
	Texture2DRef m_textureBox;

	ModelRef m_modelSphere;
	Texture2DRef m_textureSphere;

	ModelRef m_modelAABB;
	Texture2DRef m_textureAABB;
};