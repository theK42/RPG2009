
#include "engine/core/xcore.h"
#include "engine/graphics/xindexbuffer.h"
#include "engine/graphics/xgraphics.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XIndexBuffer::Init(
	XGraphics * pGraphics,
	int nPrimitiveType,
	int nIndexType,
	int nFlags,
	int nNumIndices,
	void * pIndices )
{
	m_nPrimitiveType	= nPrimitiveType;
	m_nIndexType		= nIndexType;
	m_nFlags			= nFlags;
	m_nNumIndices		= nNumIndices;
	m_pIndices			= (nFlags & XBufferFlags::Discardable) ? nullptr : pIndices;
	m_nId				= pGraphics->CreateIndexBufferObject( nIndexType, nNumIndices, pIndices, nFlags );
}

///----------------------------------------------------------------------------

void XEngine::XIndexBuffer::Deinit( XGraphics * pGraphics )
{
	pGraphics->DestroyIndexBufferObject( m_nId );
}

///----------------------------------------------------------------------------

void XEngine::XIndexBuffer::Draw( XGraphics * pGraphics ) const
{
	void * pPointerOrOffset = (m_nId == 0) ? m_pIndices : 0;

	pGraphics->DrawElements(
		m_nIndexType,
		m_nPrimitiveType,
		m_nNumIndices,
		m_nId,
		pPointerOrOffset );
}


///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
