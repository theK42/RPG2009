#include "game/shared/gamepch.h"
#include "game/spacegame/parallaxcamera.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

CParallaxCamera::CParallaxCamera()
{
	m_bInitialized = false;
}

///----------------------------------------------------------------------------

CParallaxCamera::~CParallaxCamera()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CParallaxCamera::Init( SRectangle & rViewableArea )
{
	XASSERT(!m_bInitialized);
	m_oViewableArea = rViewableArea;
	m_vDeltaPos = XVector4(ZERO);
	m_bInitialized = true;
}

///----------------------------------------------------------------------------

void CParallaxCamera::Deinit()
{
	m_bInitialized = false;
}

///----------------------------------------------------------------------------

void CParallaxCamera::Update( float fTime, XVector4 const & rFrameChange )
{
	XASSERT(m_bInitialized);
	m_vDeltaPos = rFrameChange;
}

///----------------------------------------------------------------------------

void CParallaxCamera::GetFrameChange( XVector4 & rOutVector ) const
{
	rOutVector = m_vDeltaPos;
}

///----------------------------------------------------------------------------

SRectangle const & CParallaxCamera::GetViewableArea() const
{
	return m_oViewableArea;
}

///----------------------------------------------------------------------------
