
#pragma once

#include <malloc.h>

namespace XEngine
{

	namespace XTools
	{

		template< typename T >
		class XSTLAllocator
		{
		public:
			XSTLAllocator();
			XSTLAllocator( XSTLAllocator const & rOther );

			template< typename TOther >
			XSTLAllocator( XSTLAllocator<TOther> const & rOther );

			template< typename TOther >
			XSTLAllocator<T> & operator =( XSTLAllocator<TOther> const & rOther );

			template< typename TOther >
			bool operator ==( XSTLAllocator<TOther> const & rOther ) const;

		public:
			typedef T			value_type;
			typedef T *			pointer;
			typedef T &			reference;
			typedef T const *	const_pointer;
			typedef T const &	const_reference;
			typedef uaddr		size_type;
			typedef saddr		difference_type;

			template< typename TOther >
			struct rebind
			{
				typedef XSTLAllocator<TOther>	other;
			};

		public:
			pointer			address( reference r ) const;
			const_pointer	address( const_reference r ) const;
			void			deallocate( pointer p, size_type nSize );
			pointer			allocate( size_type nCount );
			pointer			allocate( size_type nCount, const void * pHint );
			void			construct( pointer p, const_reference rValue );
			void			destroy( pointer p );
			size_type		max_size() const;
		};

	} /// namespace XTools

} /// namespace XEngine

template< typename T >
inline XEngine::XTools::XSTLAllocator<T>::XSTLAllocator()
{
}

template< typename T >
inline XEngine::XTools::XSTLAllocator<T>::XSTLAllocator( XSTLAllocator<T> const & rOther )
{
}

template< typename T > template< typename TOther >
inline XEngine::XTools::XSTLAllocator<T>::XSTLAllocator( XSTLAllocator<TOther> const & rOther )
{
}

template< typename T > template< typename TOther >
inline XEngine::XTools::XSTLAllocator<T> & XEngine::XTools::XSTLAllocator<T>::operator =( XSTLAllocator<TOther> const & rOther )
{
	return (*this);
}

template< typename T > template< typename TOther >
inline bool XEngine::XTools::XSTLAllocator<T>::operator ==( XSTLAllocator<TOther> const & rOther ) const
{
	return true;
}

template< typename T >
inline typename XEngine::XTools::XSTLAllocator<T>::pointer XEngine::XTools::XSTLAllocator<T>::address( reference r ) const
{
	return (&r);
}

template< typename T >
inline typename XEngine::XTools::XSTLAllocator<T>::const_pointer XEngine::XTools::XSTLAllocator<T>::address( const_reference r ) const
{
	return (&r);
}

template< typename T >
inline void XEngine::XTools::XSTLAllocator<T>::deallocate( pointer p, size_type nSize )
{
	_aligned_free( p );
}

template< typename T >
inline typename XEngine::XTools::XSTLAllocator<T>::pointer XEngine::XTools::XSTLAllocator<T>::allocate( size_type nCount )
{
	return (pointer) _aligned_malloc( nCount * sizeof( value_type ), alignof( value_type ) );
}

template< typename T >
inline typename XEngine::XTools::XSTLAllocator<T>::pointer XEngine::XTools::XSTLAllocator<T>::allocate( size_type nCount, void const * pHint )
{
	return (pointer) _aligned_malloc( nCount * sizeof( value_type ), alignof( value_type ) );
}

template< typename T >
inline void XEngine::XTools::XSTLAllocator<T>::construct( pointer p, const_reference rValue )
{
	new( p ) T( rValue );
}

template< typename T >
inline void XEngine::XTools::XSTLAllocator<T>::destroy( pointer p )
{
	p->~T();
}

template< typename T >
inline typename XEngine::XTools::XSTLAllocator<T>::size_type XEngine::XTools::XSTLAllocator<T>::max_size() const
{
	size_type nCount = (size_type)(-1) / sizeof( value_type );
	return (0 < nCount) ? nCount : 1;
}
