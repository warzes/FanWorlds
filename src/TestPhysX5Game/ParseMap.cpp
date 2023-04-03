#include "stdafx.h"
#include "ParseMap.h"

static void ParseEntityProperties(std::stringstream &mapStream, EntityProperties &properties)
{
	int numProperties;
	mapStream >> numProperties;

	// read the line end after numProperties
	std::string dummy;
	std::getline(mapStream, dummy);

	for( int i = 0; i < numProperties; i++ )
	{
		std::string key;
		std::getline(mapStream, key);

		std::string value;
		std::getline(mapStream, value);

		properties[key] = value;
	}
}

static void ParseEntityBrushFace(std::stringstream &mapStream, EntityBrushes &brushes)
{
	std::string textureName;
	mapStream >> textureName;

	auto &vertices = brushes.TextureToVertices[textureName];

	glm::vec3 normal;
	mapStream >> normal.x >> normal.y >> normal.z;

	int brushFaceVertexCount;
	mapStream >> brushFaceVertexCount;

	std::vector<glm::vec3> brushFaceVertexPositions(brushFaceVertexCount);
	std::vector<glm::vec2> brushFaceVertexTextureCoords(brushFaceVertexCount);
	for( int k = 0; k < brushFaceVertexCount; k++ )
	{
		glm::vec3 &position = brushFaceVertexPositions[k];
		mapStream >> position.x >> position.y >> position.z;
		glm::vec2 &textureCoord = brushFaceVertexTextureCoords[k];
		mapStream >> textureCoord.x >> textureCoord.y;
	}

	// triangulate
	for( int k = 1; k < brushFaceVertexCount - 1; k++ )
	{
		vertices.push_back({ brushFaceVertexPositions[0], normal, brushFaceVertexTextureCoords[0] });
		vertices.push_back({ brushFaceVertexPositions[k], normal, brushFaceVertexTextureCoords[k] });
		vertices.push_back({ brushFaceVertexPositions[k + 1], normal, brushFaceVertexTextureCoords[k + 1] });
	}
}

static void ParseEntityBrush(std::stringstream &mapStream, EntityBrushes &brushes, int brushIndex)
{
	int numColliderVertices;
	mapStream >> numColliderVertices;

	std::vector<physx::PxVec3> &colliderVertices = brushes.Colliders[brushIndex];
	colliderVertices.reserve(numColliderVertices);
	for( int j = 0; j < numColliderVertices; j++ )
	{
		physx::PxVec3 vertex;
		mapStream >> vertex.x >> vertex.y >> vertex.z;
		colliderVertices.push_back(vertex);
	}

	int numBrushFaces;
	mapStream >> numBrushFaces;
	for( int j = 0; j < numBrushFaces; j++ )
	{
		ParseEntityBrushFace(mapStream, brushes);
	}
}

static void ParseEntityBrushes(std::stringstream &mapStream, EntityBrushes &brushes)
{
	int numBrushes;
	mapStream >> numBrushes;

	brushes.Colliders.resize(numBrushes);

	for( int i = 0; i < numBrushes; i++ )
	{
		ParseEntityBrush(mapStream, brushes, i);
	}
}

void ParseMap(SystemRef& systemRef, const std::string &mapFilename, std::vector<EntityDefinition> &entities)
{
	std::stringstream mapStream(systemRef.GetFileSystem().Read(mapFilename));

	int numEntities;
	mapStream >> numEntities;

	entities.resize(numEntities);
	for( EntityDefinition &entity : entities )
	{
		ParseEntityProperties(mapStream, entity.Properties);
		ParseEntityBrushes(mapStream, entity.Brushes);
	}
}