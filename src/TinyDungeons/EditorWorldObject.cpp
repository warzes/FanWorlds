#include "stdafx.h"
#include "EditorWorldObject.h"
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
void EditorMap::AddObject(const EditorMapObject& obj)
{
	object.push_back({ obj });
}
//-----------------------------------------------------------------------------
void EditorMap::RemoveLastObject()
{
	if( object.empty() ) return;
	object.pop_back();
}
//-----------------------------------------------------------------------------