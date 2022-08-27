
#include "engine/core/xcore.h"
#include "engine/core/xallocator.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XEngine::XAllocatorRegistry XEngine::XAllocatorRegistry::ms_oInstance;

///----------------------------------------------------------------------------

void XEngine::XAllocatorRegistry::RegisterAllocator( XEngine::XAllocator * pAllocator, void * pRangeBegin, void * pRangeEnd )
{
	XASSERT( m_nNumRecords < MAX_RECORDS );
	XASSERT( pRangeBegin <= pRangeEnd );

	int i;
	for( i = m_nNumRecords - 1; i >= 0; i-- )
	{
		XASSERT( (m_pRecords[i].m_pRangeBegin != pRangeBegin) || (m_pRecords[i].m_pRangeEnd != pRangeEnd) );

		if( (m_pRecords[i].m_pRangeBegin > pRangeEnd) ||
			((m_pRecords[i].m_pRangeBegin >= pRangeBegin) && (m_pRecords[i].m_pRangeEnd <= pRangeEnd)))
		{
			m_pRecords[i+1] = m_pRecords[i];
			continue;
		}

		XASSERT( (m_pRecords[i].m_pRangeEnd <= pRangeBegin) ||
			((m_pRecords[i].m_pRangeBegin <= pRangeBegin) && (m_pRecords[i].m_pRangeEnd >= pRangeEnd)) );

		break;
	}

	m_pRecords[i+1].m_pAllocator	= pAllocator;
	m_pRecords[i+1].m_pRangeBegin	= pRangeBegin;
	m_pRecords[i+1].m_pRangeEnd		= pRangeEnd;
	++m_nNumRecords;
}

///----------------------------------------------------------------------------

void XEngine::XAllocatorRegistry::UnregisterAllocator( XEngine::XAllocator const * pAllocator )
{
	int nShift = 0;
	for( int i = 0; i < m_nNumRecords; ++i )
	{
		if( m_pRecords[i].m_pAllocator == pAllocator )
		{
			++nShift;
			void * pRangeEnd = m_pRecords[i].m_pRangeEnd;
			for( ++i; i < m_nNumRecords; ++i )
			{
				if( m_pRecords[i].m_pRangeBegin >= pRangeEnd )
				{
					break;
				}
				++nShift;
			}
			continue;
		}

		m_pRecords[i - nShift] = m_pRecords[i];
	}

	m_nNumRecords -= nShift;
}

///----------------------------------------------------------------------------

XEngine::XAllocator * XEngine::XAllocatorRegistry::FindAllocator( void const * p ) const
{
	Record const * pLo = m_pRecords;
	Record const * pHi = m_pRecords + m_nNumRecords;
	Record const * pMid;

	while( pLo < pHi )
	{
		pMid = pLo + ((pHi - pLo) >> 1);

		if( p >= pMid->m_pRangeEnd )
		{
			pLo = pMid + 1;
		}
		else if( p < pMid->m_pRangeBegin )
		{
			pHi = pMid;
		}
		else
		{
			break;
		}
	}

	if( XUNLIKELY( pLo == pHi ) )
	{
		return nullptr;
	}

	while( pMid + 1 < pHi )
	{
		if( (pMid[1].m_pRangeBegin <= p) || (pMid[1].m_pRangeEnd > p) )
		{
			++pMid;
			continue;
		}
		break;
	}

	return pMid->m_pAllocator;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void * XEngine::XAllocator::Allocate( XEngine::uaddr nSize, XEngine::uaddr nAlignment, XEngine::uaddr nAlignmentOffset, char const * pFile, int nLine )
{
	return nullptr;
}

///----------------------------------------------------------------------------

void * XEngine::XAllocator::Reallocate( void * p, XEngine::uaddr nNewSize, XEngine::uaddr nNewAlignment, XEngine::uaddr nAlignmentOffset, char const * pFile, int nLine )
{
	return nullptr;
}

///----------------------------------------------------------------------------

void XEngine::XAllocator::Free( void * p )
{
	return;
}

///----------------------------------------------------------------------------

void XEngine::XAllocator::InitModule()
{
	XTLS_INIT( XEngine::g_pDeleteAllocatorStack );
}

///----------------------------------------------------------------------------

void XEngine::XAllocator::DeinitModule()
{
	XTLS_DEINIT( XEngine::g_pDeleteAllocatorStack );
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XTLS_DECLARE( XEngine::XAllocatorStackEntry const * ) XEngine::g_pDeleteAllocatorStack;

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
