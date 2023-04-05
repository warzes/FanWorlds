#pragma once

class _001Triangle final : public IApp
{
	bool Create() final;
	void Destroy() final;

	void Render() final;
	void Update(float deltaTime) final;

private:
	int m_windowWidth = 0;
	int m_windowHeight = 0;

	std::shared_ptr<ShaderProgram> m_shader;
	Uniform m_uniformProjectionMatrix;
	std::shared_ptr<VertexBuffer> m_vb;
	std::shared_ptr<VertexArray> m_vao;
};