#pragma once

class DrawGrid
{
public:
	bool Create(RenderSystem& renderSystem);
	void Destroy();

	void Draw(RenderSystem& renderSystem, const glm::mat4& vpMat, const glm::vec3& pos, float stepGrid, float size);

private:
	ShaderProgramRef m_shader;
	Uniform m_uniformViewProjectionMatrix;
	Uniform m_uniformWorldMatrix;
	Uniform m_uniformSizeGrid;
	Uniform m_uniformStepGrid;
	Uniform m_uniformColor;

	GeometryBufferRef m_geom;
	Texture2DRef m_texture;
};