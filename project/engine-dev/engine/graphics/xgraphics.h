
#pragma once

#include "engine/graphics/xrenderstate.h"
#include "engine/graphics/xvertexformat.h"

namespace XEngine
{
	class XColor;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XBufferFlags
	{
		enum Type
		{
			StaticDraw	= 0x01,
			DynamicDraw	= 0x02,
							
			UsageMask	= 0x03,

			Discardable	= 0x10,
			DoNotDraw	= 0x20,
		};
	};

	///------------------------------------------------------------------------

	struct XScreenClearTarget
	{
		enum
		{
			Color	= 0x1,
			Depth	= 0x2,
			Stencil	= 0x4,

			Mask	= Color | Depth | Stencil
		};
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XGraphics
	{
	public:
		void	Init();
		void	Deinit();

		void	PushRenderState();
		void	PopRenderState();

		void	SetClearColor( XColor const & rColor );
		void	ClearScreen( int nClearTargetMask );

		void	SetViewport( int x, int y, int w, int h );
		void	SetProjectionMatrix( XMatrix44 const & rMatrix );
		void	SetViewMatrix( XMatrix44 const & rMatrix );
		void	SetModelMatrix( XMatrix44 const & rMatrix );
		void	SetTextureMatrix( int nTextureIndex, XMatrix44 const & rMatrix );
		void	SetPaletteMatrix( int nIndex, XMatrix44 const & rMatrix );

		void	SetColor( XColor const & rColor );
		void	EnableFaceCulling( bool bEnable );
		void	EnableDepthTest( bool bEnable );

		void	SetTexture( int nTextureIndex, int nId );
		void	EnableTexture( int nTextureIndex, bool bEnable );

		void	SetMaterial(
					XColor const & rAmbientColor,
					XColor const & rDiffuseColor,
					XColor const & rSpecularColor,
					XColor const & rEmissiveColor,
					float fSpecularExponent );
		void	SetLight(
					int nIndex,
					XVector4 const & rPosition,
					XColor const & rAmbientColor,
					XColor const & rDiffuseColor,
					XColor const & rSpecularColor,
					XVector4 const & rAttenuation );
		void	EnableLight( int nIndex, bool bEnable );
		void	EnableLighting( bool bEnable );

		void	SetVertexAttributePointer(
					int eAttributeId,
					int eType,
					int nNumComponents,
					int nVertexStride,
					int nVertexBufferBinding,
					void const * pPointerOrOffset );

		void	DrawElements(
					int eIndexType,
					int ePrimitiveType,
					int nNumIndices,
					int nIndexBufferBinding,
					void const * pPointerOrOffset );

		int		CreateIndexBufferObject( int nIndexType, int nNumIndices, void const * pBufferData, int nFlags );
		void	DestroyIndexBufferObject( int nId );

		int		CreateVertexBufferObject( XVertexFormat const & rFormat, int nNumVertices, void const * pBufferData, int nFlags );
		void	DestroyVertexBufferObject( int nId );

		int		CreateTextureObject( int nFormat, int nWidth, int nHeight, void const * pTexels, int nFlags );
		void	DestroyTextureObject( int nId );

	private:
		void	ApplyDrawingStates();
		void	ApplyVertexArrayStates();

	private:
		static int const MAX_STACK_DEPTH = 4;

		enum
		{
			FLAG_VIEWPORTPROJ			= 0x01,
			FLAG_MODELVIEW				= 0x02,
			FLAG_MATRIX_PALETTE			= 0x04,
			FLAG_TEXTURE				= 0x08,
			FLAG_VERTEX_ATTRIB			= 0x10,
			FLAG_MATERIAL				= 0x20,
			FLAG_LIGHTS					= 0x40,
		};

		XRenderState			m_oRenderState;
		XDriverRenderState		m_oDriverRenderState;

		XRenderState			m_pSavedRenderStates[ MAX_STACK_DEPTH ];
		int						m_nStackDepth;
		int						m_nChangeFlags;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline void XEngine::XGraphics::PushRenderState()
{
	XASSERT( m_nStackDepth < MAX_STACK_DEPTH );
	m_pSavedRenderStates[ m_nStackDepth++ ] = m_oRenderState;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::PopRenderState()
{
	XASSERT( m_nStackDepth > 0 );
	m_oRenderState = m_pSavedRenderStates[ --m_nStackDepth ];
	m_nChangeFlags = ~0;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::SetViewport( int x, int y, int w, int h )
{
	int xy = (x & 0xFFFF) | (y << 16);
	int wh = (w & 0xFFFF) | (h << 16);

	m_oRenderState.SetInt( XRenderStateEntry::ViewportXY, xy );
	m_oRenderState.SetInt( XRenderStateEntry::ViewportWH, wh );
	m_nChangeFlags |= FLAG_VIEWPORTPROJ;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::SetProjectionMatrix( XMatrix44 const & rMatrix )
{
	m_oRenderState.SetMatrix( XRenderStateMatrixEntry::Projection, rMatrix );
	m_nChangeFlags |= FLAG_VIEWPORTPROJ;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::SetViewMatrix( XMatrix44 const & rMatrix )
{
	m_oRenderState.SetMatrix( XRenderStateMatrixEntry::View, rMatrix );
	m_nChangeFlags |= FLAG_MODELVIEW | FLAG_LIGHTS;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::SetModelMatrix( XMatrix44 const & rMatrix )
{
	m_oRenderState.SetMatrix( XRenderStateMatrixEntry::Model, rMatrix );
	m_nChangeFlags |= FLAG_MODELVIEW;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::SetTextureMatrix( int nTextureIndex, XMatrix44 const & rMatrix )
{
	m_oRenderState.SetMatrix( XRenderStateMatrixEntry::Texture0 + nTextureIndex, rMatrix );
	m_nChangeFlags |= FLAG_TEXTURE;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::SetPaletteMatrix( int nIndex, XMatrix44 const & rMatrix )
{
	m_oRenderState.SetMatrix( XRenderStateMatrixEntry::MatrixPalette0 + nIndex, rMatrix );
	m_nChangeFlags |= FLAG_MATRIX_PALETTE;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::EnableFaceCulling( bool bEnable )
{
	m_oRenderState.SetInt( XRenderStateEntry::FaceCulling, bEnable );
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::EnableDepthTest( bool bEnable )
{
	m_oRenderState.SetInt( XRenderStateEntry::DepthTest, bEnable );
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::EnableTexture( int nTextureIndex, bool bEnable )
{
	int nEntry = nTextureIndex + XRenderStateEntry::Texture0;
	m_oRenderState.SetInt( (XRenderStateEntry::Type)nEntry, bEnable );
	m_nChangeFlags |= FLAG_TEXTURE;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::EnableLight( int nIndex, bool bEnable )
{
	m_oRenderState.GetLightState( nIndex ).m_bEnabled = true;
	m_nChangeFlags |= FLAG_LIGHTS;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::EnableLighting( bool bEnable )
{
	m_oRenderState.SetInt( XRenderStateEntry::Lighting, bEnable );
	m_nChangeFlags |= FLAG_LIGHTS;
}

///----------------------------------------------------------------------------

inline void XEngine::XGraphics::SetVertexAttributePointer(
	int eAttributeId,
	int eType,
	int nNumComponents,
	int nVertexStride,
	int nVertexBufferBinding,
	void const * pPointerOrOffset )
{
	XASSERT( (eAttributeId >= 0) && (eAttributeId < XVertexAttributeId::Max) );
	XVertexAttributeArrayState & rArrayState = m_oRenderState.GetVertexAttributeArrayState( eAttributeId );

	rArrayState.m_eType					= eType;
	rArrayState.m_nNumComponents		= nNumComponents;
	rArrayState.m_nVertexStride			= nVertexStride;
	rArrayState.m_nVertexBufferBinding	= nVertexBufferBinding;
	rArrayState.m_pPointerOrOffset		= pPointerOrOffset;
	rArrayState.m_bEnabled				= true;

	m_nChangeFlags |= FLAG_VERTEX_ATTRIB;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
