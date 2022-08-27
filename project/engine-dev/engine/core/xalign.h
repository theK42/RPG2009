
#pragma once

namespace XEngine
{
	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	template< typename T >
	inline bool IsPowerOfTwo( T x )
	{
		uaddr u = static_cast< uaddr >( x );
		return (u & (u - 1)) == 0;
	}

	///------------------------------------------------------------------------

	template< typename T >
	inline bool IsPowerOfTwo( T * x )
	{
		uaddr u = reinterpret_cast< uaddr >( x );
		return (u & (u - 1)) == 0;
	}

	///------------------------------------------------------------------------

	template< typename T >
	inline bool IsAligned( T x, uaddr n )
	{
		return (static_cast< uaddr >( x ) & (n - 1)) == 0;
	}

	///------------------------------------------------------------------------

	template< typename T >
	inline bool IsAligned( T * x, uaddr n )
	{
		return (reinterpret_cast< uaddr >( x ) & (n - 1)) == 0;
	}

	///------------------------------------------------------------------------

	template< typename T >
	inline T AlignUp( T x, uaddr n )
	{
		return static_cast< T >(
			(static_cast< uaddr >( x ) + (n - 1)) & ~(n - 1) );
	}

	///------------------------------------------------------------------------

	template< typename T >
	inline T * AlignUp( T * x, uaddr n )
	{
		return reinterpret_cast< T * >(
			(reinterpret_cast< uaddr >( x ) + (n - 1)) & ~(n - 1) );
	}

	///------------------------------------------------------------------------

	template< typename T >
	inline T AlignDown( T x, uaddr n )
	{
		return static_cast< T >(
			static_cast< uaddr >( x ) & ~(n - 1) );
	}

	///------------------------------------------------------------------------

	template< typename T >
	inline T * AlignDown( T * x, uaddr n )
	{
		return reinterpret_cast< T * >(
			reinterpret_cast< uaddr >( x ) & ~(n - 1) );
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine
