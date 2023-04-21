#pragma once

#if defined(_MSC_VER)
#	pragma warning(push, 0)
#endif

#include <Engine/stdafx.h>
#include <Engine/BaseFunc.h>
#include <Engine/EngineDevice.h>
#include <Engine/Camera.h>
#include <TinyDungeons/Ray.h>

#include <BulletLib/btBulletCollisionCommon.h>
#include <BulletLib/btBulletDynamicsCommon.h>

#define NOMINMAX
#include <BounceLib/bounce.h>

#include <iostream>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif