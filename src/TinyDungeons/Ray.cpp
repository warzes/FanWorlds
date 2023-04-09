#include "stdafx.h"
#include "Ray.h"

#define GLM_IMPL 0
#define GLM_UNPROJCT_IMPL 1

Ray GetMouseRay(const glm::ivec2& mousePos, int windowWidth, int windowHeight, const glm::mat4& projMat, const Camera& camera) noexcept
{
	Ray ray;
	ray.position = camera.position;

#if GLM_IMPL
	// these positions must be in range [-1, 1] (!!!), not [0, width] and [0, height]
	const float mouseX = mousePos.x / (windowWidth * 0.5f) - 1.0f;
	const float mouseY = mousePos.y / (windowHeight * 0.5f) - 1.0f;

	glm::mat4 invVP = glm::inverse(camera.GetPerspective() * camera.GetViewMatrix());
	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
	glm::vec4 worldPos = invVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));
	ray.direction = dir; // TODO: неправильно, нужно смотреть как с glm::unProject я сделал
#elif GLM_UNPROJCT_IMPL
	const glm::vec4 viewport = { 0.0f, 0.0f, (float)windowWidth, (float)windowHeight };
	const glm::vec3 dir = glm::unProject({ (float)mousePos.x, (float)(windowHeight - mousePos.y), 1.0f }, camera.GetViewMatrix(), projMat, viewport);
	ray.direction = glm::normalize(dir - ray.position); // возможно это тоже не совсем верно, скорее нужно вычислять farPoint - nearPoint. farPoint - это dir а вот nearPoint- ??? Но на текущий момент этот код работает как надо, так что оставляю как есть
#endif

	return ray;
}

// Get collision info between ray and triangle
// NOTE: The points are expected to be in counter-clockwise winding
// NOTE: Based on https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
RayCollision GetRayCollisionTriangle(const Ray& ray, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) noexcept
{
#define EPSILON 0.000001f // A small number
	RayCollision collision = {};

	// Find vectors for two edges sharing V1
	const glm::vec3 edge1 = p2 - p1;
	const glm::vec3 edge2 = p3 - p1;

	// Begin calculating determinant - also used to calculate u parameter
	const glm::vec3 p = glm::cross(ray.direction, edge2);

	// If determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	const float det = glm::dot(edge1, p);
	// Avoid culling!
	if ((det > -EPSILON) && (det < EPSILON)) return collision;
	const float invDet = 1.0f / det;

	// Calculate distance from V1 to ray origin
	const glm::vec3 tv = ray.position - p1;

	// Calculate u parameter and test bound
	const float u = glm::dot(tv, p) * invDet;
	// The intersection lies outside of the triangle
	if ((u < 0.0f) || (u > 1.0f)) return collision;

	// Prepare to test v parameter
	const glm::vec3 q = glm::cross(tv, edge1);

	// Calculate V parameter and test bound
	const float v = glm::dot(ray.direction, q) * invDet;
	// The intersection lies outside of the triangle
	if ((v < 0.0f) || ((u + v) > 1.0f)) return collision;

	const float t = glm::dot(edge2, q) * invDet;
	if (t > EPSILON)
	{
		// Ray hit, get hit point and normal
		collision.hit = true;
		collision.distance = t;
		collision.normal = glm::normalize(glm::cross(edge1, edge2));
		collision.point = (ray.position + (ray.direction * t));
	}

	return collision;
}
//-----------------------------------------------------------------------------
RayCollision GetRayCollisionQuad(const Ray& ray, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4) noexcept
{
	RayCollision collision = GetRayCollisionTriangle(ray, p1, p2, p4);
	if (!collision.hit) collision = GetRayCollisionTriangle(ray, p2, p3, p4);
	return collision;
}
//-----------------------------------------------------------------------------