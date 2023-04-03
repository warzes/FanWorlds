#include "stdafx.h"
#include "LoadEntities.h"
#include "Scene.h"
#include "AWorldSpawn.h"
#include "ACamera.h"
#include "APlayer.h"
#include "ALightPoint.h"
#include "APropStatic.h"
#include "AFuncBrush.h"
#include "AFuncMove.h"
#include "AFuncPhys.h"

void LoadWorldSpawn(Scene* scene, const EntityDefinition &definition)
{
	scene->CreateActor<AWorldSpawn>(*scene, definition.Brushes);
}

void LoadInfoPlayerStart(Scene* scene, const EntityDefinition &definition)
{
	glm::vec3 origin;
	if( !definition.GetPropertyVector("origin", origin) )
	{
		scene->Error("info_player_start doesn't have a valid origin!");
	}

	int angle = 0;
	(void)definition.GetPropertyInteger("angle", angle);
	angle -= 90; // the direction in Trenchbroom is biased

	auto *camera = scene->CreateActor<ACamera>();
	auto *player = scene->CreateActor<APlayer>(*scene, origin, glm::radians(static_cast<float>(angle)));
	camera->SetTargetActor(player);
}

void LoadLightPoint(Scene* scene, const EntityDefinition &definition)
{
	glm::vec3 origin;
	if( !definition.GetPropertyVector("origin", origin) )
	{
		scene->Error("light_point doesn't have a valid origin!");
	}

	glm::vec3 color{ 1.0f, 1.0f, 1.0f };
	(void)definition.GetPropertyColor("color", color);

	int range = 20;
	(void)definition.GetPropertyInteger("range", range);

	scene->CreateActor<ALightPoint>(origin, color, static_cast<float>(range));
}

void LoadPropStatic(Scene* scene, const EntityDefinition &definition)
{
	const std::string &model = definition.GetProperty("model");
	if ( model.empty() )
	{
		scene->Error("prop_static doesn't have a valid model!");
	}

	glm::vec3 origin;
	if( !definition.GetPropertyVector("origin", origin) )
	{
		scene->Error("prop_static doesn't have a valid origin!");
	}
	scene->CreateActor<APropStatic>(*scene, model, origin);
}

void LoadFuncBrush(Scene* scene, const EntityDefinition &definition)
{
	scene->CreateActor<AFuncBrush>(*scene, definition.Brushes);
}

void LoadFuncMove(Scene* scene, const EntityDefinition &definition)
{
	glm::vec3 moveSpeed;
	if( !definition.GetPropertyVector("move_speed", moveSpeed) )
	{
		scene->Error("func_move doesn't have a valid move_speed!");
	}

	float moveTime;
	if( !definition.GetPropertyFloat("move_time", moveTime) )
	{
		scene->Error("func_move doesn't have a valid move_time!");
	}
	scene->CreateActor<AFuncMove>(*scene, definition.Brushes, moveSpeed, moveTime);
}

void LoadFuncPhys(Scene* scene, const EntityDefinition &definition)
{
	scene->CreateActor<AFuncPhys>(*scene, definition.Brushes);
}

typedef void (*EntityLoader)(Scene* scene, const EntityDefinition &definition);

static const std::unordered_map<std::string, EntityLoader> s_EntityLoaders{
{"worldspawn",        LoadWorldSpawn},
{"info_player_start", LoadInfoPlayerStart},
{"light_point",       LoadLightPoint},
{"prop_static",       LoadPropStatic},
{"func_brush",        LoadFuncBrush},
{"func_move",         LoadFuncMove},
{"func_phys",         LoadFuncPhys}
};

void LoadEntities(Scene* scene, const std::vector<EntityDefinition> &entities)
{
	for( auto &entity : entities )
	{
		const std::string &className = entity.GetProperty("classname");
		auto loader = s_EntityLoaders.find(className);
		if( loader == s_EntityLoaders.end() )
		{
			scene->Error("Unknown entity type " + className);
			return;
		}
		loader->second(scene, entity);
	}
}