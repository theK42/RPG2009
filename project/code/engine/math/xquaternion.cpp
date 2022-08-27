
#include "engine/core/xcore.h"
#include "engine/math/xquaternion.h"

#include <cmath>
using std::sinf;
using std::cosf;

XEngine::XQuaternion & XEngine::XQuaternion::MakeRotateX( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	m_pVector[0] = s;
	m_pVector[1] = 0.0f;
	m_pVector[2] = 0.0f;
	m_pVector[3] = c;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::MakeRotateY( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	m_pVector[0] = 0.0f;
	m_pVector[1] = s;
	m_pVector[2] = 0.0f;
	m_pVector[3] = c;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::MakeRotateZ( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	m_pVector[0] = 0.0f;
	m_pVector[1] = 0.0f;
	m_pVector[2] = s;
	m_pVector[3] = c;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::MakeRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	m_pVector[0] = s * rAxis(0);
	m_pVector[1] = s * rAxis(1);
	m_pVector[2] = s * rAxis(2);
	m_pVector[3] = c;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::PreRotateX( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	float const x = m_pVector[0];
	float const y = m_pVector[1];
	float const z = m_pVector[2];
	float const w = m_pVector[3];

	m_pVector[0] = c*x + s*w;
	m_pVector[1] = c*y + s*z;
	m_pVector[2] = c*z - s*y;
	m_pVector[3] = c*w - s*x;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::PreRotateY( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	float const x = m_pVector[0];
	float const y = m_pVector[1];
	float const z = m_pVector[2];
	float const w = m_pVector[3];

	m_pVector[0] = c*x - s*z;
	m_pVector[1] = c*y + s*w;
	m_pVector[2] = c*z + s*x;
	m_pVector[3] = c*w - s*y;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::PreRotateZ( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	float const x = m_pVector[0];
	float const y = m_pVector[1];
	float const z = m_pVector[2];
	float const w = m_pVector[3];

	m_pVector[0] = c*x + s*y;
	m_pVector[1] = c*y - s*x;
	m_pVector[2] = c*z + s*w;
	m_pVector[3] = c*w - s*z;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::PostRotateX( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	float const x = m_pVector[0];
	float const y = m_pVector[1];
	float const z = m_pVector[2];
	float const w = m_pVector[3];

	m_pVector[0] = c*x + s*w;
	m_pVector[1] = c*y - s*z;
	m_pVector[2] = c*z + s*y;
	m_pVector[3] = c*w - s*x;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::PostRotateY( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	float const x = m_pVector[0];
	float const y = m_pVector[1];
	float const z = m_pVector[2];
	float const w = m_pVector[3];

	m_pVector[0] = c*x + s*z;
	m_pVector[1] = c*y + s*w;
	m_pVector[2] = c*z - s*x;
	m_pVector[3] = c*w - s*y;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::PostRotateZ( float fAngle )
{
	float const fHalfAngle = fAngle * 0.5f;
	float const c = cosf( fHalfAngle );
	float const s = sinf( fHalfAngle );

	float const x = m_pVector[0];
	float const y = m_pVector[1];
	float const z = m_pVector[2];
	float const w = m_pVector[3];

	m_pVector[0] = c*x - s*y;
	m_pVector[1] = c*y + s*x;
	m_pVector[2] = c*z + s*w;
	m_pVector[3] = c*w - s*z;

	return (*this);
}

XEngine::XQuaternion & XEngine::XQuaternion::GetConjugate( XQuaternion & rResult ) const
{
	rResult.m_pVector[0] = -m_pVector[0];
	rResult.m_pVector[1] = -m_pVector[1];
	rResult.m_pVector[2] = -m_pVector[2];
	rResult.m_pVector[3] = +m_pVector[3];

	return rResult;
}

XEngine::XQuaternion & XEngine::XQuaternion::Multiply( XQuaternion & rResult, XQuaternion const & rLeft, XQuaternion const & rRight )
{
	float const x = rLeft.m_pVector[0];
	float const y = rLeft.m_pVector[1];
	float const z = rLeft.m_pVector[2];
	float const w = rLeft.m_pVector[3];

	float const a = rRight.m_pVector[0];
	float const b = rRight.m_pVector[1];
	float const c = rRight.m_pVector[2];
	float const d = rRight.m_pVector[3];

	rResult.m_pVector[0] = + x*d + y*c - z*b + w*a;
	rResult.m_pVector[1] = - x*c + y*d + z*a + w*b;
	rResult.m_pVector[2] = + x*b - y*a + z*d + w*c;
	rResult.m_pVector[3] = - x*a - y*b - z*c + w*d;

	return rResult;
}

XEngine::XVector4 & XEngine::XQuaternion::TransformVector( XVector4 & rResult, XQuaternion const & rQuaternion, XVector4 const & rVector )
{
	XQuaternion qTemp;
	return rResult = Multiply( qTemp, rQuaternion, Multiply( qTemp, rVector, rQuaternion.GetConjugate( qTemp ) ) );
}
