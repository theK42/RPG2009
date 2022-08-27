
#include "engine/core/xcore.h"
#include "engine/resource/xfixedmemio.h"
#include "engine/algorithms/xutil.h"

#include <cstring>

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XEngine::uaddr XEngine::XFixedMemIO::Read( uaddr nSize, void * pBuffer )
{
	uaddr nBegin = Min( m_nPosition, m_nSize );
	uaddr nEnd = Min( m_nPosition + nSize, m_nSize );

	XASSERT( nEnd >= nBegin );
	nSize = nEnd - nBegin;

	std::memcpy( pBuffer, (void *)(m_nMemoryAddress + nBegin), nSize );
	m_nPosition = nEnd;

	return nSize;
}

///----------------------------------------------------------------------------

XEngine::uaddr XEngine::XFixedMemIO::Write( uaddr nSize, void const * pBuffer )
{
	uaddr nBegin = Min( m_nPosition, m_nSize );
	uaddr nEnd = Min( m_nPosition + nSize, m_nSize );

	XASSERT( nEnd >= nBegin );
	nSize = nEnd - nBegin;

	std::memcpy( (void *)(m_nMemoryAddress + nBegin), pBuffer, nSize );
	m_nPosition = nEnd;

	return nSize;
}

///----------------------------------------------------------------------------

void XEngine::XFixedMemIO::Seek( s64 nOffset, int nOrigin )
{
	saddr nRetypedOffset = static_cast< saddr >( nOffset );

	switch( nOrigin )
	{
	case XSeekOrigin::Begin:
		XASSERT( nRetypedOffset >= 0 );
		m_nPosition = nRetypedOffset;
		break;

	case XSeekOrigin::End:
		m_nPosition = m_nSize;
		/// Fall through...

	case XSeekOrigin::Current:
		XASSERT( ((nRetypedOffset >= 0) && (m_nPosition + nRetypedOffset >= m_nPosition)) || ((nRetypedOffset < 0) && (m_nPosition + nRetypedOffset < m_nPosition)) );
		m_nPosition += nRetypedOffset;
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

XEngine::u64 XEngine::XFixedMemIO::Tell() const
{
	return static_cast< u64 >( m_nPosition );
}

///----------------------------------------------------------------------------

XEngine::u64 XEngine::XFixedMemIO::GetSize()
{
	return static_cast< u64 >( m_nSize );
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
