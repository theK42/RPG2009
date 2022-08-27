#include "stdafx.h"
#include "xwallocator.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWAllocator::XWAllocator()
{
	m_pWrappedAllocator = new XWDefaultAllocator();
}

///----------------------------------------------------------------------------

XWEngine::XWAllocator::XWAllocator( XAllocator * pAllocator )
{
	m_pWrappedAllocator = pAllocator;
}

///----------------------------------------------------------------------------

XWEngine::XWAllocator::~XWAllocator()
{
	XASSERT_PTR(m_pWrappedAllocator);
	delete m_pWrappedAllocator;
	m_pWrappedAllocator = nullptr;
}

///----------------------------------------------------------------------------

XAllocator * XWEngine::XWAllocator::GetAllocator()
{
	return m_pWrappedAllocator;
}

///----------------------------------------------------------------------------