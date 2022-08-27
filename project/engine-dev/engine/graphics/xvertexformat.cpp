
#include "engine/core/xcore.h"
#include "engine/graphics/xvertexformat.h"
#include "engine/algorithms/xutil.h"

XEngine::uaddr gs_pTypeAlignment[ XEngine::XVertexAttributeType::Max ] =
{
#if defined( TARGET_IPHONEOS )
	1,
	1,
	2,
	2,
#else
	4,
	4,
	4,
	4,
#endif
	4,
	4,
	4,
};

XEngine::uaddr gs_pTypeSize[ XEngine::XVertexAttributeType::Max ] =
{
	1,
	1,
	2,
	2,
	4,
	4,
	4,
};

void XEngine::XVertexFormat::Add( int nAttributeId, int nType, unsigned nNumComponents, unsigned nOffset )
{
	XASSERT( m_nNumAttributes < XVertexAttributeId::Max );

	if( nOffset == 0 )
	{
		nOffset = AlignUp( m_nVertexStride, gs_pTypeAlignment[ nType ] );
	}

	Attribute & rAttribute		= m_pAttributes[ m_nNumAttributes ];
	rAttribute.m_nAttributeId	= nAttributeId;
	rAttribute.m_nType			= nType;
	rAttribute.m_nNumComponents	= nNumComponents;
	rAttribute.m_nOffset		= nOffset;

	uaddr nSize					= gs_pTypeSize[ nType ] * nNumComponents;
	uaddr nAlign				= Max( (uaddr)m_nVertexAlignment, gs_pTypeAlignment[ nType ] );
	m_nNumAttributes			++;
	m_nVertexStride				= (u16)AlignUp( nOffset + nSize, nAlign );
	m_nVertexAlignment			= (u8)nAlign;
}
