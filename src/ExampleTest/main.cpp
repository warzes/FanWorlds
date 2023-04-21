#include "stdafx.h"
// Basic Example
#include "001_Triangle.h"
#include "002_DinamicVertex.h"
#include "003_BufferMapping.h"
#include "004_Quad.h"
#include "005_BasicTexture.h"
#include "006_DiffuseLighting.h"
#include "007_AmbientLighting.h"
#include "008_SpecularLighting.h"
#include "009_MultiplePointLights.h"
#include "010_OtherMultipleLights.h"
#include "011_BasicObjModel.h"
#include "012_Framebuffer.h"
// Bullet Physics
#include "001_BPMinimal.h"
#include "002_BPRaycasting.h"
#include "XXX_BPExample.h"
// Other
#include "001_MonkeyScenes.h"

// физику с булета посмотреть в urho/rbfx

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
		std::cout << "Basic:" << std::endl;
		std::cout << "    b1 - Basic Draw Triangle" << std::endl;
		std::cout << "    b2 - Dinamic Vertex" << std::endl;
		std::cout << "    b3 - Buffer Mapping" << std::endl;
		std::cout << "    b4 - Draw Quad" << std::endl;
		std::cout << "    b5 - Draw Basic Texture" << std::endl;
		std::cout << "    b6 - Diffuse Lighting" << std::endl;
		std::cout << "    b7 - Ambient Lighting" << std::endl;
		std::cout << "    b8 - Specular Lighting" << std::endl;
		std::cout << "    b9 - Multiple Point Lights" << std::endl;
		std::cout << "   b10 - Other Multiple Lights" << std::endl;
		std::cout << "   b11 - Basic ObjModel Loader" << std::endl;
		std::cout << "   b12 - Framebuffer" << std::endl;

		std::cout << "Bullet Physics:" << std::endl;
		std::cout << "    bp1 - Minimal" << std::endl;
		std::cout << "    bp2 - Raycasting" << std::endl;
		std::cout << "    bpx - example" << std::endl;
		
		std::cout << "Other:" << std::endl;
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
		START_SCENE("b6", _006DiffuseLighting);
		START_SCENE("b7", _007AmbientLighting);
		START_SCENE("b8", _008SpecularLighting);
		//START_SCENE("b9", _006DiffuseLighting);
		START_SCENE("b10", _010OtherMultipleLights);
		START_SCENE("b11", _011BasicObjModel);
		START_SCENE("b12", _012Framebuffer);

		START_SCENE("bp1", _001BPMinimal);
		START_SCENE("bp2", _002BPRaycasting);
		START_SCENE("bpx", XXXBPExample);

		START_SCENE("o1", _001MonkeyScenes);

#undef START_SCENE
	}}
//-----------------------------------------------------------------------------