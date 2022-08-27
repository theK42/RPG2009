
#pragma once

#include <string>
#include "engine/tools/stl/xstlalloc.h"

namespace XEngine
{

	namespace XTools
	{

		template< typename T, typename Traits = std::char_traits< T > >
		class XSTLStringBase : public std::basic_string< T, Traits, XSTLAllocator< T > >
		{
		private:
			typedef std::basic_string< T, Traits, XSTLAllocator< T > >	BaseClass;
		public:
			XSTLStringBase() {}
			XSTLStringBase( BaseClass const & s, size_type pos = 0, size_type n = npos ) : BaseClass( s, pos, npos ) {}
			XSTLStringBase( T const * s ) : BaseClass( s ) {}
			XSTLStringBase( T const * s, size_type n ) : BaseClass( s, n ) {}
			XSTLStringBase( size_type n, T c ) : BaseClass( n, c ) {}

			template< typename InputIterator >
			XSTLStringBase( InputIterator first, InputIterator last ) : BaseClass( first, last ) {}
		};

		typedef XSTLStringBase< char >		XSTLString;
		typedef XSTLStringBase< wchar_t >	XSTLWString;

	} /// namespace XTools

} /// namespace XEngine
