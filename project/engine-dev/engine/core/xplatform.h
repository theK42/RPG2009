
#pragma once

#if defined( __GNUC__ )
#	define XHOST_GNU
#	define XLIKELY(x)		(__builtin_expect((x),1))
#	define XUNLIKELY(x)		(__builtin_expect((x),0))
#	define XASSUME(x)		((void)sizeof(x))
#	define XBREAK			__builtin_trap()
#	define XALIGNOF(x)		__alignof__(x)
#	define XFASTCALL		__attribute__((__fastcall__))
#	define XALIGNED(N)		__attribute__((__aligned__(N)))
#	define XTHREAD_LOCAL	__thread
#	define XNEVER_INLINE	__attribute__((__noinline__))
#elif defined( _MSC_VER )
#	define HOST_MS
#	define XLIKELY(x)		(x)
#	define XUNLIKELY(x)		(x)
#	define XASSUME(x)		__assume(x)
#	define XBREAK			__debugbreak()
#	define XALIGNOF(x)		__alignof(x)
#	define XFASTCALL		__fastcall
#	define XALIGNED(N)		__declspec(align(N))
#	define XTHREAD_LOCAL	__declspec(thread)
#	define XNEVER_INLINE	__declspec(noinline)
#else
#	error Unknown host compiler!
#endif

#if defined( __IPHONEOS__ )
#	define TARGET_POSIX
#	define TARGET_IPHONEOS
#	define BIG_ENDIAN
#	define POINTER_SIZE		4
#elif defined( _WIN32 )
#	if defined( _WIN64 )
#		define TARGET_MSWIN
#		define TARGET_MSWIN64
#		define LITTLE_ENDIAN
#		define POINTER_SIZE		8
#	else
#		define TARGET_MSWIN
#		define TARGET_MSWIN32
#		define LITTLE_ENDIAN
#		define POINTER_SIZE		4
#	endif
#else
#	error Unknown target platform!
#endif
