#pragma once

#define SE_USE_DOUBLE 0

#ifndef SE_FORCE_INLINE
#	if defined(_MSC_VER)
#		define SE_FORCE_INLINE __forceinline
#	else
#		define SE_FORCE_INLINE __inline
#	endif
#endif

#if SE_USE_DOUBLE
#	define	SE_MAX_SCALAR (DBL_MAX)
#	define	SE_EPSILON (DBL_EPSILON)
#else 
#	define	SE_MAX_SCALAR (FLT_MAX)
#	define	SE_EPSILON (FLT_EPSILON)
#endif