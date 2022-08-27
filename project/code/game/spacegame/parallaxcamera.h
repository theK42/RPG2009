#pragma once

#include "game/basegame/rectangle.h"

class CParallaxCamera
{
public:
	CParallaxCamera();
	~CParallaxCamera();

	void Init(SRectangle & rViewableArea);
	void Deinit();

	virtual void Update(float fTime, XVector4 const & rFrameChange);
	virtual void GetFrameChange(XVector4 & rOutVector) const;
	virtual SRectangle const & GetViewableArea() const;

private:
	bool		m_bInitialized;
	XVector4	m_vDeltaPos;
	SRectangle	m_oViewableArea;

};