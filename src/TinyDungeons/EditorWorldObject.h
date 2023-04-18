#pragma once

class EditorCollectModels;

class EditorMapObject
{
public:
	EditorMapObject& operator=(const EditorMapObject& ref);

	void Init(const EditorCollectModels& models);
	
	std::string fileNameModel;
	std::string nameMeshInFile;

	glm::vec3 worldPosition;
	glm::vec3 scale;
	glm::quat rotation;

	// temp - not save in json file
	glm::vec3 modelPosition;
	unsigned meshIndex = 0;
	AABB aabb;
};

class EditorMapObjectHelper
{
public:
	EditorMapObject object;
};

class EditorMap
{
public:
	void AddObject(const EditorMapObject& object);
	void RemoveLastObject();
	void RemoveObject(unsigned id);

	std::vector<EditorMapObjectHelper> object;
};