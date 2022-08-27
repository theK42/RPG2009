
#pragma once

#define alignof(x)	XALIGNOF(x)
#define countof(x)	(sizeof(x)/sizeof((x)[0]))

#if defined( HOST_MS )

namespace std
{

	typedef signed __int8			int8_t;
	typedef signed __int16			int16_t;
	typedef signed __int32			int32_t;
	typedef signed __int64			int64_t;

	typedef unsigned __int8			uint8_t;
	typedef unsigned __int16		uint16_t;
	typedef unsigned __int32		uint32_t;
	typedef unsigned __int64		uint64_t;

#	if POINTER_SIZE == 8

	typedef signed __int64			intptr_t;
	typedef unsigned __int64		uintptr_t;

#	elif POINTER_SIZE == 4

	typedef signed __int32 __w64	intptr_t;
	typedef unsigned __int32 __w64	uintptr_t;

#	else
#		error Unsupported pointer size!
#	endif

} /// namespace std

#else
#	include <cstdint>
#endif

namespace XEngine {

	typedef std::int8_t		s8;
	typedef std::int16_t	s16;
	typedef std::int32_t	s32;
	typedef std::int64_t	s64;
	typedef std::intptr_t	saddr;

	typedef std::uint8_t	u8;
	typedef std::uint16_t	u16;
	typedef std::uint32_t	u32;
	typedef std::uint64_t	u64;
	typedef std::uintptr_t	uaddr;

	typedef unsigned		uint;
	typedef void *			handle;
#ifndef _MANAGED
	static int const nullptr = 0;
#endif

	enum XChecksum
	{
		XChecksum_ForceU32 = 0xFFFFFFFFu
	};

} /// namespace XEngine
