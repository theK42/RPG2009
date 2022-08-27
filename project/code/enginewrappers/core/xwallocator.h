#pragma once
#pragma unmanaged
#include "engine/core/xcore.h"
#include "engine/core/xallocator.h"
#pragma managed 

using namespace XEngine;

#pragma make_public(XEngine::XAllocator)

namespace XWEngine
{
	class XWDefaultAllocator : public XAllocator
	{
	public:
		XWDefaultAllocator()
		{
			Register( (void *)0, (void *)~0 );
		}

		virtual ~XWDefaultAllocator()
		{
			Unregister();
		}

		virtual void *	Allocate(
			uaddr nSize,
			uaddr nAlignment,
			uaddr nAlignmentOffset,
			char const * pFile,
			int nLine )
		{
			return _aligned_offset_malloc( nSize, nAlignment, nAlignmentOffset );
		}

		virtual void *	Reallocate(
			void * p,
			uaddr nNewSize,
			uaddr nNewAlignment,
			uaddr nAlignmentOffset,
			char const * pFile,
			int nLine )
		{
			return _aligned_offset_realloc( p, nNewSize, nNewAlignment, nAlignmentOffset );
		}

		virtual void	Free( void * p )
		{
			return _aligned_free( p );
		}
	};

	public ref class XWAllocator
	{
	public:
		XWAllocator();
		XWAllocator(XAllocator * pAllocator);
		~XWAllocator();

		XAllocator * GetAllocator();
		
	private:
		XAllocator *	m_pWrappedAllocator;
	};
}