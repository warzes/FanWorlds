﻿#include "stdafx.h"
#include "001_Triangle.h"
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

	while( 1 )
	{
		system("CLS");
		std::cout << "Select Example (q - exit):";
		std::cout << "    b1 - Basic Triangle";
		std::cout << std::endl;

		std::string read;
		std::cin >> read;

		if( read == "q" )
			break;
		
		if( read == "b1" )
		{
			auto engineDevice = EngineDevice::Create({});
			engineDevice->RunApp(std::make_shared<_001Triangle>());
		}
	}

}
//-----------------------------------------------------------------------------