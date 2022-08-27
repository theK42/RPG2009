#include "game/shared/gamepch.h"
#include "engine/core/xcore.h"
#include "engine/algorithms/xutil.h"
#include "engine/core/xmswin.h"
#include "engine/math/xmatrix.h"
#include "engine/resource/xfileio.h"

#include "game/spacegame/spacegame.h"



#include "windows/core/window.h"

#include <cmath>
#include <cstdlib>
#include <ctime> 
#include <stdio.h>

using namespace XEngine;
using namespace SpaceGame;
using namespace Windows;
using namespace std;


class GameWindow : public Windows::CWindow
{
public:
	GameWindow()
	{
		m_nButtonMask	= 0;
		m_fTheta		= -PI/6;
		m_fPhi			= PI/6;
		m_fCameraX		= 0.0f;
		m_fCameraY		= 0.0f;
		m_fLightX		= 0.0f;
		m_fLightY		= 0.0f;
		m_fRadius		= 10;
	}

protected:
	virtual void OnCreate()
	{
		m_oGame.Init();
	}

	virtual void OnDestroy()
	{
		m_oGame.Deinit();
	}

	virtual void OnUpdate( float fDT )
	{
		m_oGame.Update(fDT);
	}

	virtual void OnRender()
	{
		m_oGame.Render();
	}

	virtual void OnResize( CDimensions const & rDimensions )
	{
		int w = rDimensions.GetWidth();
		int h = rDimensions.GetHeight();
		m_oGame.SetViewport(0, 0, w, h);
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


// 		float const THETA_PER_PIXEL = 1.0f * PI / 180.0f;
// 		float const PHI_PER_PIXEL = 1.0f * PI / 180.0f;
// 		float const POSITION_PER_PIXEL = 0.01f;
// 		float const LIGHT_POSITION_PER_PIXEL = 0.02f;
// 
// 		switch( m_nButtonMask )
// 		{
// 		case 1:
// 			m_fTheta = fmodf( m_fTheta + dx * THETA_PER_PIXEL, 2*PI );
// 			m_fPhi = Clamp( m_fPhi + dy * PHI_PER_PIXEL, -PI/2, PI/2 );
// 			break;
// 		case 2:
// 			m_fCameraX += dx * POSITION_PER_PIXEL;
// 			m_fCameraY -= dy * POSITION_PER_PIXEL;
// 			break;
// 		case 3:
// 			m_fLightX += dx * LIGHT_POSITION_PER_PIXEL;
// 			m_fLightY -= dy * LIGHT_POSITION_PER_PIXEL;
// 			break;
// 		}
// 
// 
 		m_oMousePos = rPoint;	
// 		
// 		XMatrix44 mViewMatrix;
// 		mViewMatrix.MakeTranslate( XVector4( m_fCameraX, m_fCameraY, -m_fRadius ) );
// 		mViewMatrix.PreRotateX( m_fPhi );
// 		mViewMatrix.PreRotateY( m_fTheta );
// 		m_oGame.SetViewMatrix(mViewMatrix);

	}

private:



private:
	CKRBSG			m_oGame;
	XMatrix44		m_mViewMatrix;
	CPoint			m_oMousePos;
	int				m_nButtonMask;
	float			m_fTheta;
	float			m_fPhi;
	float			m_fCameraX;
	float			m_fCameraY;
	float			m_fLightX;
	float			m_fLightY;
	float			m_fRadius;
};




int main()
{
	///Create the Game Window
	GameWindow oWindow;
	oWindow.Create( CDimensions( 800, 450 ) ); ///16:9, the widest widescreen we support
	///Android will likely be 5/3 and iPhone 3/2, standard def is 4:3  I plan to draw to a 16:9 area and treat 3/2 as safe frame.
	int nReturnValue = oWindow.DoMessageLoop();
	oWindow.Destroy();
	return nReturnValue;
}
