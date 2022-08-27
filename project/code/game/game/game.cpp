#include "game/shared/gamepch.h"
#include "game/game/game.h"
#include "engine/graphics/xcolor.h"

#include "engine/resource/xfileio.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

void erpg::CGame::Init()
{
	XEngine::XAsyncFileDescriptorIO::InitModule();
	m_oGraphics.Init();
	m_oGraphics.SetClearColor( XEngine::XColor( 0.0f, 0.0f, 0.0f, 1.0f ) );
	m_oGraphics.EnableDepthTest( true );
	m_oGraphics.EnableFaceCulling( true );

	m_oMap.Load("testmap.dat");
	m_oMap.Init(&m_oAllocator, &m_oGraphics);

}

///------------------------------------------------------------------------

void erpg::CGame::Deinit()
{
	m_oMap.Deinit();
	m_oGraphics.Deinit();
	XEngine::XAsyncFileDescriptorIO::DeinitModule();
}

///------------------------------------------------------------------------

void erpg::CGame::Update( float fTime )
{
	m_oMap.Update(fTime);
}

///------------------------------------------------------------------------

void erpg::CGame::Render()
{
	m_oGraphics.ClearScreen( XEngine::XScreenClearTarget::Color | XEngine::XScreenClearTarget::Depth );
	m_oGraphics.PushRenderState();
	m_oGraphics.SetViewMatrix( m_oViewMatrix );

	m_oGraphics.SetModelMatrix( XEngine::IDENTITY );
	m_oGraphics.SetColor( XEngine::XColor( 1, 1, 1, 1 ) );

	m_oGraphics.SetLight(
		0,
		XEngine::XVector4( m_fLightX, m_fLightY, 10.0f, 1.0f ),
		XEngine::XColor( 1, 1, 1, 1 ),
		XEngine::XColor( 1, 1, 1, 1 ),
		XEngine::XColor( 1, 1, 1, 1 ),
		XEngine::XVector4( 1, 0, 0, 0 ) );
	m_oGraphics.EnableLight( 0, true );
	m_oGraphics.EnableLighting( true );

	m_oMap.Render(&m_oGraphics);
	m_oGraphics.PopRenderState();
}

///------------------------------------------------------------------------

void erpg::CGame::SetViewport( int nX0, int nY0, int nX1, int nY1 )
{
	float const fNear = 1.0f;
	float const fFar = 1000.0f;
	float const fFovy = PI / 6;	/// 30 degrees
	float fAspect = (float) (nX1 - nX0) / (float) (nY1 - nY0);

	XEngine::XMatrix44 mProjection;

	m_oGraphics.SetViewport( nX0, nY0, nX1, nY1 );
	//	m_oGraphics.SetProjectionMatrix( mProjection.MakePerspective( fNear, fFar, fFovy, fAspect ) );
	m_oGraphics.SetProjectionMatrix( mProjection.MakeOrthographic( 0.0f, 16.0f, 9.0f, 0.0f, 0.01f, 1000.0f) );
}

///------------------------------------------------------------------------

void erpg::CGame::SetViewMatrix( XEngine::XMatrix44 & mView )
{
	m_oViewMatrix = mView;
}

///------------------------------------------------------------------------

void erpg::CGame::SetLightPosition( float fX, float fY )
{
	m_fLightX = fX;
	m_fLightY = fY;
}

///------------------------------------------------------------------------
