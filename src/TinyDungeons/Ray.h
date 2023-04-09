#pragma once

// RayCollision, ray hit information
struct RayCollision final
{
	bool hit = false; // Did the ray hit something?
	float distance = FLT_MAX; // Distance to nearest hit
	glm::vec3 point; // Point of nearest hit
	glm::vec3 normal; // Surface normal of hit
};

// Ray, ray for raycasting
class Ray final
{
public:
	glm::vec3 position; // Ray position (origin)
	glm::vec3 direction; // Ray direction
};

Ray GetMouseRay(const glm::ivec2& mousePos, int windowWidth, int windowHeight, const glm::mat4& projMat, const Camera& camera) noexcept;

// Get collision info between ray and triangle
RayCollision GetRayCollisionTriangle(const Ray& ray, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) noexcept;
// Get collision info between ray and quad
RayCollision GetRayCollisionQuad(const Ray& ray, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4) noexcept;