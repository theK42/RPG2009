
#pragma once

#include <hash_map>
#include "engine/tools/stl/xstlalloc.h"

namespace XEngine
{

	namespace XTools
	{

		template< typename TKey, typename TValue, typename THashCompare = stdext::hash_compare< TKey, std::less< TKey > > >
		class XSTLHashMap : public stdext::hash_map< TKey, TValue, THashCompare, XSTLAllocator< std::pair< TKey const, TValue > > >
		{
		};

	} /// namespace XTools

} /// namespace XEngine
