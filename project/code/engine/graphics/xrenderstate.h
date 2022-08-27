
#pragma once

#include "engine/graphics/xvertexformat.h"
#include "engine/math/xmatrix.h"

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XRenderStateEntry
	{
		enum Type
		{
			ClearColor,
			Color,
			FaceCulling,
			DepthTest,
			Lighting,
			MaterialEmissiveColor,
			MaterialAmbientColor,
			MaterialDiffuseColor,
			MaterialSpecularColor,
			MaterialSpecularExponent,
			MatrixPalette,
			Texture0,
			Texture1,
			Texture0Binding,
			Texture1Binding,
			ViewportXY,
			ViewportWH,

			Max
		};
	};

	///------------------------------------------------------------------------

	struct XRenderStateMatrixEntry
	{
		enum Type
		{
			Projection,
			View,
			Model,
			Texture0,
			Texture1,
			BaseEnd,

			MatrixPaletteBegin	= BaseEnd,
			MatrixPalette0		= MatrixPaletteBegin,
			MatrixPalette1,
			MatrixPalette2,
			MatrixPalette3,
			MatrixPalette4,
			MatrixPalette5,
			MatrixPalette6,
			MatrixPalette7,
			MatrixPalette8,
			MatrixPaletteEnd,

			Max					= MatrixPaletteEnd,

			/// On the app side we have View and Model separate
			/// On the driver side we have them combined
			ModelView			= Model,
		};
	};

	///------------------------------------------------------------------------

	struct XVertexAttributeArrayState
	{
		int				m_eType;
		int				m_nNumComponents;
		int				m_nVertexStride;
		int				m_nVertexBufferBinding;
		void const *	m_pPointerOrOffset;
		bool			m_bEnabled;
	};

	///------------------------------------------------------------------------

	struct XLightState
	{
		XVector4	m_vPosition;
		u32			m_nAmbientColor;
		u32			m_nDiffuseColor;
		u32			m_nSpecularColor;
		float		m_fConstantAttenuation;
		float		m_fLinearAttenuation;
		float		m_fQuadraticAttenuation;
		bool		m_bEnabled;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XRenderState
	{
	public:
		int		GetInt( XRenderStateEntry::Type eEntry ) const;
		float	GetFloat( XRenderStateEntry::Type eEntry ) const;

		void	SetInt( XRenderStateEntry::Type eEntry, int nValue );
		void	SetFloat( XRenderStateEntry::Type eEntry, float fValue );

		XMatrix44 const &	GetMatrix( int nEntry ) const;
		XMatrix44 &			GetMatrix( int nEntry );
		void				SetMatrix( int nEntry, XMatrix44 const & rMatix );

		XVertexAttributeArrayState const &	GetVertexAttributeArrayState( int eAttributeId ) const;
		XVertexAttributeArrayState &		GetVertexAttributeArrayState( int eAttributeId );
		void								SetVertexAttributeArrayState( int eAttributeId, XVertexAttributeArrayState const & rState );

		static int const	MAX_LIGHTS = 8;
		XLightState const &	GetLightState( int nIndex ) const;
		XLightState &		GetLightState( int nIndex );
		void				SetLightState( int nIndex, XLightState const & rState );

	protected:
		union RenderStateValue
		{
			s32		m_nValue;
			float	m_fValue;
		};

	private:
		XMatrix44					m_pMatrices[ XRenderStateMatrixEntry::Max ];
		XVertexAttributeArrayState	m_pVertexAttributeArrays[ XVertexAttributeId::Max ];
		XLightState					m_pLights[ MAX_LIGHTS ];
		RenderStateValue			m_pRenderState[ XRenderStateEntry::Max ];
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XDriverRenderStateEntry
	{
		enum Type
		{
			VertexBufferBinding,
			IndexBufferBinding,
			ClientActiveTexture,
			ActiveTexture,
			MatrixMode,
			CurrentPaletteMatrix,

			Max
		};
	};

	///------------------------------------------------------------------------

	class XDriverRenderState : public XRenderState
	{
	public:
		int		GetDriverInt( XDriverRenderStateEntry::Type eEntry ) const;
		float	GetDriverFloat( XDriverRenderStateEntry::Type eEntry ) const;

		void	SetDriverInt( XDriverRenderStateEntry::Type eEntry, int nValue );
		void	SetDriverFloat( XDriverRenderStateEntry::Type eEntry, float fValue );

	private:
		RenderStateValue	m_pDriverRenderState[ XDriverRenderStateEntry::Max ];
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline int XEngine::XRenderState::GetInt( XRenderStateEntry::Type eEntry ) const
{
	return m_pRenderState[ eEntry ].m_nValue;
}

///----------------------------------------------------------------------------

inline float XEngine::XRenderState::GetFloat( XRenderStateEntry::Type eEntry ) const
{
	return m_pRenderState[ eEntry ].m_fValue;
}

///----------------------------------------------------------------------------

inline void XEngine::XRenderState::SetInt( XRenderStateEntry::Type eEntry, int nValue )
{
	m_pRenderState[ eEntry ].m_nValue = nValue;
}

///----------------------------------------------------------------------------

inline void XEngine::XRenderState::SetFloat( XRenderStateEntry::Type eEntry, float fValue )
{
	m_pRenderState[ eEntry ].m_fValue = fValue;
}

///----------------------------------------------------------------------------

inline XEngine::XVertexAttributeArrayState const & XEngine::XRenderState::GetVertexAttributeArrayState( int eAttributeId ) const
{
	return m_pVertexAttributeArrays[ eAttributeId ];
}

///----------------------------------------------------------------------------

inline XEngine::XVertexAttributeArrayState & XEngine::XRenderState::GetVertexAttributeArrayState( int eAttributeId )
{
	return m_pVertexAttributeArrays[ eAttributeId ];
}

///----------------------------------------------------------------------------

inline void XEngine::XRenderState::SetVertexAttributeArrayState( int eAttributeId, XEngine::XVertexAttributeArrayState const & rState )
{
	m_pVertexAttributeArrays[ eAttributeId ] = rState;
}

///----------------------------------------------------------------------------

inline XEngine::XLightState const & XEngine::XRenderState::GetLightState( int nIndex ) const
{
	return m_pLights[ nIndex ];
}

///----------------------------------------------------------------------------

inline XEngine::XLightState & XEngine::XRenderState::GetLightState( int nIndex )
{
	return m_pLights[ nIndex ];
}

///----------------------------------------------------------------------------

inline void XEngine::XRenderState::SetLightState( int nIndex, XLightState const & rLight )
{
	m_pLights[ nIndex ] = rLight;
}

///----------------------------------------------------------------------------

inline XEngine::XMatrix44 const & XEngine::XRenderState::GetMatrix( int nEntry ) const
{
	return m_pMatrices[ nEntry ];
}

///----------------------------------------------------------------------------

inline XEngine::XMatrix44 & XEngine::XRenderState::GetMatrix( int nEntry )
{
	return m_pMatrices[ nEntry ];
}

///----------------------------------------------------------------------------

inline void XEngine::XRenderState::SetMatrix( int nEntry, XMatrix44 const & rMatrix )
{
	m_pMatrices[ nEntry ] = rMatrix;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline int XEngine::XDriverRenderState::GetDriverInt( XDriverRenderStateEntry::Type eEntry ) const
{
	return m_pDriverRenderState[ eEntry ].m_nValue;
}

///----------------------------------------------------------------------------

inline float XEngine::XDriverRenderState::GetDriverFloat( XDriverRenderStateEntry::Type eEntry ) const
{
	return m_pDriverRenderState[ eEntry ].m_fValue;
}

///----------------------------------------------------------------------------

inline void XEngine::XDriverRenderState::SetDriverInt( XDriverRenderStateEntry::Type eEntry, int nValue )
{
	m_pDriverRenderState[ eEntry ].m_nValue = nValue;
}

///----------------------------------------------------------------------------

inline void XEngine::XDriverRenderState::SetDriverFloat( XDriverRenderStateEntry::Type eEntry, float fValue )
{
	m_pDriverRenderState[ eEntry ].m_fValue = fValue;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
