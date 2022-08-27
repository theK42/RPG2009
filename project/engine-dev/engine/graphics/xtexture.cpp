
#include "engine/core/xcore.h"
#include "engine/graphics/xtexture.h"
#include "engine/graphics/xgraphics.h"

void XEngine::XTexture::Init(
	XGraphics * pGraphics,
	int nFormat,
	int nWidth,
	int nHeight, 
	int nFlags,
	void * pTexels )
{
	m_nFormat	= nFormat;
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;
	m_nFlags	= nFlags;
	m_nId		= pGraphics->CreateTextureObject( nFormat, nWidth, nHeight, pTexels, nFlags );
	m_pTexels	= (nFlags & XTextureFlags::Discardable) ? nullptr : pTexels;
}

void XEngine::XTexture::Deinit( XGraphics * pGraphics )
{
	pGraphics->DestroyTextureObject( m_nId );
}

void XEngine::XTexture::Select( XGraphics * pGraphics, int nTextureIndex ) const
{
	pGraphics->SetTexture( nTextureIndex, m_nId );
}
