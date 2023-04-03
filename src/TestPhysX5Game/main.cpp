#include "stdafx.h"
#include "GameApp.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "Engine.lib" )
#	pragma comment( lib, "PhysX_static_64.lib" )
#	pragma comment( lib, "PhysXCharacterKinematic_static_64.lib" )
#	pragma comment( lib, "PhysXCommon_static_64.lib" )
#	pragma comment( lib, "PhysXCooking_static_64.lib" )
#	pragma comment( lib, "PhysXExtensions_static_64.lib" )
#	pragma comment( lib, "PhysXFoundation_static_64.lib" )
#	pragma comment( lib, "PhysXPvdSDK_static_64.lib" )
#	pragma comment( lib, "PhysXVehicle_static_64.lib" )
#	pragma comment( lib, "PhysXVehicle2_static_64.lib" )
#endif
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	auto engineDevice = EngineDevice::Create({});
	engineDevice->RunApp(std::make_shared<GameApp>());
}
//-----------------------------------------------------------------------------