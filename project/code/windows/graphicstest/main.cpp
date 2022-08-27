
#include "engine/core/xcore.h"
#include "engine/core/xallocator.h"
#include "engine/algorithms/xutil.h"
#include "engine/graphics/xgraphics.h"
#include "engine/graphics/xvertexbuffer.h"
#include "engine/graphics/xmesh.h"
#include "engine/graphics/xtexture.h"
#include "engine/graphics/xcolor.h"

#include "engine/core/xmswin.h"
#include "windows/core/window.h"

#include <cmath>
#include <cstdlib>

using namespace XEngine;
using namespace Windows;
using namespace std;

class MyAllocator : public XEngine::XAllocator
{
public:
	MyAllocator()
	{
		Register( (void *)0, (void *)~0 );
	}

	virtual ~MyAllocator()
	{
		Unregister();
	}

	virtual void *	Allocate(
						uaddr nSize,
						uaddr nAlignment,
						uaddr nAlignmentOffset,
						char const * pFile,
						int nLine )
	{
		return _aligned_offset_malloc( nSize, nAlignment, nAlignmentOffset );
	}

	virtual void *	Reallocate(
						void * p,
						uaddr nNewSize,
						uaddr nNewAlignment,
						uaddr nAlignmentOffset,
						char const * pFile,
						int nLine )
	{
		return _aligned_offset_realloc( p, nNewSize, nNewAlignment, nAlignmentOffset );
	}

	virtual void	Free( void * p )
	{
		return _aligned_free( p );
	}
};

class MyWindow : public Windows::CWindow
{
public:
	MyWindow()
	{
		m_nButtonMask	= 0;
		m_fTheta		= -PI/6;
		m_fPhi			= PI/6;
		m_fRadius		= 10;
	}

protected:
	virtual void OnCreate()
	{
		m_oGraphics.Init();
		m_oGraphics.SetClearColor( XColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
		m_oGraphics.EnableDepthTest( true );
		m_oGraphics.EnableFaceCulling( true );

		MakeTexture();
		MakeMeshes();
		MakeView();
	}

	virtual void OnDestroy()
	{
		m_oSquare.GetIndexBuffer().Deinit( &m_oGraphics );
		m_oAxes.GetIndexBuffer().Deinit( &m_oGraphics );
		m_oSquareVertexBuffer.Deinit( &m_oGraphics );
		m_oAxesVertexBuffer.Deinit( &m_oGraphics );
		m_oTexture.Deinit( &m_oGraphics );
		m_oGraphics.Deinit();
	}

	virtual void OnUpdate( float fDT )
	{
	}

	virtual void OnRender()
	{
		m_oGraphics.ClearScreen( XScreenClearTarget::Color | XScreenClearTarget::Depth );

		m_oGraphics.PushRenderState();

		m_oGraphics.SetViewMatrix( m_mViewMatrix );

		m_oGraphics.PushRenderState();
		m_oAxes.Draw( &m_oGraphics );
		m_oGraphics.PopRenderState();

		m_oGraphics.PushRenderState();
		m_oGraphics.SetModelMatrix( m_mSquareMatrix );
		m_oGraphics.SetColor( XColor( 1, 1, 1, 1 ) );
		m_oTexture.Select( &m_oGraphics, 0 );
		m_oGraphics.EnableTexture( 0, true );
		m_oGraphics.SetLight(
			0,
			XVector4( 1, 3, 2, 0 ),
			XColor( 1, 1, 1, 1 ),
			XColor( 1, 1, 1, 1 ),
			XColor( 1, 1, 1, 1 ),
			XVector4( 1, 0, 0, 0 ) );
		m_oGraphics.EnableLight( 0, true );
		m_oGraphics.EnableLighting( true );
		m_oSquare.Draw( &m_oGraphics );
		m_oGraphics.PopRenderState();

		m_oGraphics.PopRenderState();
	}

	virtual void OnResize( CDimensions const & rDimensions )
	{
		int w = rDimensions.GetWidth();
		int h = rDimensions.GetHeight();

		float const fNear = 1.0f;
		float const fFar = 1000.0f;
		float const fFovy = PI / 6;	/// 30 degrees
		float fAspect = (float) w / (float) h;

		XMatrix44 mProjection;

		m_oGraphics.SetViewport( 0, 0, w, h );
		m_oGraphics.SetProjectionMatrix( mProjection.MakePerspective( fNear, fFar, fFovy, fAspect ) );
	}

	virtual void OnMouseDown( CPoint const & rPoint, int nButton )
	{
		if( m_nButtonMask == 0 )
		{
			CaptureMouse();
		}

		m_oMousePos = rPoint;
		m_nButtonMask |= (1 << nButton);
	}

	virtual void OnMouseUp( CPoint const & rPoint, int nButton )
	{
		m_oMousePos = rPoint;
		m_nButtonMask &= ~(1 << nButton);

		if( m_nButtonMask == 0 )
		{
			ReleaseMouse();
		}
	}

	virtual void OnMouseMove( CPoint const & rPoint )
	{
		int dx = rPoint.GetX() - m_oMousePos.GetX();
		int dy = rPoint.GetY() - m_oMousePos.GetY();

		float const THETA_PER_PIXEL = 1.0f * PI / 180.0f;
		float const PHI_PER_PIXEL = 1.0f * PI / 180.0f;

		switch( m_nButtonMask )
		{
		case 1:
			m_fTheta = fmodf( m_fTheta + dx * THETA_PER_PIXEL, 2*PI );
			m_fPhi = Clamp( m_fPhi + dy * PHI_PER_PIXEL, -PI/2, PI/2 );
			break;
		}

		m_oMousePos = rPoint;

		MakeView();
	}


private:
	void MakeTexture()
	{
		static int const TEXTURE_WIDTH = 64;
		static int const TEXTURE_HEIGHT = 64;

		u8 (*pTexture)[TEXTURE_WIDTH] = XNEW_ALIGNED( &m_oAllocator, 16 ) u8 [TEXTURE_HEIGHT][TEXTURE_WIDTH];

		for( int i = 0; i < TEXTURE_HEIGHT; ++i )
		{
			for( int j = 0; j < TEXTURE_WIDTH; ++j )
			{
				pTexture[i][j] = rand() & 0xFF;
			}
		}

		m_oTexture.Init(
			&m_oGraphics,
			XTextureFormat::L_8,
			TEXTURE_WIDTH, TEXTURE_HEIGHT,
			XTextureFlags::GenerateMips | XTextureFlags::Discardable,
			pTexture );

		XDELETE_ARRAY( pTexture, &m_oAllocator );
	}

	void MakeMeshes()
	{
		__declspec(align(4)) struct Vertex
		{
			float x, y, z;
		};

		__declspec(align(4)) struct Normal
		{
			s8 x, y, z;
		};

		__declspec(align(4)) struct Color
		{
			u8 r, g, b, a;
		};

		__declspec(align(4)) struct TexCoord
		{
			s16	u, v;
		};

		struct AxisVertex
		{
			Vertex v;
			Color c;
		};

		struct SquareVertex
		{
			Vertex v;
			Normal n;
			TexCoord uv;
		};

		Color const red		= { 255, 0, 0, 255 };
		Color const green	= { 0, 255, 0, 255 };
		Color const blue	= { 0, 0, 255, 255 };
		Color const white	= { 255, 255, 255, 255 };

		AxisVertex pAxisVertices[] =
		{
			{ { 0, 0, 0 }, red },
			{ { 1, 0, 0 }, red },
			{ { 0, 0, 0 }, green },
			{ { 0, 1, 0 }, green },
			{ { 0, 0, 0 }, blue },
			{ { 0, 0, 1 }, blue },
		};

		u8 pAxisIndices[] =
		{
			0, 1,
			2, 3,
			4, 5,
		};

		XVertexFormat oAxisVF;
		oAxisVF.Add( XVertexAttributeId::Position, XVertexAttributeType::F32, 3 );
		oAxisVF.Add( XVertexAttributeId::Color, XVertexAttributeType::U8, 4 );
		m_oAxesVertexBuffer.Init(
			&m_oGraphics,
			oAxisVF,
			XBufferFlags::StaticDraw | XBufferFlags::Discardable,
			sizeof( pAxisVertices ) / sizeof( AxisVertex ),
			&pAxisVertices );
		m_oAxes.GetVertexBufferSet().Add( &m_oAxesVertexBuffer );
		m_oAxes.GetIndexBuffer().Init(
			&m_oGraphics,
			XPrimitiveType::LineList,
			XIndexType::U8,
			XBufferFlags::StaticDraw | XBufferFlags::Discardable,
			sizeof( pAxisIndices ) / sizeof( u8 ),
			&pAxisIndices );

		SquareVertex pSquareVertices[] =
		{
			{ {  1,  1,  1 }, { 0,  0x7F, 0 }, { 0, 0 } },
			{ {  1,  1, -1 }, { 0,  0x7F, 0 }, { 0, 1 } },
			{ { -1,  1,  1 }, { 0,  0x7F, 0 }, { 1, 0 } },
			{ { -1,  1, -1 }, { 0,  0x7F, 0 }, { 1, 1 } },

			{ { -1,  1,  1 }, { -0x80, 0, 0 }, { 0, 0 } },
			{ { -1,  1, -1 }, { -0x80, 0, 0 }, { 0, 1 } },
			{ { -1, -1,  1 }, { -0x80, 0, 0 }, { 1, 0 } },
			{ { -1, -1, -1 }, { -0x80, 0, 0 }, { 1, 1 } },

			{ { -1, -1,  1 }, { 0, -0x80, 0 }, { 0, 0 } },
			{ { -1, -1, -1 }, { 0, -0x80, 0 }, { 0, 1 } },
			{ {  1, -1,  1 }, { 0, -0x80, 0 }, { 1, 0 } },
			{ {  1, -1, -1 }, { 0, -0x80, 0 }, { 1, 1 } },

			{ {  1, -1,  1 }, {  0x7F, 0, 0 }, { 0, 0 } },
			{ {  1, -1, -1 }, {  0x7F, 0, 0 }, { 0, 1 } },
			{ {  1,  1,  1 }, {  0x7F, 0, 0 }, { 1, 0 } },
			{ {  1,  1, -1 }, {  0x7F, 0, 0 }, { 1, 1 } },

			{ { -1, -1,  1 }, { 0, 0, 0x7F }, { 0, 0 } },
			{ {  1, -1,  1 }, { 0, 0, 0x7F }, { 0, 1 } },
			{ { -1,  1,  1 }, { 0, 0, 0x7F }, { 1, 0 } },
			{ {  1,  1,  1 }, { 0, 0, 0x7F }, { 1, 1 } },

			{ {  1, -1, -1 }, { 0, 0, -0x80 }, { 0, 0 } },
			{ { -1, -1, -1 }, { 0, 0, -0x80 }, { 0, 1 } },
			{ {  1,  1, -1 }, { 0, 0, -0x80 }, { 1, 0 } },
			{ { -1,  1, -1 }, { 0, 0, -0x80 }, { 1, 1 } },
		};

		u8 pSquareIndices[] = 
		{
			0, 1, 2, 2, 1, 3,
			4, 5, 6, 6, 5, 7,
			8, 9, 10, 10, 9, 11,
			12, 13, 14, 14, 13, 15,
			16, 17, 18, 18, 17, 19,
			20, 21, 22, 22, 21, 23
		};

		XVertexFormat oSquareVF;
		oSquareVF.Add( XVertexAttributeId::Position, XVertexAttributeType::F32, 3 );
		oSquareVF.Add( XVertexAttributeId::Normal, XVertexAttributeType::S8, 3 );
		oSquareVF.Add( XVertexAttributeId::TexCoord0, XVertexAttributeType::S16, 2 );
		m_oSquareVertexBuffer.Init(
			&m_oGraphics,
			oSquareVF,
			XBufferFlags::StaticDraw | XBufferFlags::Discardable,
			sizeof( pSquareVertices ) / sizeof( SquareVertex ),
			&pSquareVertices );
		m_oSquare.GetVertexBufferSet().Add( &m_oSquareVertexBuffer );
		m_oSquare.GetIndexBuffer().Init(
			&m_oGraphics,
			XPrimitiveType::TriangleList,
			XIndexType::U8,
			XBufferFlags::StaticDraw | XBufferFlags::Discardable,
			sizeof( pSquareIndices ) / sizeof( u8 ),
			&pSquareIndices );

		m_mSquareMatrix.MakeTranslate( XVector4( 0, 1, 0 ) );
	}

	void MakeView()
	{
		m_mViewMatrix.MakeTranslate( XVector4( 0, 0, -m_fRadius ) );
		m_mViewMatrix.PreRotateX( m_fPhi );
		m_mViewMatrix.PreRotateY( m_fTheta );
	}

private:
	MyAllocator		m_oAllocator;
	XGraphics		m_oGraphics;
	XTexture		m_oTexture;
	XVertexBuffer	m_oAxesVertexBuffer;
	XVertexBuffer	m_oSquareVertexBuffer;
	XMesh			m_oAxes;
	XMesh			m_oSquare;
	XMatrix44		m_mViewMatrix;
	XMatrix44		m_mSquareMatrix;
	CPoint			m_oMousePos;
	int				m_nButtonMask;
	float			m_fTheta;
	float			m_fPhi;
	float			m_fRadius;
};

int main()
{
	XAllocator::InitModule();

	MyWindow oWindow;
	oWindow.Create( CDimensions( 800, 600 ) );
	int nReturnValue = oWindow.DoMessageLoop();
	oWindow.Destroy();

	XAllocator::DeinitModule();

	return nReturnValue;
}
