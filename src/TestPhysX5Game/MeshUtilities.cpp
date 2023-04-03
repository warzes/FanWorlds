#include "stdafx.h"
#include "MeshUtilities.h"

std::vector<VertexBase> CreateBox(const glm::vec3 &min, const glm::vec3 &max)
{
	const glm::vec3 p000{ min.x, min.y, min.z };
	const glm::vec3 p001{ min.x, min.y, max.z };
	const glm::vec3 p010{ min.x, max.y, min.z };
	const glm::vec3 p011{ min.x, max.y, max.z };
	const glm::vec3 p100{ max.x, min.y, min.z };
	const glm::vec3 p101{ max.x, min.y, max.z };
	const glm::vec3 p110{ max.x, max.y, min.z };
	const glm::vec3 p111{ max.x, max.y, max.z };

	constexpr glm::vec3 npx{ 1, 0, 0 };
	constexpr glm::vec3 nnx{ -1, 0, 0 };
	constexpr glm::vec3 npy{ 0, 1, 0 };
	constexpr glm::vec3 nny{ 0, -1, 0 };
	constexpr glm::vec3 npz{ 0, 0, 1 };
	constexpr glm::vec3 nnz{ 0, 0, -1 };

	const float width = max.x - min.x;
	const float height = max.y - min.y;
	const float depth = max.z - min.z;

	constexpr glm::vec2 uvx00{ 0.0f, 0.0f };
	const glm::vec2 uvx01{ 0.0f, height };
	const glm::vec2 uvx10{ depth, 0.0f };
	const glm::vec2 uvx11{ depth, height };

	constexpr glm::vec2 uvy00{ 0.0f, 0.0f };
	const glm::vec2 uvy01{ 0.0f, depth };
	const glm::vec2 uvy10{ width, 0.0f };
	const glm::vec2 uvy11{ width, depth };

	constexpr glm::vec2 uvz00{ 0.0f, 0.0f };
	const glm::vec2 uvz01{ 0.0f, height };
	const glm::vec2 uvz10{ width, 0.0f };
	const glm::vec2 uvz11{ width, height };

	std::vector<VertexBase> vertices{
		// +x
		{p101, npx, uvx00},
		{p100, npx, uvx10},
		{p111, npx, uvx01},
		{p111, npx, uvx01},
		{p100, npx, uvx10},
		{p110, npx, uvx11},
		// -x
		{p000, nnx, uvx00},
		{p001, nnx, uvx10},
		{p010, nnx, uvx01},
		{p010, nnx, uvx01},
		{p001, nnx, uvx10},
		{p011, nnx, uvx11},
		// posY
		{p011, npy, uvy00},
		{p111, npy, uvy10},
		{p010, npy, uvy01},
		{p010, npy, uvy01},
		{p111, npy, uvy10},
		{p110, npy, uvy11},
		// -y
		{p000, nny, uvy00},
		{p100, nny, uvy10},
		{p001, nny, uvy01},
		{p001, nny, uvy01},
		{p100, nny, uvy10},
		{p101, nny, uvy11},
		// +z
		{p001, npz, uvz00},
		{p101, npz, uvz10},
		{p011, npz, uvz01},
		{p011, npz, uvz01},
		{p101, npz, uvz10},
		{p111, npz, uvz11},
		// -z
		{p100, nnz, uvz00},
		{p000, nnz, uvz10},
		{p110, nnz, uvz01},
		{p110, nnz, uvz01},
		{p000, nnz, uvz10},
		{p010, nnz, uvz11}
	};

	return vertices;
}