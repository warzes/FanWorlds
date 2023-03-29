#include "stdafx.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#	pragma comment( lib, "Engine.lib" )
#endif
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	extern void fnMy3rdparty(); // 3rdparty.lib
	extern void fnEngine();     // Engine.lib

	fnMy3rdparty();
	fnEngine();
}
//-----------------------------------------------------------------------------