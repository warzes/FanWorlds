#include "stdafx.h"
// Basic Example
#include "001_Triangle.h"
#include "002_DinamicVertex.h"
#include "003_BufferMapping.h"
#include "004_Quad.h"
#include "005_BasicTexture.h"
#include "006_MultipleLights.h"
#include "007_BasicObjModel.h"
#include "008_Framebuffer.h"
// Other
#include "001_MonkeyScenes.h"
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
		std::cout << "Select Example (q - exit):" << std::endl;
		std::cout << "    b1 - Basic Draw Triangle" << std::endl;
		std::cout << "    b2 - Dinamic Vertex" << std::endl;
		std::cout << "    b3 - Buffer Mapping" << std::endl;
		std::cout << "    b4 - Draw Quad" << std::endl;
		std::cout << "    b5 - Draw Basic Texture" << std::endl;
		std::cout << "    b6 - Basic Multiple Lights" << std::endl;
		std::cout << "    b7 - Basic ObjModel Loader" << std::endl;
		std::cout << "    b8 - Framebuffer" << std::endl;

		std::cout << "    o1 - Monkey Scenes" << std::endl;
		std::cout << std::endl;

		std::string read;
		std::cin >> read;

		if( read == "q" )
			break;

#define START_SCENE(arg, x) \
		if( read == arg ) \
		{ \
			auto engineDevice = EngineDevice::Create({}); \
			engineDevice->RunApp(std::make_shared<x>()); \
		}
		START_SCENE("b1", _001Triangle);
		START_SCENE("b2", _002DinamicVertex);
		START_SCENE("b3", _003BufferMapping);
		START_SCENE("b4", _004Quad);
		START_SCENE("b5", _005BasicTexture);
		START_SCENE("b6", _006MultipleLights);
		START_SCENE("b7", _007BasicObjModel);
		START_SCENE("b8", _008Framebuffer);

		START_SCENE("o1", _001MonkeyScenes);

#undef START_SCENE
	}}
//-----------------------------------------------------------------------------