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
RayCollision GetRayCollisionBox(Ray ray, const AABB& box) noexcept
{
#if 1
	RayCollision collision = {};
	collision.hit = false;

	// Note: If ray.position is inside the box, the distance is negative (as if the ray was reversed)
	// Reversing ray.direction will give use the correct result.
	const bool insideBox = box.Contains(ray.position);
	if( insideBox ) ray.direction = -ray.direction;

	float t[11] = { 0 };
	t[8] = 1.0f / ray.direction.x;
	t[9] = 1.0f / ray.direction.y;
	t[10] = 1.0f / ray.direction.z;
	t[0] = (box.min.x - ray.position.x) * t[8];
	t[1] = (box.max.x - ray.position.x) * t[8];
	t[2] = (box.min.y - ray.position.y) * t[9];
	t[3] = (box.max.y - ray.position.y) * t[9];
	t[4] = (box.min.z - ray.position.z) * t[10];
	t[5] = (box.max.z - ray.position.z) * t[10];
	t[6] = fmax(fmax(fmin(t[0], t[1]), fmin(t[2], t[3])), fmin(t[4], t[5]));
	t[7] = fmin(fmin(fmax(t[0], t[1]), fmax(t[2], t[3])), fmax(t[4], t[5]));

	collision.hit = !((t[7] < 0) || (t[6] > t[7]));
	collision.distance = t[6];
	collision.point = ray.position + ray.direction * collision.distance;

	// Get box center point
#if 0
	collision.normal = Vector3Lerp(box.min, box.max, 0.5f);
#else
	collision.normal = glm::lerp(box.min, box.max, 0.5f);
#endif

	// Get vector center point->hit point
	collision.normal = collision.point - collision.normal;
	// Scale vector to unit cube
	// NOTE: We use an additional .01 to fix numerical errors
	collision.normal = collision.normal * 2.01f;
	collision.normal = collision.normal / (box.max - box.min);
	// The relevant elements of the vector are now slightly larger than 1.0f (or smaller than -1.0f)
	// and the others are somewhere between -1.0 and 1.0 casting to int is exactly our wanted normal!
	collision.normal.x = (float)((int)collision.normal.x);
	collision.normal.y = (float)((int)collision.normal.y);
	collision.normal.z = (float)((int)collision.normal.z);
	collision.normal = glm::normalize(collision.normal);

	if( insideBox )
	{
		// Reset ray.direction
		ray.direction = -ray.direction;
		// Fix result
		collision.distance *= -1.0f;
		collision.normal = -collision.normal;
	}

	return collision;
#else
	RayCollision collision = {};
	collision.hit = false;


	glm::vec3 tMin = (box.min - ray.position) / ray.direction;
	glm::vec3 tMax = (box.max - ray.position) / ray.direction;
	glm::vec3 t1 = glm::min(tMin, tMax);
	glm::vec3 t2 = glm::max(tMin, tMax);
	float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
	float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
	collision.hit = tNear < tFar;

	// TODO: доделать остальное
	return collision;
#endif
}
//-----------------------------------------------------------------------------