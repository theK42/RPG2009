
#include "engine/core/xcore.h"
#include "engine/graphics/xcolor.h"
#include "engine/math/xvector.h"
#include "engine/algorithms/xutil.h"

void XEngine::XColor::Set( float r, float g, float b, float a )
{
	r = Clamp( r, 0.0f, 1.0f );
	g = Clamp( g, 0.0f, 1.0f );
	b = Clamp( b, 0.0f, 1.0f );
	a = Clamp( a, 0.0f, 1.0f );

	u32 nr = (u32)(r * 255.0f);
	u32 ng = (u32)(g * 255.0f);
	u32 nb = (u32)(b * 255.0f);
	u32 na = (u32)(a * 255.0f);

	m_nPackedColor = (nr << RED_SHIFT) | (ng << GREEN_SHIFT) | (nb << BLUE_SHIFT) | (na << ALPHA_SHIFT);
}

XEngine::XVector4 & XEngine::XColor::GetAsVector( XVector4 & rResult ) const
{
	u32 r = 0xFF & (m_nPackedColor >> RED_SHIFT);
	u32 g = 0xFF & (m_nPackedColor >> GREEN_SHIFT);
	u32 b = 0xFF & (m_nPackedColor >> BLUE_SHIFT);
	u32 a = 0xFF & (m_nPackedColor >> ALPHA_SHIFT);

	rResult(0) = r / 255.0f;
	rResult(1) = g / 255.0f;
	rResult(2) = b / 255.0f;
	rResult(3) = a / 255.0f;

	return rResult;
}
