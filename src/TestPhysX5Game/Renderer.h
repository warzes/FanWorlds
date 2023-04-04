#pragma once

// TODO: delete

#include "MeshPositionOnly.h"
#include "MeshBase.h"

struct Material2;
class RenderSystem;

// point light attenuation from https://wiki.ogre3d.org/-Point+Light+Attenuation
// poly-fitted expression: log(y) = A * log(x) + B
static inline float PointLightAttenuationLinearFromRange(float range)
{
	return glm::exp(-1.00971225f * glm::log(range) + 1.54554086f);
}

static inline float PointLightAttenuationQuadraticFromRange(float range)
{
	return glm::exp(-2.01920616f * glm::log(range) + 4.41212873f);
}

class Renderer
{
public:
	MOVE_ONLY(Renderer);
	Renderer() = delete;
	Renderer(RenderSystem& renderSystem) : m_renderSystem(renderSystem) {}
	virtual ~Renderer() = default;

	void Resize(const glm::ivec2 &size)
	{
		m_screenSize = size;
		OnResize(size);
	}

	virtual void OnResize(const glm::ivec2 &size) = 0;

	[[nodiscard]] const glm::ivec2 &GetSize() const
	{
		return m_screenSize;
	}

	virtual void Update(float deltaTime) = 0;

	virtual void SetCameraInfo(const glm::mat4 &view, float fov, float near, float far) = 0;

	virtual void SetWorldLight(
		const glm::vec3 &lightDirection,
		const glm::vec3 &lightColor,
		const glm::vec3 &ambientColor
	) = 0;

	virtual void SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) = 0;

	virtual void BeginDraw() = 0;

	virtual void EndDraw() = 0;

	void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float range)
	{
		const float linear = PointLightAttenuationLinearFromRange(range);
		const float quadratic = PointLightAttenuationQuadraticFromRange(range);
		DrawPointLight(position, color, linear, quadratic);
	}

	virtual void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float linear, float quadratic) = 0;

	virtual void DebugDrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &color) = 0;

	void DebugDrawMarker(const glm::vec3 &position, float size);

	virtual void DrawMesh(const MeshBase &mesh, const glm::mat4 &model, const Material2 *material) = 0;

protected:
	RenderSystem& m_renderSystem;

private:
	glm::ivec2 m_screenSize{};
};