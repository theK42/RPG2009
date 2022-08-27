
#pragma once

#include <vector>
#include "engine/tools/stl/xstlalloc.h"

namespace XEngine
{

	namespace XTools
	{

		template< typename T >
		class XSTLVector : public std::vector< T, XSTLAllocator< T > >
		{
		};

	} /// namespace XTools

} /// namespace XEngine
