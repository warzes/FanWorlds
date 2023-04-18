#pragma once

class EditorMapObject
{
public:
	EditorMapObject& operator=(const EditorMapObject& ref);
	
	std::string fileNameModel;
	std::string nameMeshInFile;

	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;

	// temp - not save in json file
	unsigned meshIndex = 0;
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

	std::vector<EditorMapObjectHelper> object;
};