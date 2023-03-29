#include "stdafx.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#	pragma comment( lib, "Engine.lib" )
#endif
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma warning(disable : 4514) // /W4 - 'function' : unreferenced inline function has been removed
#	pragma warning(push, 0)
#endif
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include "WindowsInclude.h"
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif
//-----------------------------------------------------------------------------
#if defined(_WIN32)
#endif
#if defined(__linux__)
#endif
#if defined(__EMSCRIPTEN__)
#endif
//-----------------------------------------------------------------------------
// Turn argument to string constant:
// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html#Stringizing
#define SE_STRINGIZE( _n )   SE_STRINGIZE_2( _n )
#define SE_STRINGIZE_2( _n ) #_n

#if defined(_MSC_VER)
#	define TODO( _msg )  __pragma(message("" __FILE__ "(" SE_STRINGIZE(__LINE__) "): TODO: " _msg))
#else
#	define TODO( _msg )
#endif
//-----------------------------------------------------------------------------
// Example operator over
struct Foo
{
	constexpr Foo() = default;
	constexpr Foo(Foo&&) = default;
	constexpr Foo(const Foo&) = default;
	constexpr Foo(int scalar) : a(scalar) {}

	constexpr Foo& operator=(Foo&&) = default;
	constexpr Foo& operator=(const Foo&) = default;

	constexpr Foo operator+(int f) const { return { a + f }; }
	constexpr Foo operator+(const Foo& v) const { return { a + v.a }; }
	constexpr Foo& operator+=(const Foo& v) { a += v.a; return(*this); }

	constexpr int& operator[](size_t i) { return (&a)[i]; }
	constexpr const int operator[](size_t i) const { return (&a)[i]; }

	int a = 0;
};
inline bool operator==(const Foo& Left, const Foo& Right) noexcept { return Left.a == Right.a; }
inline Foo operator-(const Foo& In) noexcept { return { -In.a }; }
inline Foo operator-(int Left, const Foo& Right) noexcept { return { Left - Right.a }; }
inline Foo operator-(const Foo& Left, int Right) noexcept { return { Left.a - Right }; }
inline Foo operator-(const Foo& Left, const Foo& Right) noexcept { return { Left.a - Right.a }; }
inline Foo& operator-=(Foo& Left, int Right) noexcept { return Left = Left - Right; }
inline Foo& operator-=(Foo& Left, const Foo& Right) noexcept { return Left = Left - Right; }
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
}
//-----------------------------------------------------------------------------