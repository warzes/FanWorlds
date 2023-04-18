#include "stdafx.h"
#include "EditorWorldObject.h"
#include "EditorCollectModels.h"
//-----------------------------------------------------------------------------
EditorMapObject& EditorMapObject::operator=(const EditorMapObject& ref)
{
	worldPosition = ref.worldPosition;
	modelPosition = ref.modelPosition;
	scale = ref.scale;
	rotation = ref.rotation;
	meshIndex = ref.meshIndex;

	fileNameModel = ref.fileNameModel;
	nameMeshInFile = ref.nameMeshInFile;
	return *this;
}
//-----------------------------------------------------------------------------
void EditorMapObject::Init(const EditorCollectModels& models)
{
	fileNameModel = models.GetFileModelName();
	nameMeshInFile = models.GetCurrentMeshName();
	meshIndex = models.GetCurrentMeshId();
	modelPosition = models.GetCurrentModelPosition();
	worldPosition = models.GetCurrentWorldPosition();
	scale = models.GetCurrentScale();
	rotation = models.GetCurrentRotation();
	aabb = models.GetCurrentAABB();
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