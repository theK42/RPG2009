
#pragma once

namespace XEngine
{
	template< typename T >
	inline T Min( T x, T y )
	{
		return (y < x) ? y : x;
	}

	template< typename T >
	inline T Max( T x, T y )
	{
		return (y > x) ? y : x;
	}

	template< typename T >
	inline T Clamp( T x, T lo, T hi )
	{
		return (x < lo) ? lo : (x > hi) ? hi : x;
	}

	template < typename T >
	inline bool IsBounded( T x, T lo, T hi )
	{
		return (x >= lo) && (x <= hi);
	}
}
