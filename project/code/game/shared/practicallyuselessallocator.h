#pragma once
#include "engine/core/xallocator.h"

class CPracticallyUselessAllocator : public XEngine::XAllocator
{
public:
	CPracticallyUselessAllocator()
	{
		Register( (void *)0, (void *)~0 );
	}

	virtual ~CPracticallyUselessAllocator()
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