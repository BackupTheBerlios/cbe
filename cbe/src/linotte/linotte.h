// ===========================================================================
//	linotte.h		 	   	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include <size_t.h>
#include <gl.h>

#define USING_LINOTTE_NAMESPACE	using namespace linotte;

#define BEGIN_LINOTTE_NAMESPACE	namespace linotte {
#define END_LINOTTE_NAMESPACE	}

#if macintosh
#define LINOTTE_IS_BIG_ENDIAN 1
#elif __INTEL__
#define LINOTTE_IS_BIG_ENDIAN 0
#else
#error unknown operating system
#endif

BEGIN_LINOTTE_NAMESPACE

using namespace std;

typedef unsigned long long u64;
typedef unsigned long u32;
typedef unsigned short u16;
typedef unsigned char u08;

typedef signed long long s64;
typedef signed long s32;
typedef signed short s16;
typedef signed char s08;

typedef float f32;
typedef double f64;

typedef std::size_t size_t;

typedef s32 fpos_t;

inline u16 swap_endian32( u32 x )
{
	return ( ( ( ( x >> 8 ) & 0xff ) | ( ( x << 8 ) & 0xff00 ) ) << 16 ) |
		( ( ( x >> 24 ) & 0xff ) | ( ( ( x >> 16 ) << 8 ) & 0xff00 ) );
}

inline u16 swap_endian16( u16 x )
{
	return ( ( x >> 8 ) & 0xff ) | ( ( x << 8 ) & 0xff00 );
}

inline u32 cond_swap_endian32( u32 x )
{
#if LINOTTE_IS_BIG_ENDIAN
	return x;
#else
	return swap_endian32( x );
#endif
}

inline u16 cond_swap_endian16( u16 x )
{
#if LINOTTE_IS_BIG_ENDIAN
	return x;
#else
	return swap_endian16( x );
#endif
}
#define throw_linotte_error(x) throw std::runtime_error( x )

END_LINOTTE_NAMESPACE
