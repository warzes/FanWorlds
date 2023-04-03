#pragma once

#include "MdlStructs.h"

#include "Texture.h"
#include "MeshBase.h"

class MdlLoader
{
public:
	MdlLoader(SystemRef& systemRef, const std::string &filename);

	[[nodiscard]] std::vector<physx::PxVec3> GetColliderTriangles();

	[[nodiscard]] Texture* GetTexture() const
	{
		return new Texture{ m_systemRef.GetRenderSystem(), {m_skinWidth, m_skinHeight}, m_textureData.data()};
	}

	[[nodiscard]] MeshBase GetMesh() const
	{
		return { m_meshVertices, GL_TRIANGLES };
	}

private:
	SystemRef& m_systemRef;
	bool ReadBytes(size_t numBytes, void *output);

	template<typename T>
	bool Read(std::vector<T> &vector)
	{
		return ReadBytes(vector.size() * sizeof(T), vector.data());
	}

	template<typename T>
	bool Read(T &value) { return ReadBytes(sizeof(T), &value); }

	bool LoadHeader();

	bool LoadSkins();

	bool LoadTextureCoords();

	bool LoadTriangles();

	bool LoadFrames();

	bool LoadSimpleFrame();

	VertexBase GenerateVertex(const std::vector<trivertx_t> &vertices, const dtriangle_t &triangle, int indexInTriangle);

	std::string m_bytes;
	const char *m_currentPos;
	size_t m_remainingBytes;

	glm::vec3 m_scale{};
	glm::vec3 m_origin{};
	float m_radius{};
	glm::vec3 m_eyePosition{};
	long m_numSkins{};
	long m_skinWidth{};
	long m_skinHeight{};
	long m_numVertices{};
	long m_numTriangles{};
	long m_numFrames{};
	long m_syncType{};
	long m_flags{};
	float m_size{};

	std::vector<stvert_t> m_textureCoords;
	std::vector<dtriangle_t> m_triangles;

	// one skin + one frame (temporary)
	std::vector<unsigned char> m_textureData;
	std::vector<VertexBase> m_meshVertices;
};