#include "stdafx.h"
#include "EditorWorldObject.h"

EditorMapObject& EditorMapObject::operator=(const EditorMapObject& ref)
{
	position = ref.position;
	scale = ref.scale;
	rotation = ref.rotation;
	meshIndex = ref.meshIndex;

	fileNameModel = ref.fileNameModel;
	nameMeshInFile = ref.nameMeshInFile;
	return *this;
}

void EditorMap::AddObject(const EditorMapObject& obj)
{
	for( size_t i = 0; i < object.size(); i++ )
	{
		if( object[i].isLive == false )
		{
			object[i].object = obj;
			return;
		}
	}
	object.push_back({ obj, true });
}