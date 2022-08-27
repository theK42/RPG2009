
#include "engine/core/xcore.h"
#include "engine/graphics/xvertexbuffer.h"
#include "engine/graphics/xgraphics.h"

void XEngine::XVertexBuffer::Init(
	XGraphics * pGraphics,
	XVertexFormat const & rFormat,
	int nFlags,
	int nNumVertices,
	void * pVertices )
{
	m_oFormat		= rFormat;
	m_nFlags		= nFlags;
	m_nNumVertices	= nNumVertices;
	m_pVertices		= (nFlags & XBufferFlags::Discardable) ? nullptr : pVertices;
	m_nId			= pGraphics->CreateVertexBufferObject( rFormat, nNumVertices, pVertices, nFlags );
}

///----------------------------------------------------------------------------

void XEngine::XVertexBuffer::Deinit( XGraphics * pGraphics )
{
	pGraphics->DestroyVertexBufferObject( m_nId );
}
