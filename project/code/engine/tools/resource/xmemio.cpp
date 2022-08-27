
#include "engine/core/xcore.h"
#include "engine/tools/resource/xmemio.h"
#include "engine/algorithms/xutil.h"

#include <cstring>
#include <malloc.h>

XEngine::XTools::XMemIO::XMemIO( XMemIO const & rOther )
{
	m_pBuffer		= rOther.m_pBuffer;
	m_nPosition		= rOther.m_nPosition;
	m_nAlignment	= rOther.m_nAlignment;

	AddReference( m_pBuffer );
}

XEngine::XTools::XMemIO::~XMemIO()
{
	RemoveReference( m_pBuffer );
}

XEngine::XTools::XMemIO & XEngine::XTools::XMemIO::operator =( XMemIO const & rOther )
{
	Buffer * pBuffer = m_pBuffer;

	m_pBuffer		= rOther.m_pBuffer;
	m_nPosition		= rOther.m_nPosition;
	m_nAlignment	= rOther.m_nAlignment;

	AddReference( m_pBuffer );
	RemoveReference( pBuffer );

	return (*this);
}

void XEngine::XTools::XMemIO::AddReference( Buffer * pBuffer )
{
	if( pBuffer )
	{
		++pBuffer->m_nReferenceCount;
	}
}

void XEngine::XTools::XMemIO::RemoveReference( Buffer * pBuffer )
{
	if( (pBuffer != nullptr) && (--pBuffer->m_nReferenceCount == 0) )
	{
		_aligned_free( pBuffer->m_pMemory );
		free( pBuffer );
	}
}

XEngine::XTools::XMemIO::Buffer * XEngine::XTools::XMemIO::CreateBuffer( uaddr nCapacity, uaddr nAlignment )
{
	nCapacity = Max( nCapacity, INITIAL_BUFFER_SIZE );

	Buffer * pBuffer			= (Buffer *)malloc( sizeof( Buffer ) );
	pBuffer->m_nCapacity		= nCapacity;
	pBuffer->m_nAlignment		= nAlignment;
	pBuffer->m_nSize			= 0;
	pBuffer->m_pMemory			= _aligned_malloc( nCapacity, nAlignment );
	pBuffer->m_nReferenceCount	= 1;

	return pBuffer;
}

void XEngine::XTools::XMemIO::Reserve( uaddr nSize )
{
	if( m_pBuffer == nullptr )
	{
		m_pBuffer = CreateBuffer( nSize, m_nAlignment );
	}
	else if( m_pBuffer->m_nCapacity < nSize )
	{
		m_pBuffer->m_nCapacity	+= m_pBuffer->m_nCapacity >> 1;
		m_pBuffer->m_nCapacity	= Max( m_pBuffer->m_nCapacity, nSize );
		m_pBuffer->m_pMemory	= _aligned_realloc( m_pBuffer->m_pMemory, m_pBuffer->m_nCapacity, m_pBuffer->m_nAlignment );
	}
	XASSERT( (m_pBuffer != nullptr) && (m_pBuffer->m_nCapacity >= nSize) );
}

void XEngine::XTools::XMemIO::SetAlignment( uaddr nAlignment )
{
	m_nAlignment = nAlignment;

	if( (m_pBuffer != nullptr) && (m_pBuffer->m_nAlignment < nAlignment) )
	{
		m_pBuffer->m_nAlignment	= nAlignment;
		m_pBuffer->m_pMemory	= _aligned_realloc( m_pBuffer->m_pMemory, m_pBuffer->m_nCapacity, m_pBuffer->m_nAlignment );
	}
}

XEngine::uaddr XEngine::XTools::XMemIO::Read( uaddr nSize, void * pBuffer )
{
	if( m_pBuffer == nullptr )
	{
		return 0;
	}

	uaddr nBegin = Min( m_nPosition, m_pBuffer->m_nSize );
	uaddr nEnd = Min( m_nPosition + nSize, m_pBuffer->m_nSize );

	XASSERT( nEnd >= nBegin );
	nSize = nEnd - nBegin;

	std::memcpy( pBuffer, (void *)(m_pBuffer->m_nMemoryAddress + nBegin), nSize );
	m_nPosition = nEnd;

	return nSize;
}

///----------------------------------------------------------------------------

XEngine::uaddr XEngine::XTools::XMemIO::Write( uaddr nSize, void const * pBuffer )
{
	Reserve( m_nPosition + nSize );

	uaddr nBegin = m_nPosition;
	uaddr nEnd = m_nPosition + nSize;

	XASSERT( nEnd >= nBegin );

	std::memcpy( (void *)(m_pBuffer->m_nMemoryAddress + nBegin), pBuffer, nSize );
	m_nPosition = nEnd;

	m_pBuffer->m_nSize = Max( m_nPosition, m_pBuffer->m_nSize );

	return nSize;
}

///----------------------------------------------------------------------------

void XEngine::XTools::XMemIO::Seek( s64 nOffset, int nOrigin )
{
	saddr nRetypedOffset = static_cast< saddr >( nOffset );

	switch( nOrigin )
	{
	case XSeekOrigin::Begin:
		XASSERT( nRetypedOffset >= 0 );
		m_nPosition = nRetypedOffset;
		break;

	case XSeekOrigin::End:
		m_nPosition = GetDataSize();
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

XEngine::u64 XEngine::XTools::XMemIO::Tell() const
{
	return static_cast< u64 >( m_nPosition );
}

///----------------------------------------------------------------------------

XEngine::u64 XEngine::XTools::XMemIO::GetSize()
{
	return static_cast< u64 >( GetDataSize() );
}
