
#include "engine/core/xcore.h"
#include "engine/algorithms/xutil.h"
#include "engine/graphics/xgraphics.h"
#include "engine/graphics/xindexbuffer.h"
#include "engine/graphics/xtexture.h"
#include "engine/graphics/xcolor.h"
#include "engine/graphics/xgl.h"

#include <cstdio>
using namespace std;

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

static int const WEIGHT_ATTRIB_INDEX		= 1;
static int const MATRIX_INDEX_ATTRIB_INDEX	= 2;

///----------------------------------------------------------------------------

static void APIENTRY NormalPointer( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer )
{
	(void)size;
	glNormalPointer( type, stride, pointer );
}

///----------------------------------------------------------------------------

static void APIENTRY WeightPointer( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer )
{
	glVertexAttribPointerARB(
		WEIGHT_ATTRIB_INDEX,
		size,
		type,
		GL_TRUE,
		stride,
		pointer );
}

///----------------------------------------------------------------------------

static void APIENTRY MatrixIndexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid * pointer )
{
	glVertexAttribPointerARB(
		MATRIX_INDEX_ATTRIB_INDEX,
		size,
		type,
		GL_FALSE,
		stride,
		pointer );
}

///----------------------------------------------------------------------------

static void (APIENTRY * gs_pfnAttributePointer[ XEngine::XVertexAttributeId::Max ])( GLint, GLenum, GLsizei, const GLvoid * ) =
{
	&glVertexPointer,
	&NormalPointer,
	&glColorPointer,
	&glTexCoordPointer,
	&glTexCoordPointer,
#if defined( TARGET_OPENGLES )
	&glWeightPointerOES,
	&glMatrixIndexPointerOES,
#else
	&glWeightPointerARB,		/// Will change to &WeightPointer if !ARB_vertex_blend
	&glMatrixIndexPointerARB,	/// Will change to &MatrixIndexPointer if !ARB_matrix_palette
#endif
};

///----------------------------------------------------------------------------

static void XFASTCALL EnableVertexArray()
{
	glEnableClientState( GL_VERTEX_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL EnableNormalArray()
{
	glEnableClientState( GL_NORMAL_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL EnableColorArray()
{
	glEnableClientState( GL_COLOR_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL EnableTexCoordArray()
{
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL EnableWeightArray()
{
#if defined( TARGET_OPENGLES )
	glEnableClientState( GL_WEIGHT_ARRAY_OES );
#else
	glEnableClientState( GL_WEIGHT_ARRAY_ARB );
#endif
}

///----------------------------------------------------------------------------

static void XFASTCALL EnableMatrixIndexArray()
{
#if defined( TARGET_OPENGLES )
	glEnableClientState( GL_MATRIX_INDEX_ARRAY_OES );
#else
	glEnableClientState( GL_MATRIX_INDEX_ARRAY_ARB );
#endif
}

///----------------------------------------------------------------------------

#if !defined( TARGET_OPENGLES )

///----------------------------------------------------------------------------

static void XFASTCALL EnableWeightArray2()
{
	glEnableVertexAttribArrayARB( WEIGHT_ATTRIB_INDEX );
}

///----------------------------------------------------------------------------

static void XFASTCALL EnableMatrixIndexArray2()
{
	glEnableVertexAttribArrayARB( MATRIX_INDEX_ATTRIB_INDEX );
}

///----------------------------------------------------------------------------

#endif

///----------------------------------------------------------------------------

static void (XFASTCALL * gs_pfnEnableAttrib[ XEngine::XVertexAttributeId::Max ])() =
{
	&EnableVertexArray,
	&EnableNormalArray,
	&EnableColorArray,
	&EnableTexCoordArray,
	&EnableTexCoordArray,
	&EnableWeightArray,			/// Will change to &EnableWeightArray2 if !ARB_vertex_blend
	&EnableMatrixIndexArray,	/// Will change to &EnableMatrixIndexArray2 if !ARB_matrix_palette
};

///----------------------------------------------------------------------------

static void XFASTCALL DisableVertexArray()
{
	glDisableClientState( GL_VERTEX_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL DisableNormalArray()
{
	glDisableClientState( GL_NORMAL_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL DisableColorArray()
{
	glDisableClientState( GL_COLOR_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL DisableTexCoordArray()
{
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

///----------------------------------------------------------------------------

static void XFASTCALL DisableWeightArray()
{
#if defined( TARGET_OPENGLES )
	glDisableClientState( GL_WEIGHT_ARRAY_OES );
#else
	glDisableClientState( GL_WEIGHT_ARRAY_ARB );
#endif
}

///----------------------------------------------------------------------------

static void XFASTCALL DisableMatrixIndexArray()
{
#if defined( TARGET_OPENGLES )
	glDisableClientState( GL_MATRIX_INDEX_ARRAY_OES );
#else
	glDisableClientState( GL_MATRIX_INDEX_ARRAY_ARB );
#endif
}

///----------------------------------------------------------------------------

#if !defined( TARGET_OPENGLES )

///----------------------------------------------------------------------------

static void XFASTCALL DisableWeightArray2()
{
	glDisableVertexAttribArrayARB( WEIGHT_ATTRIB_INDEX );
}

///----------------------------------------------------------------------------

static void XFASTCALL DisableMatrixIndexArray2()
{
	glDisableVertexAttribArrayARB( MATRIX_INDEX_ATTRIB_INDEX );
}

///----------------------------------------------------------------------------

#endif

///----------------------------------------------------------------------------

static void (XFASTCALL * gs_pfnDisableAttrib[ XEngine::XVertexAttributeId::Max ])() =
{
	&DisableVertexArray,
	&DisableNormalArray,
	&DisableColorArray,
	&DisableTexCoordArray,
	&DisableTexCoordArray,
	&DisableWeightArray,			/// Will change to &DisableWeightArray2 if !ARB_vertex_blend
	&DisableMatrixIndexArray,	/// Will change to &DisableMatrixIndexArray2 if !ARB_matrix_palette
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLClearTargetsByScreenClearTargetMask[] =
{
	0,
	GL_COLOR_BUFFER_BIT,
	GL_DEPTH_BUFFER_BIT,
	GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
	GL_STENCIL_BUFFER_BIT,
	GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
	GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
	GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLTypesByAttributeType[ XEngine::XVertexAttributeType::Max ] =
{
	GL_BYTE,
	GL_UNSIGNED_BYTE,
	GL_SHORT,
	GL_UNSIGNED_SHORT,
	GL_INT,
	GL_UNSIGNED_INT,
	GL_FLOAT,
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLTextureNamesByAttributeId[ XEngine::XVertexAttributeId::Max ] =
{
	~0u,
	~0u,
	~0u,
	GL_TEXTURE0,
	GL_TEXTURE1,
	~0u,
	~0u,
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLPrimitiveModesByPrimitiveType[ XEngine::XPrimitiveType::Max ] =
{
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_POINTS
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLIndexTypesByIndexType[ XEngine::XIndexType::Max ] =
{
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT,
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLBufferUsageByFlags[] =
{
	~0u,
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW,
	~0u,
};

///----------------------------------------------------------------------------

static int gs_pIndexSizeByIndexType[ XEngine::XIndexType::Max ] =
{
	1,
	2
};

///----------------------------------------------------------------------------

static bool const gs_pIsTextureFormatCompressed[ XEngine::XTextureFormat::Max ] =
{
	false,
	false,
	false,
	false,
	false,
	false,
	false,
	true,
	true,
	true,
	true,
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLImageFormatByTextureFormat[ XEngine::XTextureFormat::Max ] =
{
	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_RGB,
	GL_RGB,
	GL_RGBA,
	GL_RGBA,
	GL_RGBA,
#if defined( TARGET_OPENGLES )
	GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
	GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
	GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
	GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
#else
	~0u,
	~0u,
	~0u,
	~0u,
#endif
};

///----------------------------------------------------------------------------

static GLenum const gs_pGLTexelTypeByTextureFormat[ XEngine::XTextureFormat::Max ] =
{
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT_5_6_5,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT_5_5_5_1,
	GL_UNSIGNED_SHORT_4_4_4_4,
	GL_UNSIGNED_BYTE,
	~0u,
	~0u,
	~0u,
	~0u,
};

///----------------------------------------------------------------------------

static GLsizei const gs_pTexelSizeByTextureFormat[ XEngine::XTextureFormat::Max ] =
{
	1,
	2,
	2,
	3,
	2,
	2,
	4,
	~0u,
	~0u,
	~0u,
	~0u,
};

///----------------------------------------------------------------------------

static GLsizei GetTextureImageSize( int nFormat, int nWidth, int nHeight )
{
	XEngine::uaddr const ROW_ALIGNMENT		= 4;
	XEngine::uaddr const PVRTC_BLOCK_SIZE	= 8;

	if( gs_pIsTextureFormatCompressed[ nFormat ] )
	{
		switch( nFormat )
		{
		case XEngine::XTextureFormat::RGB_PVRTC2:
		case XEngine::XTextureFormat::RGBA_PVRTC2:
			return XEngine::AlignUp( (nWidth * nHeight + 3) >> 2, PVRTC_BLOCK_SIZE );

		case XEngine::XTextureFormat::RGB_PVRTC4:
		case XEngine::XTextureFormat::RGBA_PVRTC4:
			return XEngine::AlignUp( (nWidth * nHeight + 1) >> 1, PVRTC_BLOCK_SIZE );

		default:
			return 0;
		}
	}
	else
	{
		return nHeight * XEngine::AlignUp( nWidth * gs_pTexelSizeByTextureFormat[ nFormat ], ROW_ALIGNMENT );
	}
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XGraphics::Init()
{
	/// Initialize extensions

	/* TODO: Check that extensions are supported */
	XGLExtInit();

	/// Initialize the hardware render state to default values

	static XVertexAttributeArrayState const pDefaultAttributeArrayState[ XVertexAttributeId::Max ] =
	{
		/// Position
		{ XVertexAttributeType::F32, 4, 0, 0, nullptr, false  },

		/// Normal
		{ XVertexAttributeType::F32, 3, 0, 0, nullptr, false  },

		/// Color
		{ XVertexAttributeType::F32, 4, 0, 0, nullptr, false  },

		/// TexCoord0
		{ XVertexAttributeType::F32, 4, 0, 0, nullptr, false  },

		/// TexCoord1
		{ XVertexAttributeType::F32, 4, 0, 0, nullptr, false  },

		/// Weights
		{ XVertexAttributeType::F32, 0, 0, 0, nullptr, false  },

		/// MatrixIndices
		{ XVertexAttributeType::U8, 0, 0, 0, nullptr, false  },
	};

	static XLightState const oDefaultLightState =
	{
		XVector4( 0, 0, 1, 0 ),
		0xFF000000,
		0xFF000000,
		0xFF000000,
		1, 0, 0,
		false
	};

	static int const pDefaultState[ XRenderStateEntry::Max ] =
	{
		0x00000000,		/// ClearColor
		0xFFFFFFFF,		/// Color
		0x00000000,		/// FaceCulling
		0x00000000,		/// DepthTest
		0x00000000,		/// Lighting
		0xFF000000,		/// MaterialEmissiveColor
		0xFF333333,		/// MaterialAmbientColor
		0xFFCCCCCC,		/// MaterialDiffuseColor
		0xFF000000,		/// MaterialSpecularColor
		0x00000000,		/// MaterialSpecularExponent
		0x00000000,		/// MatrixPalette
		0x00000000,		/// Texture0
		0x00000000,		/// Texture1
		0,				/// Texture0Binding
		0,				/// Texture1Binding
		0x00000000,		/// ViewportXY
		0xFFFFFFFF,		/// ViewportWH*
	};

	/// *: We don't actually know the initial viewport dimensions, because
	/// they're set to the initial size of the window that the context is
	/// bound to. However, it's reasonable to expect that the app won't try
	/// to set these particular values.

	static int const pDefaultDriverState[ XDriverRenderStateEntry::Max ] =
	{
		0,				/// VertexBufferBinding,
		0,				/// IndexBufferBinding,
		GL_TEXTURE0,	/// ClientActiveTexture,
		GL_TEXTURE0,	/// ActiveTexture
		GL_MODELVIEW,	/// MatrixMode
		0,				/// CurrentPaletteMatrix
	};

	for( int i = 0; i < XRenderStateMatrixEntry::Max; ++i )
	{
		m_oDriverRenderState.SetMatrix( i, IDENTITY );
	}

	for( int i = 0; i < XVertexAttributeId::Max; ++i )
	{
		m_oDriverRenderState.SetVertexAttributeArrayState( i, pDefaultAttributeArrayState[ i ] );
	}

	for( int i = 0; i < XRenderState::MAX_LIGHTS; ++i )
	{
		m_oDriverRenderState.SetLightState( i, oDefaultLightState );
	}

	XLightState & rLight0State		= m_oDriverRenderState.GetLightState( 0 );
	rLight0State.m_nDiffuseColor	= 0xFFFFFFFF;
	rLight0State.m_nSpecularColor	= 0xFFFFFFFF;

	for( int i = 0; i < XRenderStateEntry::Max; ++i )
	{
		m_oDriverRenderState.SetInt( (XRenderStateEntry::Type)i, pDefaultState[ i ] );
	}

	for( int i = 0; i < XDriverRenderStateEntry::Max; ++i )
	{
		m_oDriverRenderState.SetDriverInt( (XDriverRenderStateEntry::Type)i, pDefaultDriverState[ i ] );
	}

	/// Copy the initial hardware state into the current graphics state

	m_oRenderState = m_oDriverRenderState;

	/// Initialize the render state stack

	m_nStackDepth	= 0;
	m_nChangeFlags	= 0;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::Deinit()
{
	XGLExtDeinit();
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::SetClearColor( XColor const & rColor )
{
	m_oRenderState.SetInt( XRenderStateEntry::ClearColor, rColor.GetPacked() );
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::ClearScreen( int nClearTargetMask )
{
	if( nClearTargetMask & XScreenClearTarget::Color )
	{
		int nClearColor = m_oRenderState.GetInt( XRenderStateEntry::ClearColor );
		if( nClearColor != m_oDriverRenderState.GetInt( XRenderStateEntry::ClearColor ) )
		{
			XColor oColor( nClearColor );

			float fr = oColor.GetRed();
			float fg = oColor.GetGreen();
			float fb = oColor.GetBlue();
			float fa = oColor.GetAlpha();

			glClearColor( fr, fg, fb, fa );

			m_oDriverRenderState.SetInt( XRenderStateEntry::ClearColor, nClearColor );
		}
	}

	glClear( gs_pGLClearTargetsByScreenClearTargetMask[ nClearTargetMask & XScreenClearTarget::Mask ] );
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::SetColor( XColor const & rColor )
{
	m_oRenderState.SetInt( XRenderStateEntry::Color, rColor.GetPacked() );
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::SetTexture( int nTextureIndex, int nId )
{
	int nEntry = XRenderStateEntry::Texture0Binding + nTextureIndex;
	m_oRenderState.SetInt( (XRenderStateEntry::Type) nEntry, nId );
	m_nChangeFlags |= FLAG_TEXTURE;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::ApplyDrawingStates()
{
	int nFlags = m_nChangeFlags;

	XVertexAttributeArrayState const & rColorArrayState = m_oRenderState.GetVertexAttributeArrayState( XVertexAttributeId::Color );
	XVertexAttributeArrayState const & rDriverColorArrayState = m_oDriverRenderState.GetVertexAttributeArrayState( XVertexAttributeId::Color );

	int nColor = m_oRenderState.GetInt( XRenderStateEntry::Color );
	int nDriverColor = m_oDriverRenderState.GetInt( XRenderStateEntry::Color );

	/// Don't set the color if the color array is enabled.
	/// If we drew something with a color array, the current color is undefined, so update it.
	if( !rColorArrayState.m_bEnabled &&
		(nColor != nDriverColor || rDriverColorArrayState.m_bEnabled) )
	{
		glColor4ubv( (GLubyte *)&nColor );
		XASSERT_GL();

		m_oDriverRenderState.SetInt( XRenderStateEntry::Color, nColor );
	}

	int bEnabled = m_oRenderState.GetInt( XRenderStateEntry::DepthTest );
	int bEnabledInDriver = m_oDriverRenderState.GetInt( XRenderStateEntry::DepthTest );
	if( bEnabled && !bEnabledInDriver )
	{
		glEnable( GL_DEPTH_TEST );
		XASSERT_GL();
	}
	else if( !bEnabled && bEnabledInDriver )
	{
		glDisable( GL_DEPTH_TEST );
		XASSERT_GL();
	}
	m_oDriverRenderState.SetInt( XRenderStateEntry::DepthTest, bEnabled );

	bEnabled = m_oRenderState.GetInt( XRenderStateEntry::FaceCulling );
	bEnabledInDriver = m_oDriverRenderState.GetInt( XRenderStateEntry::FaceCulling );
	if( bEnabled && !bEnabledInDriver )
	{
		glEnable( GL_CULL_FACE );
		XASSERT_GL();
	}
	else if( !bEnabled && bEnabledInDriver )
	{
		glDisable( GL_CULL_FACE );
		XASSERT_GL();
	}
	m_oDriverRenderState.SetInt( XRenderStateEntry::FaceCulling, bEnabled );

	XColor oColor;
	XVector4 vColor;

	if( nFlags & FLAG_MATERIAL )
	{
		/* TODO: If we support color-material mode, check for color vertex arrays as above for ambient/diffuse */

		nColor = m_oRenderState.GetInt( XRenderStateEntry::MaterialAmbientColor );
		nDriverColor = m_oDriverRenderState.GetInt( XRenderStateEntry::MaterialAmbientColor );
		if( nColor != nDriverColor )
		{
			oColor.SetPacked( nColor );
			oColor.GetAsVector( vColor );
			glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, vColor.GetVectorPointer() );
			XASSERT_GL();
			m_oDriverRenderState.SetInt( XRenderStateEntry::MaterialAmbientColor, nColor );
		}

		nColor = m_oRenderState.GetInt( XRenderStateEntry::MaterialDiffuseColor );
		nDriverColor = m_oDriverRenderState.GetInt( XRenderStateEntry::MaterialDiffuseColor );
		if( nColor != nDriverColor )
		{
			oColor.SetPacked( nColor );
			oColor.GetAsVector( vColor );
			glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, vColor.GetVectorPointer() );
			XASSERT_GL();
			m_oDriverRenderState.SetInt( XRenderStateEntry::MaterialDiffuseColor, nColor );
		}

		nColor = m_oRenderState.GetInt( XRenderStateEntry::MaterialSpecularColor );
		nDriverColor = m_oDriverRenderState.GetInt( XRenderStateEntry::MaterialSpecularColor );
		if( nColor != nDriverColor )
		{
			oColor.SetPacked( nColor );
			oColor.GetAsVector( vColor );
			glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, vColor.GetVectorPointer() );
			XASSERT_GL();
			m_oDriverRenderState.SetInt( XRenderStateEntry::MaterialSpecularColor, nColor );
		}

		nColor = m_oRenderState.GetInt( XRenderStateEntry::MaterialEmissiveColor );
		nDriverColor = m_oDriverRenderState.GetInt( XRenderStateEntry::MaterialEmissiveColor );
		if( nColor != nDriverColor )
		{
			oColor.SetPacked( nColor );
			oColor.GetAsVector( vColor );
			glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, vColor.GetVectorPointer() );
			XASSERT_GL();
			m_oDriverRenderState.SetInt( XRenderStateEntry::MaterialEmissiveColor, nColor );
		}

		float fExponent = m_oRenderState.GetFloat( XRenderStateEntry::MaterialSpecularExponent );
		if( fExponent != m_oDriverRenderState.GetFloat( XRenderStateEntry::MaterialSpecularExponent ) )
		{
			glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, fExponent );
			XASSERT_GL();
			m_oDriverRenderState.SetFloat( XRenderStateEntry::MaterialSpecularExponent, fExponent );
		}

		nFlags &= ~FLAG_MATERIAL;
	}

	if( nFlags & FLAG_LIGHTS )
	{
		bEnabled = m_oRenderState.GetInt( XRenderStateEntry::Lighting );
		bEnabledInDriver = m_oDriverRenderState.GetInt( XRenderStateEntry::Lighting );
		if( bEnabled )
		{
			XMatrix44 const & rViewMatrix = m_oRenderState.GetMatrix( XRenderStateMatrixEntry::View );

			bool bMatrixReset = false;

			for( int i = 0; i < XRenderState::MAX_LIGHTS; ++i )
			{
				XLightState const & rLight = m_oRenderState.GetLightState( i );
				XLightState & rDriverLight = m_oDriverRenderState.GetLightState( i );

				GLenum eLight = i + GL_LIGHT0;

				if( rLight.m_bEnabled )
				{
					XVector4 vTransformedPosition;
					XMatrix44::TransformVectorUnsafe( vTransformedPosition, rLight.m_vPosition, rViewMatrix );
					if( vTransformedPosition != rDriverLight.m_vPosition )
					{
						if( !bMatrixReset )
						{
							if( m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::MatrixMode ) != GL_MODELVIEW )
							{
								glMatrixMode( GL_MODELVIEW );
								XASSERT_GL();
							}
							glLoadIdentity();
							XASSERT_GL();
							m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::MatrixMode, GL_MODELVIEW );
							m_oDriverRenderState.SetMatrix( XRenderStateMatrixEntry::ModelView, IDENTITY );
							nFlags |= FLAG_MODELVIEW;

							bMatrixReset = true;
						}
						glLightfv( eLight, GL_POSITION, vTransformedPosition.GetVectorPointer() );
						XASSERT_GL();
					}
					if( rLight.m_nAmbientColor != rDriverLight.m_nAmbientColor )
					{
						oColor.SetPacked( rLight.m_nAmbientColor );
						oColor.GetAsVector( vColor );
						glLightfv( eLight, GL_AMBIENT, vColor.GetVectorPointer() );
						XASSERT_GL();
					}
					if( rLight.m_nDiffuseColor != rDriverLight.m_nDiffuseColor )
					{
						oColor.SetPacked( rLight.m_nDiffuseColor );
						oColor.GetAsVector( vColor );
						glLightfv( eLight, GL_DIFFUSE, vColor.GetVectorPointer() );
						XASSERT_GL();
					}
					if( rLight.m_nSpecularColor != rDriverLight.m_nSpecularColor )
					{
						oColor.SetPacked( rLight.m_nSpecularColor );
						oColor.GetAsVector( vColor );
						glLightfv( eLight, GL_SPECULAR, vColor.GetVectorPointer() );
						XASSERT_GL();
					}
					if( rLight.m_fConstantAttenuation != rDriverLight.m_fConstantAttenuation )
					{
						glLightf( eLight, GL_CONSTANT_ATTENUATION, rLight.m_fConstantAttenuation );
						XASSERT_GL();
					}
					if( rLight.m_fLinearAttenuation != rDriverLight.m_fLinearAttenuation )
					{
						glLightf( eLight, GL_LINEAR_ATTENUATION, rLight.m_fLinearAttenuation );
						XASSERT_GL();
					}
					if( rLight.m_fQuadraticAttenuation != rDriverLight.m_fQuadraticAttenuation )
					{
						glLightf( eLight, GL_QUADRATIC_ATTENUATION, rLight.m_fQuadraticAttenuation );
						XASSERT_GL();
					}
					if( !rDriverLight.m_bEnabled )
					{
						glEnable( eLight );
						XASSERT_GL();
					}
					rDriverLight = rLight;
				}
				else if( rDriverLight.m_bEnabled )
				{
					glDisable( eLight );
					XASSERT_GL();

					rDriverLight.m_bEnabled = false;
				}
			}

			if( !bEnabledInDriver )
			{
				glEnable( GL_LIGHTING );
				XASSERT_GL();
			}
		}
		else if( bEnabledInDriver )
		{
			glDisable( GL_LIGHTING );
			XASSERT_GL();

		}
		m_oDriverRenderState.SetInt( XRenderStateEntry::Lighting, bEnabled );

		nFlags &= ~FLAG_LIGHTS;
	}

	XMatrix44 const * pMatrix;

	int nMatrixMode = m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::MatrixMode );

	if( nFlags & FLAG_VIEWPORTPROJ )
	{
		int nViewportXY = m_oRenderState.GetInt( XRenderStateEntry::ViewportXY );
		int nViewportWH = m_oRenderState.GetInt( XRenderStateEntry::ViewportWH );

		if( (nViewportXY != m_oDriverRenderState.GetInt( XRenderStateEntry::ViewportXY )) ||
			(nViewportWH != m_oDriverRenderState.GetInt( XRenderStateEntry::ViewportWH )) )
		{
			int x = nViewportXY & 0xFFFF;
			int y = (nViewportXY >> 16) & 0xFFFF;
			int w = nViewportWH & 0xFFFF;
			int h = (nViewportWH >> 16) & 0xFFFF;

			glViewport( x, y, w, h );
			XASSERT_GL();

			m_oDriverRenderState.SetInt( XRenderStateEntry::ViewportXY, nViewportXY );
			m_oDriverRenderState.SetInt( XRenderStateEntry::ViewportWH, nViewportWH );
		}

		pMatrix = &m_oRenderState.GetMatrix( XRenderStateMatrixEntry::Projection );
		if( *pMatrix != m_oDriverRenderState.GetMatrix( XRenderStateMatrixEntry::Projection ) )
		{
			if( nMatrixMode != GL_PROJECTION )
			{
				glMatrixMode( GL_PROJECTION );
				XASSERT_GL();

				nMatrixMode = GL_PROJECTION;
			}

			glLoadMatrixf( pMatrix->GetMatrixPointer() );
			XASSERT_GL();

			m_oDriverRenderState.SetMatrix( XRenderStateMatrixEntry::Projection, *pMatrix );
		}

		nFlags  &= ~FLAG_VIEWPORTPROJ;
	}

	int nEnabled, nEnabledInDriver;

	/* TODO: sort to reduce redundant glMatrixMode/glActiveTexture/glCurrentMatrix calls? */
	if( nFlags & FLAG_MODELVIEW )
	{
		XMatrix44 oModelViewMatrix;
		XMatrix44::MultiplyUnsafe(
			oModelViewMatrix,
			m_oRenderState.GetMatrix( XRenderStateMatrixEntry::Model ),
			m_oRenderState.GetMatrix( XRenderStateMatrixEntry::View ) );
		if( oModelViewMatrix != m_oDriverRenderState.GetMatrix( XRenderStateMatrixEntry::ModelView ) )
		{
			if( nMatrixMode != GL_MODELVIEW )
			{
				glMatrixMode( GL_MODELVIEW );
				XASSERT_GL();

				nMatrixMode = GL_MODELVIEW;
			}

			glLoadMatrixf( oModelViewMatrix.GetMatrixPointer() );
			XASSERT_GL();

			m_oDriverRenderState.SetMatrix( XRenderStateMatrixEntry::ModelView, oModelViewMatrix );
		}
		nFlags &= ~FLAG_MODELVIEW;
	}

	if( nFlags & FLAG_TEXTURE )
	{
		int nActiveTexture = m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::ActiveTexture );

		for( int i = 0; i < 2; ++i )
		{
			int nMatrixIndex				= i + XRenderStateMatrixEntry::Texture0;
			int nRenderStateEntry			= i + XRenderStateEntry::Texture0;
			int nRenderStateBindingEntry	= i + XRenderStateEntry::Texture0Binding;
			int nGLTextureName				= i + GL_TEXTURE0;

			nEnabled = m_oRenderState.GetInt( (XRenderStateEntry::Type)nRenderStateEntry );
			nEnabledInDriver = m_oDriverRenderState.GetInt( (XRenderStateEntry::Type)nRenderStateEntry );

			if( nActiveTexture != nGLTextureName )
			{
				glActiveTexture( nGLTextureName );
				XASSERT_GL();

				nActiveTexture = nGLTextureName;
			}

			if( nEnabled )
			{
				if( !nEnabledInDriver )
				{
					//if( nActiveTexture != nGLTextureName )
					//{
					//	glActiveTexture( nGLTextureName );
					//	XASSERT_GL();

					//	nActiveTexture = nGLTextureName;
					//}

					glEnable( GL_TEXTURE_2D );
					XASSERT_GL();
				}

				int nBinding = m_oRenderState.GetInt( (XRenderStateEntry::Type)nRenderStateBindingEntry );
				if( nBinding != m_oDriverRenderState.GetInt( (XRenderStateEntry::Type)nRenderStateBindingEntry ) )
				{
					//if( nActiveTexture != nGLTextureName )
					//{
					//	glActiveTexture( nGLTextureName );
					//	XASSERT_GL();

					//	nActiveTexture = nGLTextureName;
					//}

					glBindTexture( GL_TEXTURE_2D, nBinding );
					XASSERT_GL();

					m_oDriverRenderState.SetInt( (XRenderStateEntry::Type)nRenderStateBindingEntry, nBinding );
				}

				pMatrix = &m_oRenderState.GetMatrix( nMatrixIndex );
				if( *pMatrix != m_oDriverRenderState.GetMatrix( nMatrixIndex ) )
				{
					if( nMatrixMode != GL_TEXTURE )
					{
						glMatrixMode( GL_TEXTURE );
						XASSERT_GL();

						nMatrixMode = GL_TEXTURE;
					}

					//if( nActiveTexture != nGLTextureName )
					//{
					//	glActiveTexture( nGLTextureName );
					//	XASSERT_GL();

					//	nActiveTexture = nGLTextureName;
					//}

					glLoadMatrixf( pMatrix->GetMatrixPointer() );
					XASSERT_GL();

					m_oDriverRenderState.SetMatrix( nMatrixIndex, *pMatrix );
				}
			}
			else if( nEnabledInDriver )
			{
				//if( nActiveTexture != nGLTextureName )
				//{
				//	glActiveTexture( nGLTextureName );
				//	XASSERT_GL();

				//	nActiveTexture = nGLTextureName;
				//}

				glDisable( GL_TEXTURE_2D );
				XASSERT_GL();
			}
			m_oDriverRenderState.SetInt( (XRenderStateEntry::Type)nRenderStateEntry, nEnabled );
		}

		m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::ActiveTexture, nActiveTexture );

		nFlags &= ~FLAG_TEXTURE;
	}

	if( nFlags & FLAG_MATRIX_PALETTE )
	{
		int nCurrentMatrix = m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::CurrentPaletteMatrix );

		nEnabled = m_oRenderState.GetInt( XRenderStateEntry::MatrixPalette );
		nEnabledInDriver = m_oDriverRenderState.GetInt( XRenderStateEntry::MatrixPalette );
		if( nEnabled )
		{
			if( !nEnabledInDriver )
			{
#if defined( TARGET_OPENGLES )
				glEnable( GL_MATRIX_PALETTE_OES );
				XASSERT_GL();
#else
				/* TODO */
#endif
			}

			for( int i = XRenderStateMatrixEntry::MatrixPaletteBegin; i < XRenderStateMatrixEntry::MatrixPaletteEnd; ++i )
			{
				pMatrix = &m_oRenderState.GetMatrix( i );
				if( *pMatrix == m_oDriverRenderState.GetMatrix( i ) )
				{
					continue;
				}

				int nIndex = i - XRenderStateMatrixEntry::MatrixPaletteBegin;

#if defined( TARGET_OPENGLES )
				if( nMatrixMode != GL_MATRIX_PALETTE_OES )
				{
					glMatrixMode( GL_MATRIX_PALETTE_OES );
					XASSERT_GL();

					nMatrixMode = GL_MATRIX_PALETTE_OES;
				}

				if( nCurrentMatrix != nIndex )
				{
					glCurrentPaletteMatrixOES( nIndex );
					XASSERT_GL();

					nCurrentMatrix = nIndex;
				}

				glLoadMatrixf( pMatrix->GetMatrixPointer() );
				XASSERT_GL();
#else
				/* TODO: Figure out what to do here */
#endif

				m_oDriverRenderState.SetMatrix( i, *pMatrix );
			}
		}
		else if( nEnabledInDriver )
		{
#if defined( TARGET_OPENGLES )
			glDisable( GL_MATRIX_PALETTE_OES );
			XASSERT_GL();
#else
			/* TODO */
#endif
		}

		m_oDriverRenderState.SetInt( XRenderStateEntry::MatrixPalette, nEnabled );
		m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::CurrentPaletteMatrix, nCurrentMatrix );

		nFlags &= ~FLAG_MATRIX_PALETTE;
	}

	m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::MatrixMode, nMatrixMode );

	m_nChangeFlags = nFlags;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::ApplyVertexArrayStates()
{
	if( ~m_nChangeFlags & FLAG_VERTEX_ATTRIB )
	{
		return;
	}

	XVertexAttributeArrayState const * pArrayState;
	XVertexAttributeArrayState * pDriverArrayState;

	int nCurrentVertexBufferBinding		= m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::VertexBufferBinding );
	int nClientActiveTexture			= m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::ClientActiveTexture );
#if 1
	int nThisPassVertexBufferBinding	= nCurrentVertexBufferBinding;
	int nMask							= (1 << XVertexAttributeId::Max) - 1;

	/// First check for unchanged or disabled attributes

	for( int iAttribute = 0; iAttribute < XVertexAttributeId::Max; ++iAttribute )
	{
		int nAttributeMask = (1 << iAttribute);

		pArrayState			= &m_oRenderState.GetVertexAttributeArrayState( (XVertexAttributeId::Type)iAttribute );
		pDriverArrayState	= &m_oDriverRenderState.GetVertexAttributeArrayState( (XVertexAttributeId::Type)iAttribute );

		if( (pDriverArrayState->m_eType					== pArrayState->m_eType					) &&
			(pDriverArrayState->m_nNumComponents		== pArrayState->m_nNumComponents		) &&
			(pDriverArrayState->m_nVertexStride			== pArrayState->m_nVertexStride			) &&
			(pDriverArrayState->m_nVertexBufferBinding	== pArrayState->m_nVertexBufferBinding	) &&
			(pDriverArrayState->m_pPointerOrOffset		== pArrayState->m_pPointerOrOffset		) &&
			(pDriverArrayState->m_bEnabled				== pArrayState->m_bEnabled				) )
		{
			/// Lucky!
			nMask &= ~nAttributeMask;
		}
		else if( !pArrayState->m_bEnabled )
		{
			if( pDriverArrayState->m_bEnabled )
			{
				GLenum eTexture = gs_pGLTextureNamesByAttributeId[ iAttribute ];

				if( (eTexture != ~0u) &&
					(eTexture != nClientActiveTexture) )
				{
					glClientActiveTexture( eTexture );
					XASSERT_GL();

					nClientActiveTexture = eTexture;
				}

				(*gs_pfnDisableAttrib[ iAttribute ])();
				XASSERT_GL();

				pDriverArrayState->m_bEnabled = false;
			}

			nMask &= ~nAttributeMask;
		}
	}

	/// The point of this loop is to minimize the number of vertex buffer
	/// object binding changes.
	/// On the first iteration, set up any arrays that use the current binding
	/// (whatever state we inherited).
	/// On subsequent iterations, find an array that hasn't been set up yet,
	/// set it up, and then set up any other arrays that share the same
	/// binding.
	/// Continue until all arrays have been set up.

	while( nMask != 0 )
	{
		/*
		TODO: If we have an intrinsic to count trailing zeroes (or ones), we
		can skip any initial sequence of masked-out attributes.
		*/
		for( int iAttribute = 0; iAttribute < XVertexAttributeId::Max; ++iAttribute )
		{
			int nAttributeMask = (1 << iAttribute);

			if( (nMask & nAttributeMask) == 0 )
			{
				continue;
			}

			pArrayState			= &m_oRenderState.GetVertexAttributeArrayState( iAttribute );
			pDriverArrayState	= &m_oDriverRenderState.GetVertexAttributeArrayState( iAttribute );

			/// Attributes that aren't enabled should have been masked out in the first loop
			XASSERT( pArrayState->m_bEnabled );

			int nVertexBufferBinding = pArrayState->m_nVertexBufferBinding;

			if( nThisPassVertexBufferBinding < 0 )
			{
				nCurrentVertexBufferBinding = nThisPassVertexBufferBinding = nVertexBufferBinding;
				glBindBuffer( GL_ARRAY_BUFFER, nCurrentVertexBufferBinding );
				XASSERT_GL();
			}
			else if( nVertexBufferBinding != nCurrentVertexBufferBinding )
			{
				continue;
			}

			GLenum eTexture = gs_pGLTextureNamesByAttributeId[ iAttribute ];

			if( (eTexture != ~0u) &&
				(eTexture != nClientActiveTexture) )
			{
				glClientActiveTexture( eTexture );
				XASSERT_GL();

				nClientActiveTexture = eTexture;
			}

			int eType						= pArrayState->m_eType;
			int nNumComponents				= pArrayState->m_nNumComponents;
			int nVertexStride				= pArrayState->m_nVertexStride;
			void const * pPointerOrOffset	= pArrayState->m_pPointerOrOffset;

			if( (pDriverArrayState->m_eType					!= eType						) ||
				(pDriverArrayState->m_nNumComponents		!= nNumComponents				) ||
				(pDriverArrayState->m_nVertexStride			!= nVertexStride				) ||
				(pDriverArrayState->m_nVertexBufferBinding	!= nCurrentVertexBufferBinding	) ||
				(pDriverArrayState->m_pPointerOrOffset		!= pPointerOrOffset				) )
			{
				(*gs_pfnAttributePointer[ iAttribute ])(
					nNumComponents,
					gs_pGLTypesByAttributeType[ eType ],
					nVertexStride,
					pPointerOrOffset );
				XASSERT_GL();
			}

			if( !pDriverArrayState->m_bEnabled )
			{
				(*gs_pfnEnableAttrib[ iAttribute ])();
				XASSERT_GL();
			}

			*pDriverArrayState = *pArrayState;

			nMask &= ~nAttributeMask;
		}

		nThisPassVertexBufferBinding = -1;
	}
#else
	for( int i = 0; i < XVertexAttributeId::Max; ++i )
	{
		pArrayState = &m_oRenderState.GetVertexAttributeArrayState( i );
		pDriverArrayState = &m_oDriverRenderState.GetVertexAttributeArrayState( i );

		if( (pDriverArrayState->m_eType					== pArrayState->m_eType					) &&
			(pDriverArrayState->m_nNumComponents		== pArrayState->m_nNumComponents		) &&
			(pDriverArrayState->m_nVertexStride			== pArrayState->m_nVertexStride			) &&
			(pDriverArrayState->m_nVertexBufferBinding	== pArrayState->m_nVertexBufferBinding	) &&
			(pDriverArrayState->m_pPointerOrOffset		== pArrayState->m_pPointerOrOffset		) &&
			(pDriverArrayState->m_bEnabled				== pArrayState->m_bEnabled				) )
		{
			/// Lucky!
			continue;
		}
		else if( pArrayState->m_bEnabled )
		{
			int nVertexBufferBinding = pArrayState->m_nVertexBufferBinding;

			if( nCurrentVertexBufferBinding != nVertexBufferBinding )
			{
				nCurrentVertexBufferBinding = nVertexBufferBinding;

				glBindBuffer( GL_ARRAY_BUFFER, nCurrentVertexBufferBinding );
				XASSERT_GL();
			}

			GLenum eTexture = gs_pGLTextureNamesByAttributeId[ i ];

			if( (eTexture != ~0u) &&
				(eTexture != nClientActiveTexture) )
			{
				glClientActiveTexture( eTexture );
				XASSERT_GL();

				nClientActiveTexture = eTexture;
			}

			int eType						= pArrayState->m_eType;
			int nNumComponents				= pArrayState->m_nNumComponents;
			int nVertexStride				= pArrayState->m_nVertexStride;
			void const * pPointerOrOffset	= pArrayState->m_pPointerOrOffset;

			if( (pDriverArrayState->m_eType					!= eType						) ||
				(pDriverArrayState->m_nNumComponents		!= nNumComponents				) ||
				(pDriverArrayState->m_nVertexStride			!= nVertexStride				) ||
				(pDriverArrayState->m_nVertexBufferBinding	!= nCurrentVertexBufferBinding	) ||
				(pDriverArrayState->m_pPointerOrOffset		!= pPointerOrOffset				) )
			{
				(*gs_pfnAttributePointer[ i ])(
					nNumComponents,
					gs_pGLTypesByAttributeType[ eType ],
					nVertexStride,
					pPointerOrOffset );
				XASSERT_GL();
			}

			if( !pDriverArrayState->m_bEnabled )
			{
				(*gs_pfnEnableAttrib[ i ])();
				XASSERT_GL();
			}

			*pDriverArrayState = *pArrayState;
		}
		else if( pDriverArrayState->m_bEnabled )
		{
			(*gs_pfnDisableAttrib[ i ])();
			XASSERT_GL();

			pDriverArrayState->m_bEnabled = false;
		}
	}
#endif

	m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::VertexBufferBinding, nCurrentVertexBufferBinding );
	m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::ClientActiveTexture, nClientActiveTexture );

	m_nChangeFlags &= ~FLAG_VERTEX_ATTRIB;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::DrawElements(
	int eIndexType,
	int ePrimitiveType,
	int nNumIndices,
	int nIndexBufferBinding,
	void const * pPointerOrOffset )
{
	ApplyDrawingStates();
	ApplyVertexArrayStates();

	if( nIndexBufferBinding != m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::IndexBufferBinding ) )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, nIndexBufferBinding );
		XASSERT_GL();

		m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::IndexBufferBinding, nIndexBufferBinding );
	}

	glDrawElements(
		gs_pGLPrimitiveModesByPrimitiveType[ ePrimitiveType ],
		nNumIndices,
		gs_pGLIndexTypesByIndexType[ eIndexType ],
		pPointerOrOffset );
	XASSERT_GL();
}

///----------------------------------------------------------------------------

int XEngine::XGraphics::CreateIndexBufferObject( int nIndexType, int nNumIndices, void const * pBufferData, int nFlags )
{
	int nUsageFlags = nFlags & XBufferFlags::UsageMask;

	if( nUsageFlags == 0 )
	{
		return 0;
	}

	GLuint nId;

	uaddr nBufferSize = nNumIndices * gs_pIndexSizeByIndexType[ nIndexType ];

	glGenBuffers( 1, &nId );
	XASSERT_GL();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, nId );
	XASSERT_GL();

	glBufferData( GL_ELEMENT_ARRAY_BUFFER, nBufferSize, pBufferData, gs_pGLBufferUsageByFlags[ nUsageFlags ] );
	XASSERT_GL();

	m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::IndexBufferBinding, nId );

	return nId;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::DestroyIndexBufferObject( int nId )
{
	if( nId == 0 )
	{
		return;
	}

	GLuint nBufferId = nId;

	glDeleteBuffers( 1, &nBufferId );
	XASSERT_GL();

	if( m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::IndexBufferBinding ) == nId )
	{
		m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::IndexBufferBinding, 0 );
	}
}

///----------------------------------------------------------------------------

int XEngine::XGraphics::CreateVertexBufferObject( XVertexFormat const & rFormat, int nNumVertices, void const * pBufferData, int nFlags )
{
	int nUsageFlags = nFlags & XBufferFlags::UsageMask;

	if( nUsageFlags == 0 )
	{
		return 0;
	}

	GLuint nId;

	uaddr nBufferSize = nNumVertices * rFormat.GetVertexStride();

	glGenBuffers( 1, &nId );
	XASSERT_GL();

	glBindBuffer( GL_ARRAY_BUFFER, nId );
	XASSERT_GL();

	glBufferData( GL_ARRAY_BUFFER, nBufferSize, pBufferData, gs_pGLBufferUsageByFlags[ nFlags & XBufferFlags::UsageMask ] );
	XASSERT_GL();

	m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::VertexBufferBinding, nId );
	m_nChangeFlags |= FLAG_VERTEX_ATTRIB;

	return nId;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::DestroyVertexBufferObject( int nId )
{
	if( nId == 0 )
	{
		return;
	}

	GLuint nBufferId = nId;

	glDeleteBuffers( 1, &nBufferId );
	XASSERT_GL();

	if( m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::VertexBufferBinding ) == nId )
	{
		m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::VertexBufferBinding, 0 );
		m_nChangeFlags |= FLAG_VERTEX_ATTRIB;
	}
}

///----------------------------------------------------------------------------

int XEngine::XGraphics::CreateTextureObject( int nFormat, int nWidth, int nHeight, void const * pTexels, int nFlags )
{
	static uaddr const TEXTURE_IMAGE_ALIGNMENT = 16;

	GLuint nId;

	if( m_oDriverRenderState.GetDriverInt( XDriverRenderStateEntry::ActiveTexture ) != GL_TEXTURE0 )
	{
		glActiveTexture( GL_TEXTURE0 );
		XASSERT_GL();
	}

	glGenTextures( 1, &nId );
	XASSERT_GL();

	glBindTexture( GL_TEXTURE_2D, nId );
	XASSERT_GL();

	GLenum const eFormat		= gs_pGLImageFormatByTextureFormat[ nFormat ];
	GLenum const eType			= gs_pGLTexelTypeByTextureFormat[ nFormat ];

	bool const bHasMips			= (nFlags & XTextureFlags::HasMips) != 0;
	bool const bGenerateMips	= (nFlags & XTextureFlags::GenerateMips) != 0;
	bool const bCompressed		= gs_pIsTextureFormatCompressed[ nFormat ];
	bool const bMips			= bHasMips || (bGenerateMips && !bCompressed);

	GLsizei nImageSize			= GetTextureImageSize( nFormat, nWidth, nHeight );

	if( bCompressed )
	{
		glCompressedTexImage2D(
			GL_TEXTURE_2D,
			0,
			eFormat,
			nWidth,
			nHeight,
			0,
			nImageSize,
			pTexels );
		XASSERT_GL();

		if( bHasMips )
		{
			uaddr nTexelAddress = (uaddr)pTexels;
			int nLevel = 1;
			while( (nWidth >>= 1) && (nHeight >>= 1) )
			{
				nTexelAddress += AlignUp( nImageSize, TEXTURE_IMAGE_ALIGNMENT );
				nImageSize = GetTextureImageSize( nFormat, nWidth, nHeight );
				glCompressedTexImage2D(
					GL_TEXTURE_2D,
					nLevel++,
					eFormat,
					nWidth,
					nHeight,
					0,
					nImageSize,
					(void *)nTexelAddress );
				XASSERT_GL();
			}
		}
	}
	else
	{
		if( bGenerateMips && !bHasMips )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
			XASSERT_GL();
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			eFormat,
			nWidth,
			nHeight,
			0,
			eFormat,
			eType,
			pTexels );
		XASSERT_GL();

		if( bHasMips )
		{
			uaddr nTexelAddress = (uaddr)pTexels;
			int nLevel = 1;
			while( (nWidth >>= 1) && (nHeight >>= 1) )
			{
				nTexelAddress += AlignUp( nImageSize, TEXTURE_IMAGE_ALIGNMENT );
				nImageSize = GetTextureImageSize( nFormat, nWidth, nHeight );
				glTexImage2D(
					GL_TEXTURE_2D,
					nLevel++,
					eFormat,
					nWidth,
					nHeight,
					0,
					eFormat,
					eType,
					(void *)nTexelAddress );
				XASSERT_GL();
			}
		}
	}

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, bMips ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR );
	XASSERT_GL();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	XASSERT_GL();

	m_oDriverRenderState.SetDriverInt( XDriverRenderStateEntry::ActiveTexture, GL_TEXTURE0 );
	m_oDriverRenderState.SetInt( XRenderStateEntry::Texture0Binding, nId );

	m_nChangeFlags |= FLAG_TEXTURE;
	
	return nId;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::DestroyTextureObject( int nId )
{
	if( nId == 0 )
	{
		return;
	}

	GLuint nTextureId = nId;

	glDeleteTextures( 1, &nTextureId );
	XASSERT_GL();

	if( nId == m_oDriverRenderState.GetInt( XRenderStateEntry::Texture0Binding ) )
	{
		m_oDriverRenderState.SetInt( XRenderStateEntry::Texture0Binding, 0 );
		m_nChangeFlags |= FLAG_TEXTURE;
	}

	if( nId == m_oDriverRenderState.GetInt( XRenderStateEntry::Texture1Binding ) )
	{
		m_oDriverRenderState.SetInt( XRenderStateEntry::Texture1Binding, 0 );
		m_nChangeFlags |= FLAG_TEXTURE;
	}
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::SetMaterial(
	XColor const & rAmbientColor,
	XColor const & rDiffuseColor,
	XColor const & rSpecularColor,
	XColor const & rEmissiveColor,
	float fSpecularExponent )
{
	m_oRenderState.SetInt( XRenderStateEntry::MaterialAmbientColor, rAmbientColor.GetPacked() );
	m_oRenderState.SetInt( XRenderStateEntry::MaterialDiffuseColor, rDiffuseColor.GetPacked() );
	m_oRenderState.SetInt( XRenderStateEntry::MaterialSpecularColor, rSpecularColor.GetPacked() );
	m_oRenderState.SetInt( XRenderStateEntry::MaterialEmissiveColor, rEmissiveColor.GetPacked() );
	m_oRenderState.SetFloat( XRenderStateEntry::MaterialSpecularExponent, fSpecularExponent );

	m_nChangeFlags |= FLAG_MATERIAL;
}

///----------------------------------------------------------------------------

void XEngine::XGraphics::SetLight(
	int nIndex,
	XVector4 const & rPosition,
	XColor const & rAmbientColor,
	XColor const & rDiffuseColor,
	XColor const & rSpecularColor,
	XVector4 const & rAttenuation )
{
	XLightState & rState = m_oRenderState.GetLightState( nIndex );

	rState.m_vPosition				= rPosition;
	rState.m_nAmbientColor			= rAmbientColor.GetPacked();
	rState.m_nDiffuseColor			= rDiffuseColor.GetPacked();
	rState.m_nSpecularColor			= rSpecularColor.GetPacked();
	rState.m_fConstantAttenuation	= rAttenuation(0);
	rState.m_fLinearAttenuation		= rAttenuation(1);
	rState.m_fQuadraticAttenuation	= rAttenuation(2);

	m_nChangeFlags |= FLAG_LIGHTS;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
