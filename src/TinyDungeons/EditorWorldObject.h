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
	glm::vec3 eulerRot;

	// temp - not save in json file
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

	void Update();

	std::vector<EditorMapObjectHelper> object;

	bool m_edit = true;
};