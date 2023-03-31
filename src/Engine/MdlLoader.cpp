#include "stdafx.h"
#include "MdlLoader.h"
#include "MdlPalette.h"
#include "MdlNormals.h"
#include "FileSystem.h"
//-----------------------------------------------------------------------------
MdlLoader::MdlLoader(SystemRef& systemRef, const std::string &filename)
	: m_systemRef(systemRef)
	, m_bytes(systemRef.GetFileSystem().Read(filename))
	, m_currentPos(m_bytes.data())
	, m_remainingBytes(m_bytes.size())
{
	if( !LoadHeader() )
	{
		systemRef.Error("Invalid header");
		return;
	}

	if( !LoadSkins() )
	{
		systemRef.Error("Failed to load textures");
		return;
	}

	if( !LoadTextureCoords() )
	{
		systemRef.Error("Failed to load texture coords");
		return;
	}

	if( !LoadTriangles() )
	{
		systemRef.Error("Failed to load triangles");
		return;
	}

	if( !LoadFrames() )
	{
		systemRef.Error("Failed to load frames");
		return;
	}
}
//-----------------------------------------------------------------------------
bool MdlLoader::ReadBytes(size_t numBytes, void *output)
{
	if( m_remainingBytes < numBytes )
	{
		// not enough data
		return false;
	}
	memcpy(output, m_currentPos, numBytes);
	m_currentPos += numBytes;
	m_remainingBytes -= numBytes;
	return true;
}
//-----------------------------------------------------------------------------
bool MdlLoader::LoadHeader()
{
	mdl_t header;

	if( !Read(header) ) return false;

	assert(header.ident == (('O' << 24) + ('P' << 16) + ('D' << 8) + 'I'));
	assert(header.version == 6);

	m_scale = header.scale;
	m_origin = header.scale_origin;
	m_radius = header.boundingradius;
	m_eyePosition = header.eyeposition;

	m_numSkins = header.numskins;
	m_skinWidth = header.skinwidth;
	m_skinHeight = header.skinheight;

	m_numVertices = header.numverts;
	m_numTriangles = header.numtris;
	m_numFrames = header.numframes;

	m_syncType = header.synctype;
	m_flags = header.flags;

	if( m_numSkins != 1 )
	{
		m_systemRef.Error("Multiple skin is not supported yet");
		return false;
	}

	if( m_numFrames != 1 )
	{
		m_systemRef.Error("Animated mdl is not supported yet");
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
bool MdlLoader::LoadSkins()
{
	const size_t skinDataSize = m_skinWidth * m_skinHeight;
	for( int i = 0; i < m_numSkins; i++ )
	{
		int32_t group;
		if( !Read(group) ) return false;

		if( group != 0 )
		{
			m_systemRef.Error("Group skins is not supported yet");
			return false;
		}

		if( group == 0 )
		{
			// single skin

			std::vector<uint8_t> skinData(skinDataSize);
			if( !Read(skinData) ) return false;

			m_textureData.resize(skinDataSize * 4);
			for( int j = 0; j < skinDataSize; j++ )
			{
				const RgbU8 &color = MDL_PALETTE[skinData[j]];
				m_textureData[4 * j + 0] = color.R;
				m_textureData[4 * j + 1] = color.G;
				m_textureData[4 * j + 2] = color.B;
				m_textureData[4 * j + 3] = 0xFF;
			}
		}
		else
		{
			// group skins

			int32_t number;
			if( !Read(number) ) return false;

			std::vector<float> time(number);
			if( !Read(time) ) return false;

			for( int j = 0; j < number; j++ )
			{
				std::vector<uint8_t> skinData(skinDataSize);
				if( !Read(skinData) ) return false;
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool MdlLoader::LoadTextureCoords()
{
	m_textureCoords.resize(m_numVertices);
	if( !Read(m_textureCoords) ) return false;
	return true;
}
//-----------------------------------------------------------------------------
bool MdlLoader::LoadTriangles()
{
	m_triangles.resize(m_numTriangles);
	if( !Read(m_triangles) ) return false;
	return true;
}
//-----------------------------------------------------------------------------
bool MdlLoader::LoadFrames()
{
	for( int i = 0; i < m_numFrames; i++ )
	{
		int type;
		if( !Read(type) ) return false;

		if( type != 0 )
		{
			m_systemRef.Error("Group frame is not supported yet");
			return false;
		}

		if( type == 0 )
		{
			// one simple frame
			LoadSimpleFrame();
		}
		else
		{
			// group of simple frames
			int numSimpleFrames = type;

			trivertx_t min;
			if( !Read(min) ) return false;

			trivertx_t max;
			if( !Read(max) ) return false;

			std::vector<float> time(numSimpleFrames);
			if( !Read(time) ) return false;

			for( int j = 0; j < numSimpleFrames; j++ )
			{
				LoadSimpleFrame();
			}
		}
	}

	return true;
}
//-----------------------------------------------------------------------------
bool MdlLoader::LoadSimpleFrame()
{
	daliasframe_t frame;
	if( !Read(frame) ) return false;

	std::vector<trivertx_t> vertices(m_numVertices);
	if( !Read(vertices) ) return false;

	m_meshVertices.reserve(m_numVertices);
	for( const auto &triangle : m_triangles )
	{
		// reversed because the winding order is wrong
		VertexBase v0 = GenerateVertex(vertices, triangle, 2);
		VertexBase v1 = GenerateVertex(vertices, triangle, 1);
		VertexBase v2 = GenerateVertex(vertices, triangle, 0);

		// recalculate triangle normal
		glm::vec3 edge1 = v1.Position - v0.Position;
		glm::vec3 edge2 = v2.Position - v1.Position;
		glm::vec3 normal = glm::cross(edge1, edge2);

		v0.Normal = normal;
		v1.Normal = normal;
		v2.Normal = normal;

		m_meshVertices.push_back(v0);
		m_meshVertices.push_back(v1);
		m_meshVertices.push_back(v2);
	}

	return true;
}
//-----------------------------------------------------------------------------
VertexBase MdlLoader::GenerateVertex(const std::vector<trivertx_t> &vertices, const dtriangle_t &triangle, int indexInTriangle)
{
	const int vertexIndex = triangle.vertindex[indexInTriangle];
	const trivertx_t &frameVertex = vertices[vertexIndex];

	glm::vec3 packedPosition{ frameVertex.v[0], frameVertex.v[1], frameVertex.v[2] };
	glm::vec3 quakePosition = m_scale * packedPosition + m_origin;

	// convert from quake coords to haru coords
	glm::vec3 position{
	quakePosition.x / 32.0f,
	quakePosition.z / 32.0f,
	-quakePosition.y / 32.0f
	};

	glm::vec3 quakeNormal = MDL_NORMALS[frameVertex.lightnormalindex];

	// convert from quake coords to haru coords
	glm::vec3 normal{
	quakeNormal.x,
	quakeNormal.z,
	-quakeNormal.y
	};

	const stvert_t &rawTextureCoords = m_textureCoords[vertexIndex];
	int s = rawTextureCoords.s;
	int t = rawTextureCoords.t;
	if( triangle.facesfront && rawTextureCoords.onseam )
	{
		s += m_skinWidth / 2;
	}
	glm::vec2 textureCoords{
	(static_cast<float>(s) + 0.5f) / static_cast<float>(m_skinWidth),
	(static_cast<float>(t) + 0.5f) / static_cast<float>(m_skinHeight)
	};

	return { position, normal, textureCoords };
}
//-----------------------------------------------------------------------------
std::vector<physx::PxVec3> MdlLoader::GetColliderTriangles()
{
	std::vector<physx::PxVec3> triangles;
	for( const VertexBase &vertex : m_meshVertices )
	{
		const glm::vec3 &position = vertex.Position;
		triangles.emplace_back(position.x, position.y, position.z);
	}
	return triangles;
}
//-----------------------------------------------------------------------------