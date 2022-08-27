
#include "engine/core/xcore.h"
#include "engine/math/xmatrix.h"
#include "engine/math/xvector.h"

#include <cmath>
using std::sinf;
using std::cosf;
using std::tanf;

XEngine::XMatrix44 & XEngine::XMatrix44::MakeIdentity()
{
	m_pMatrix[0][0] = 1.0f;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = 1.0f;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = 1.0f;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = 0.0f;
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix44::GetUpper3x3( XMatrix33 & rResult ) const
{
	rResult(0,0) = m_pMatrix[0][0];
	rResult(0,1) = m_pMatrix[0][1];
	rResult(0,2) = m_pMatrix[0][2];
	rResult(1,0) = m_pMatrix[1][0];
	rResult(1,1) = m_pMatrix[1][1];
	rResult(1,2) = m_pMatrix[1][2];
	rResult(2,0) = m_pMatrix[2][0];
	rResult(2,1) = m_pMatrix[2][1];
	rResult(2,2) = m_pMatrix[2][2];

	return rResult;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeRotateX( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	m_pMatrix[0][0] = 1.0f;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = c;
	m_pMatrix[1][2] = s;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = -s;
	m_pMatrix[2][2] = c;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = 0.0f;
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeRotateY( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	m_pMatrix[0][0] = c;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = -s;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = 1.0f;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = s;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = c;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = 0.0f;
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeRotateZ( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	m_pMatrix[0][0] = c;
	m_pMatrix[0][1] = s;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = -s;
	m_pMatrix[1][1] = c;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = 1.0f;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = 0.0f;
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );
	float const t = (1.0f - c);
	float const x = rAxis(0);
	float const y = rAxis(1);
	float const z = rAxis(2);
	float const xt = x*t;
	float const yt = y*t;
	float const zt = z*t;
	float const xs = x*s;
	float const ys = y*s;
	float const zs = z*s;
	float const xxt = xt*x;
	float const xyt = xt*y;
	float const xzt = xt*z;
	float const yyt = yt*y;
	float const yzt = yt*z;
	float const zzt = zt*t;

	m_pMatrix[0][0] = xxt + c;
	m_pMatrix[0][1] = xyt + zs;
	m_pMatrix[0][2] = xzt - ys;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = xyt - zs;
	m_pMatrix[1][1] = yyt + c;
	m_pMatrix[1][2] = yzt + xs;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = xzt + ys;
	m_pMatrix[2][1] = yzt - xs;
	m_pMatrix[2][2] = zzt + c;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = 0.0f;
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeTranslate( XVector4 const & rDisplacement )
{
	m_pMatrix[0][0] = 1.0f;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = 1.0f;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = 1.0f;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = rDisplacement(0);
	m_pMatrix[3][1] = rDisplacement(1);
	m_pMatrix[3][2] = rDisplacement(2);
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeScale( XVector4 const & rScaleVector )
{
	m_pMatrix[0][0] = rScaleVector(0);
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = rScaleVector(1);
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = rScaleVector(2);
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = 0.0f;
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeStretchPreNormalized( XVector4 const & rAxis, float fScale )
{
	float const t = (fScale - 1.0f);
	float const x = rAxis(0);
	float const y = rAxis(1);
	float const z = rAxis(2);
	float const xt = x * t;
	float const yt = y * t;
	float const zt = z * t;
	float const xxt = xt * x;
	float const xyt = xt * y;
	float const xzt = xt * z;
	float const yyt = yt * y;
	float const yzt = yt * z;
	float const zzt = zt * z;

	m_pMatrix[0][0] = xxt + 1.0f;
	m_pMatrix[0][1] = xyt;
	m_pMatrix[0][2] = xzt;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = xyt;
	m_pMatrix[1][1] = yyt + 1.0f;
	m_pMatrix[1][2] = yzt;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = xzt;
	m_pMatrix[2][1] = yzt;
	m_pMatrix[2][2] = zzt + 1.0f;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = 0.0f;
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PreRotateX( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[1][0];
	float const t1 = m_pMatrix[1][1];
	float const t2 = m_pMatrix[1][2];
	float const t3 = m_pMatrix[1][3];

	m_pMatrix[1][0] = c * t0 + s * m_pMatrix[2][0];
	m_pMatrix[1][1] = c * t1 + s * m_pMatrix[2][1];
	m_pMatrix[1][2] = c * t2 + s * m_pMatrix[2][2];
	m_pMatrix[1][3] = c * t3 + s * m_pMatrix[2][3];
	m_pMatrix[2][0] = -s * t0 + c * m_pMatrix[2][0];
	m_pMatrix[2][1] = -s * t1 + c * m_pMatrix[2][1];
	m_pMatrix[2][2] = -s * t2 + c * m_pMatrix[2][2];
	m_pMatrix[2][3] = -s * t3 + c * m_pMatrix[2][3];

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PreRotateY( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[0][1];
	float const t2 = m_pMatrix[0][2];
	float const t3 = m_pMatrix[0][3];

	m_pMatrix[0][0] = c * t0 - s * m_pMatrix[2][0];
	m_pMatrix[0][1] = c * t1 - s * m_pMatrix[2][1];
	m_pMatrix[0][2] = c * t2 - s * m_pMatrix[2][2];
	m_pMatrix[0][3] = c * t3 - s * m_pMatrix[2][3];
	m_pMatrix[2][0] = s * t0 + c * m_pMatrix[2][0];
	m_pMatrix[2][1] = s * t1 + c * m_pMatrix[2][1];
	m_pMatrix[2][2] = s * t2 + c * m_pMatrix[2][2];
	m_pMatrix[2][3] = s * t3 + c * m_pMatrix[2][3];

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PreRotateZ( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[0][1];
	float const t2 = m_pMatrix[0][2];
	float const t3 = m_pMatrix[0][3];

	m_pMatrix[0][0] = c * t0 + s * m_pMatrix[1][0];
	m_pMatrix[0][1] = c * t1 + s * m_pMatrix[1][1];
	m_pMatrix[0][2] = c * t2 + s * m_pMatrix[1][2];
	m_pMatrix[0][3] = c * t3 + s * m_pMatrix[1][3];
	m_pMatrix[1][0] = -s * t0 + c * m_pMatrix[1][0];
	m_pMatrix[1][1] = -s * t1 + c * m_pMatrix[1][1];
	m_pMatrix[1][2] = -s * t2 + c * m_pMatrix[1][2];
	m_pMatrix[1][3] = -s * t3 + c * m_pMatrix[1][3];

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PreRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	XMatrix44 tmp;

	return Multiply( *this, tmp.MakeRotatePreNormalized( rAxis, fAngle ), *this );
}

XEngine::XMatrix44 & XEngine::XMatrix44::PreTranslate( XVector4 const & rDisplacement )
{
	float const x = rDisplacement(0);
	float const y = rDisplacement(1);
	float const z = rDisplacement(2);

	m_pMatrix[3][0] += x * m_pMatrix[0][0] + y * m_pMatrix[1][0] + z * m_pMatrix[2][0];
	m_pMatrix[3][1] += x * m_pMatrix[0][1] + y * m_pMatrix[1][1] + z * m_pMatrix[2][1];
	m_pMatrix[3][2] += x * m_pMatrix[0][2] + y * m_pMatrix[1][2] + z * m_pMatrix[2][2];
	m_pMatrix[3][3] += x * m_pMatrix[0][3] + y * m_pMatrix[1][3] + z * m_pMatrix[2][3];

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PreStretchPreNormalized( XVector4 const & rAxis, float fScale )
{
	XMatrix44 tmp;

	return Multiply( *this, tmp.MakeStretchPreNormalized( rAxis, fScale ), *this );
}

XEngine::XMatrix44 & XEngine::XMatrix44::PostRotateX( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][1];
	float const t1 = m_pMatrix[1][1];
	float const t2 = m_pMatrix[2][1];
	float const t3 = m_pMatrix[3][1];

	m_pMatrix[0][1] = t0 * c - m_pMatrix[0][2] * s;
	m_pMatrix[1][1] = t1 * c - m_pMatrix[1][2] * s;
	m_pMatrix[2][1] = t2 * c - m_pMatrix[2][2] * s;
	m_pMatrix[3][1] = t3 * c - m_pMatrix[3][2] * s;
	m_pMatrix[0][2] = t0 * s + m_pMatrix[0][2] * c;
	m_pMatrix[1][2] = t1 * s + m_pMatrix[1][2] * c;
	m_pMatrix[2][2] = t2 * s + m_pMatrix[2][2] * c;
	m_pMatrix[3][2] = t3 * s + m_pMatrix[3][2] * c;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PostRotateY( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[1][0];
	float const t2 = m_pMatrix[2][0];
	float const t3 = m_pMatrix[3][0];

	m_pMatrix[0][0] = t0 * c + m_pMatrix[0][2] * s;
	m_pMatrix[1][0] = t1 * c + m_pMatrix[1][2] * s;
	m_pMatrix[2][0] = t2 * c + m_pMatrix[2][2] * s;
	m_pMatrix[3][0] = t3 * c + m_pMatrix[3][2] * s;
	m_pMatrix[0][2] = t0 * -s + m_pMatrix[0][2] * c;
	m_pMatrix[1][2] = t1 * -s + m_pMatrix[1][2] * c;
	m_pMatrix[2][2] = t2 * -s + m_pMatrix[2][2] * c;
	m_pMatrix[3][2] = t3 * -s + m_pMatrix[3][2] * c;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PostRotateZ( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[1][0];
	float const t2 = m_pMatrix[2][0];
	float const t3 = m_pMatrix[3][0];

	m_pMatrix[0][0] = t0 * c - m_pMatrix[0][1] * s;
	m_pMatrix[1][0] = t1 * c - m_pMatrix[1][1] * s;
	m_pMatrix[2][0] = t2 * c - m_pMatrix[2][1] * s;
	m_pMatrix[3][0] = t3 * c - m_pMatrix[3][1] * s;
	m_pMatrix[0][1] = t0 * s + m_pMatrix[0][1] * c;
	m_pMatrix[1][1] = t1 * s + m_pMatrix[1][1] * c;
	m_pMatrix[2][1] = t2 * s + m_pMatrix[2][1] * c;
	m_pMatrix[3][1] = t3 * s + m_pMatrix[3][1] * c;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PostRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	XMatrix44 tmp;

	return Multiply( *this, *this, tmp.MakeRotatePreNormalized( rAxis, fAngle ) );
}

XEngine::XMatrix44 & XEngine::XMatrix44::PostTranslate( XVector4 const & rDisplacement )
{
	float const x = rDisplacement(0);
	float const y = rDisplacement(1);
	float const z = rDisplacement(2);

	float const w0 = m_pMatrix[0][3];
	float const w1 = m_pMatrix[1][3];
	float const w2 = m_pMatrix[2][3];
	float const w3 = m_pMatrix[3][3];

	m_pMatrix[0][0] += x * w0;
	m_pMatrix[0][1] += y * w0;
	m_pMatrix[0][2] += z * w0;
	m_pMatrix[1][0] += x * w1;
	m_pMatrix[1][1] += y * w1;
	m_pMatrix[1][2] += z * w1;
	m_pMatrix[2][0] += x * w2;
	m_pMatrix[2][1] += y * w2;
	m_pMatrix[2][2] += z * w2;
	m_pMatrix[3][0] += x * w3;
	m_pMatrix[3][1] += y * w3;
	m_pMatrix[3][2] += z * w3;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::PostStretchPreNormalized( XVector4 const & rAxis, float fScale )
{
	XMatrix44 tmp;

	return Multiply( *this, *this, tmp.MakeStretchPreNormalized( rAxis, fScale ) );
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeOrthographic(
	float fLeft, float fRight,
	float fBottom, float fTop,
	float fNear, float fFar )
{
	float const rdx = 1.0f / (fRight - fLeft);
	float const rdy = 1.0f / (fTop - fBottom);
	float const rdz = 1.0f / (fFar - fNear);

	m_pMatrix[0][0] = rdx * 2.0f;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = rdy * 2.0f;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = rdz * -2.0f;
	m_pMatrix[2][3] = 0.0f;
	m_pMatrix[3][0] = rdx * -(fRight + fLeft);
	m_pMatrix[3][1] = rdy * -(fTop + fBottom);
	m_pMatrix[3][2] = rdz * -(fFar + fNear);
	m_pMatrix[3][3] = 1.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakeFrustum(
	float fLeft, float fRight,
	float fBottom, float fTop,
	float fNear, float fFar )
{
	float const rdx = 1.0f / (fRight - fLeft);
	float const rdy = 1.0f / (fTop - fBottom);
	float const rdz = 1.0f / (fFar - fNear);
	float const n2 = fNear * 2.0f;

	m_pMatrix[0][0] = rdx * n2;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[0][3] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = rdy * n2;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[1][3] = 0.0f;
	m_pMatrix[2][0] = rdx * (fRight + fLeft);
	m_pMatrix[2][1] = rdy * (fTop + fBottom);
	m_pMatrix[2][2] = rdz * -(fFar + fNear);
	m_pMatrix[2][3] = -1.0f;
	m_pMatrix[3][0] = 0.0f;
	m_pMatrix[3][1] = 0.0f;
	m_pMatrix[3][2] = rdz * n2 * -fFar;
	m_pMatrix[3][3] = 0.0f;

	return *this;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MakePerspective(
	float fNear, float fFar,
	float fFovY, float fAspect )
{
	float y = fNear * tanf( fFovY );
	float x = y * fAspect;

	return MakeFrustum( -x, x, -y, y, fNear, fFar );
}

bool XEngine::XMatrix44::operator ==( XMatrix44 const & rOther ) const
{
	if( XUNLIKELY( this == &rOther ) )
	{
		return true;
	}

	bool bEqual = true;

	bEqual &= m_pMatrix[0][0] == rOther.m_pMatrix[0][0];
	bEqual &= m_pMatrix[0][1] == rOther.m_pMatrix[0][1];
	bEqual &= m_pMatrix[0][2] == rOther.m_pMatrix[0][2];
	bEqual &= m_pMatrix[0][3] == rOther.m_pMatrix[0][3];
	bEqual &= m_pMatrix[1][0] == rOther.m_pMatrix[1][0];
	bEqual &= m_pMatrix[1][1] == rOther.m_pMatrix[1][1];
	bEqual &= m_pMatrix[1][2] == rOther.m_pMatrix[1][2];
	bEqual &= m_pMatrix[1][3] == rOther.m_pMatrix[1][3];
	bEqual &= m_pMatrix[2][0] == rOther.m_pMatrix[2][0];
	bEqual &= m_pMatrix[2][1] == rOther.m_pMatrix[2][1];
	bEqual &= m_pMatrix[2][2] == rOther.m_pMatrix[2][2];
	bEqual &= m_pMatrix[2][3] == rOther.m_pMatrix[2][3];
	bEqual &= m_pMatrix[3][0] == rOther.m_pMatrix[3][0];
	bEqual &= m_pMatrix[3][1] == rOther.m_pMatrix[3][1];
	bEqual &= m_pMatrix[3][2] == rOther.m_pMatrix[3][2];
	bEqual &= m_pMatrix[3][3] == rOther.m_pMatrix[3][3];

	return bEqual;
}

XEngine::XMatrix44 & XEngine::XMatrix44::MultiplyUnsafe( XMatrix44 & rResult, XMatrix44 const & rLeft, XMatrix44 const & rRight )
{
	rResult.m_pMatrix[0][0] = rLeft.m_pMatrix[0][0] * rRight.m_pMatrix[0][0] + rLeft.m_pMatrix[0][1] * rRight.m_pMatrix[1][0] + rLeft.m_pMatrix[0][2] * rRight.m_pMatrix[2][0] + rLeft.m_pMatrix[0][3] * rRight.m_pMatrix[3][0];
	rResult.m_pMatrix[0][1] = rLeft.m_pMatrix[0][0] * rRight.m_pMatrix[0][1] + rLeft.m_pMatrix[0][1] * rRight.m_pMatrix[1][1] + rLeft.m_pMatrix[0][2] * rRight.m_pMatrix[2][1] + rLeft.m_pMatrix[0][3] * rRight.m_pMatrix[3][1];
	rResult.m_pMatrix[0][2] = rLeft.m_pMatrix[0][0] * rRight.m_pMatrix[0][2] + rLeft.m_pMatrix[0][1] * rRight.m_pMatrix[1][2] + rLeft.m_pMatrix[0][2] * rRight.m_pMatrix[2][2] + rLeft.m_pMatrix[0][3] * rRight.m_pMatrix[3][2];
	rResult.m_pMatrix[0][3] = rLeft.m_pMatrix[0][0] * rRight.m_pMatrix[0][3] + rLeft.m_pMatrix[0][1] * rRight.m_pMatrix[1][3] + rLeft.m_pMatrix[0][2] * rRight.m_pMatrix[2][3] + rLeft.m_pMatrix[0][3] * rRight.m_pMatrix[3][3];
	rResult.m_pMatrix[1][0] = rLeft.m_pMatrix[1][0] * rRight.m_pMatrix[0][0] + rLeft.m_pMatrix[1][1] * rRight.m_pMatrix[1][0] + rLeft.m_pMatrix[1][2] * rRight.m_pMatrix[2][0] + rLeft.m_pMatrix[1][3] * rRight.m_pMatrix[3][0];
	rResult.m_pMatrix[1][1] = rLeft.m_pMatrix[1][0] * rRight.m_pMatrix[0][1] + rLeft.m_pMatrix[1][1] * rRight.m_pMatrix[1][1] + rLeft.m_pMatrix[1][2] * rRight.m_pMatrix[2][1] + rLeft.m_pMatrix[1][3] * rRight.m_pMatrix[3][1];
	rResult.m_pMatrix[1][2] = rLeft.m_pMatrix[1][0] * rRight.m_pMatrix[0][2] + rLeft.m_pMatrix[1][1] * rRight.m_pMatrix[1][2] + rLeft.m_pMatrix[1][2] * rRight.m_pMatrix[2][2] + rLeft.m_pMatrix[1][3] * rRight.m_pMatrix[3][2];
	rResult.m_pMatrix[1][3] = rLeft.m_pMatrix[1][0] * rRight.m_pMatrix[0][3] + rLeft.m_pMatrix[1][1] * rRight.m_pMatrix[1][3] + rLeft.m_pMatrix[1][2] * rRight.m_pMatrix[2][3] + rLeft.m_pMatrix[1][3] * rRight.m_pMatrix[3][3];
	rResult.m_pMatrix[2][0] = rLeft.m_pMatrix[2][0] * rRight.m_pMatrix[0][0] + rLeft.m_pMatrix[2][1] * rRight.m_pMatrix[1][0] + rLeft.m_pMatrix[2][2] * rRight.m_pMatrix[2][0] + rLeft.m_pMatrix[2][3] * rRight.m_pMatrix[3][0];
	rResult.m_pMatrix[2][1] = rLeft.m_pMatrix[2][0] * rRight.m_pMatrix[0][1] + rLeft.m_pMatrix[2][1] * rRight.m_pMatrix[1][1] + rLeft.m_pMatrix[2][2] * rRight.m_pMatrix[2][1] + rLeft.m_pMatrix[2][3] * rRight.m_pMatrix[3][1];
	rResult.m_pMatrix[2][2] = rLeft.m_pMatrix[2][0] * rRight.m_pMatrix[0][2] + rLeft.m_pMatrix[2][1] * rRight.m_pMatrix[1][2] + rLeft.m_pMatrix[2][2] * rRight.m_pMatrix[2][2] + rLeft.m_pMatrix[2][3] * rRight.m_pMatrix[3][2];
	rResult.m_pMatrix[2][3] = rLeft.m_pMatrix[2][0] * rRight.m_pMatrix[0][3] + rLeft.m_pMatrix[2][1] * rRight.m_pMatrix[1][3] + rLeft.m_pMatrix[2][2] * rRight.m_pMatrix[2][3] + rLeft.m_pMatrix[2][3] * rRight.m_pMatrix[3][3];
	rResult.m_pMatrix[3][0] = rLeft.m_pMatrix[3][0] * rRight.m_pMatrix[0][0] + rLeft.m_pMatrix[3][1] * rRight.m_pMatrix[1][0] + rLeft.m_pMatrix[3][2] * rRight.m_pMatrix[2][0] + rLeft.m_pMatrix[3][3] * rRight.m_pMatrix[3][0];
	rResult.m_pMatrix[3][1] = rLeft.m_pMatrix[3][0] * rRight.m_pMatrix[0][1] + rLeft.m_pMatrix[3][1] * rRight.m_pMatrix[1][1] + rLeft.m_pMatrix[3][2] * rRight.m_pMatrix[2][1] + rLeft.m_pMatrix[3][3] * rRight.m_pMatrix[3][1];
	rResult.m_pMatrix[3][2] = rLeft.m_pMatrix[3][0] * rRight.m_pMatrix[0][2] + rLeft.m_pMatrix[3][1] * rRight.m_pMatrix[1][2] + rLeft.m_pMatrix[3][2] * rRight.m_pMatrix[2][2] + rLeft.m_pMatrix[3][3] * rRight.m_pMatrix[3][2];
	rResult.m_pMatrix[3][3] = rLeft.m_pMatrix[3][0] * rRight.m_pMatrix[0][3] + rLeft.m_pMatrix[3][1] * rRight.m_pMatrix[1][3] + rLeft.m_pMatrix[3][2] * rRight.m_pMatrix[2][3] + rLeft.m_pMatrix[3][3] * rRight.m_pMatrix[3][3];

	return rResult;
}

XEngine::XVector4 & XEngine::XMatrix44::TransformVectorUnsafe( XVector4 & rResult, XVector4 const & rVector, XMatrix44 const & rMatrix )
{
	rResult(0) = rVector(0) * rMatrix.m_pMatrix[0][0] + rVector(1) * rMatrix.m_pMatrix[1][0] + rVector(2) * rMatrix.m_pMatrix[2][0] + rVector(3) * rMatrix.m_pMatrix[3][0];
	rResult(1) = rVector(0) * rMatrix.m_pMatrix[0][1] + rVector(1) * rMatrix.m_pMatrix[1][1] + rVector(2) * rMatrix.m_pMatrix[2][1] + rVector(3) * rMatrix.m_pMatrix[3][1];
	rResult(2) = rVector(0) * rMatrix.m_pMatrix[0][2] + rVector(1) * rMatrix.m_pMatrix[1][2] + rVector(2) * rMatrix.m_pMatrix[2][2] + rVector(3) * rMatrix.m_pMatrix[3][2];
	rResult(3) = rVector(0) * rMatrix.m_pMatrix[0][3] + rVector(1) * rMatrix.m_pMatrix[1][3] + rVector(2) * rMatrix.m_pMatrix[2][3] + rVector(3) * rMatrix.m_pMatrix[3][3];

	return rResult;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MakeIdentity()
{
	m_pMatrix[0][0] = 1.0f;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = 1.0f;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = 1.0f;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::GetInverseTransposed( XMatrix33 & rResult ) const
{
	XMatrix33 tmp;

	tmp.m_pMatrix[0][0] = m_pMatrix[1][1] * m_pMatrix[2][2] - m_pMatrix[2][1] * m_pMatrix[1][2];
	tmp.m_pMatrix[0][1] = m_pMatrix[2][0] * m_pMatrix[1][2] - m_pMatrix[1][0] * m_pMatrix[2][2];
	tmp.m_pMatrix[0][2] = m_pMatrix[1][0] * m_pMatrix[2][1] - m_pMatrix[2][0] * m_pMatrix[1][1];
	tmp.m_pMatrix[1][0] = m_pMatrix[2][1] * m_pMatrix[0][2] - m_pMatrix[0][1] * m_pMatrix[2][2];
	tmp.m_pMatrix[1][1] = m_pMatrix[0][0] * m_pMatrix[2][2] - m_pMatrix[2][0] * m_pMatrix[0][2];
	tmp.m_pMatrix[1][2] = m_pMatrix[2][0] * m_pMatrix[0][1] - m_pMatrix[0][0] * m_pMatrix[2][1];
	tmp.m_pMatrix[2][0] = m_pMatrix[0][1] * m_pMatrix[1][2] - m_pMatrix[1][0] * m_pMatrix[0][2];
	tmp.m_pMatrix[2][1] = m_pMatrix[1][0] * m_pMatrix[0][2] - m_pMatrix[0][0] * m_pMatrix[1][2];
	tmp.m_pMatrix[2][2] = m_pMatrix[0][0] * m_pMatrix[1][1] - m_pMatrix[1][0] * m_pMatrix[1][1];

	float frdet =
		m_pMatrix[0][0] * tmp.m_pMatrix[0][0] +
		m_pMatrix[0][1] * tmp.m_pMatrix[0][1] +
		m_pMatrix[0][2] * tmp.m_pMatrix[0][2];
	frdet = 1.0f / frdet;

	rResult.m_pMatrix[0][0] = tmp.m_pMatrix[0][0] * frdet;
	rResult.m_pMatrix[0][1] = tmp.m_pMatrix[0][1] * frdet;
	rResult.m_pMatrix[0][2] = tmp.m_pMatrix[0][2] * frdet;
	rResult.m_pMatrix[1][0] = tmp.m_pMatrix[1][0] * frdet;
	rResult.m_pMatrix[1][1] = tmp.m_pMatrix[1][1] * frdet;
	rResult.m_pMatrix[1][2] = tmp.m_pMatrix[1][2] * frdet;
	rResult.m_pMatrix[2][0] = tmp.m_pMatrix[2][0] * frdet;
	rResult.m_pMatrix[2][1] = tmp.m_pMatrix[2][1] * frdet;
	rResult.m_pMatrix[2][2] = tmp.m_pMatrix[2][2] * frdet;

	return rResult;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MakeRotateX( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	m_pMatrix[0][0] = 1.0f;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = c;
	m_pMatrix[1][2] = s;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = -s;
	m_pMatrix[2][2] = c;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MakeRotateY( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	m_pMatrix[0][0] = c;
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = -s;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = 1.0f;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[2][0] = s;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = c;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MakeRotateZ( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	m_pMatrix[0][0] = c;
	m_pMatrix[0][1] = s;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[1][0] = -s;
	m_pMatrix[1][1] = c;
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = 1.0f;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MakeRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );
	float const t = (1.0f - c);
	float const x = rAxis(0);
	float const y = rAxis(1);
	float const z = rAxis(2);
	float const xt = x*t;
	float const yt = y*t;
	float const zt = z*t;
	float const xs = x*s;
	float const ys = y*s;
	float const zs = z*s;
	float const xxt = xt*x;
	float const xyt = xt*y;
	float const xzt = xt*z;
	float const yyt = yt*y;
	float const yzt = yt*z;
	float const zzt = zt*t;

	m_pMatrix[0][0] = xxt + c;
	m_pMatrix[0][1] = xyt + zs;
	m_pMatrix[0][2] = xzt - ys;
	m_pMatrix[1][0] = xyt - zs;
	m_pMatrix[1][1] = yyt + c;
	m_pMatrix[1][2] = yzt + xs;
	m_pMatrix[2][0] = xzt + ys;
	m_pMatrix[2][1] = yzt - xs;
	m_pMatrix[2][2] = zzt + c;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MakeScale( XVector4 const & rScaleVector )
{
	m_pMatrix[0][0] = rScaleVector(0);
	m_pMatrix[0][1] = 0.0f;
	m_pMatrix[0][2] = 0.0f;
	m_pMatrix[1][0] = 0.0f;
	m_pMatrix[1][1] = rScaleVector(1);
	m_pMatrix[1][2] = 0.0f;
	m_pMatrix[2][0] = 0.0f;
	m_pMatrix[2][1] = 0.0f;
	m_pMatrix[2][2] = rScaleVector(2);

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MakeStretchPreNormalized( XVector4 const & rAxis, float fScale )
{
	float const t = (fScale - 1.0f);
	float const x = rAxis(0);
	float const y = rAxis(1);
	float const z = rAxis(2);
	float const xt = x * t;
	float const yt = y * t;
	float const zt = z * t;
	float const xxt = xt * x;
	float const xyt = xt * y;
	float const xzt = xt * z;
	float const yyt = yt * y;
	float const yzt = yt * z;
	float const zzt = zt * z;

	m_pMatrix[0][0] = xxt + 1.0f;
	m_pMatrix[0][1] = xyt;
	m_pMatrix[0][2] = xzt;
	m_pMatrix[1][0] = xyt;
	m_pMatrix[1][1] = yyt + 1.0f;
	m_pMatrix[1][2] = yzt;
	m_pMatrix[2][0] = xzt;
	m_pMatrix[2][1] = yzt;
	m_pMatrix[2][2] = zzt + 1.0f;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::PreRotateX( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[1][0];
	float const t1 = m_pMatrix[1][1];
	float const t2 = m_pMatrix[1][2];

	m_pMatrix[1][0] = c * t0 + s * m_pMatrix[2][0];
	m_pMatrix[1][1] = c * t1 + s * m_pMatrix[2][1];
	m_pMatrix[1][2] = c * t2 + s * m_pMatrix[2][2];
	m_pMatrix[2][0] = -s * t0 + c * m_pMatrix[2][0];
	m_pMatrix[2][1] = -s * t1 + c * m_pMatrix[2][1];
	m_pMatrix[2][2] = -s * t2 + c * m_pMatrix[2][2];

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::PreRotateY( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[0][1];
	float const t2 = m_pMatrix[0][2];

	m_pMatrix[0][0] = c * t0 - s * m_pMatrix[2][0];
	m_pMatrix[0][1] = c * t1 - s * m_pMatrix[2][1];
	m_pMatrix[0][2] = c * t2 - s * m_pMatrix[2][2];
	m_pMatrix[2][0] = s * t0 + c * m_pMatrix[2][0];
	m_pMatrix[2][1] = s * t1 + c * m_pMatrix[2][1];
	m_pMatrix[2][2] = s * t2 + c * m_pMatrix[2][2];

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::PreRotateZ( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[0][1];
	float const t2 = m_pMatrix[0][2];

	m_pMatrix[0][0] = c * t0 + s * m_pMatrix[1][0];
	m_pMatrix[0][1] = c * t1 + s * m_pMatrix[1][1];
	m_pMatrix[0][2] = c * t2 + s * m_pMatrix[1][2];
	m_pMatrix[1][0] = -s * t0 + c * m_pMatrix[1][0];
	m_pMatrix[1][1] = -s * t1 + c * m_pMatrix[1][1];
	m_pMatrix[1][2] = -s * t2 + c * m_pMatrix[1][2];

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::PreRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	XMatrix33 tmp;

	return Multiply( *this, tmp.MakeRotatePreNormalized( rAxis, fAngle ), *this );
}

XEngine::XMatrix33 & XEngine::XMatrix33::PreStretchPreNormalized( XVector4 const & rAxis, float fScale )
{
	XMatrix33 tmp;

	return Multiply( *this, tmp.MakeStretchPreNormalized( rAxis, fScale ), *this );
}

XEngine::XMatrix33 & XEngine::XMatrix33::PostRotateX( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][1];
	float const t1 = m_pMatrix[1][1];
	float const t2 = m_pMatrix[2][1];

	m_pMatrix[0][1] = t0 * c - m_pMatrix[0][2] * s;
	m_pMatrix[1][1] = t1 * c - m_pMatrix[1][2] * s;
	m_pMatrix[2][1] = t2 * c - m_pMatrix[2][2] * s;
	m_pMatrix[0][2] = t0 * s + m_pMatrix[0][2] * c;
	m_pMatrix[1][2] = t1 * s + m_pMatrix[1][2] * c;
	m_pMatrix[2][2] = t2 * s + m_pMatrix[2][2] * c;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::PostRotateY( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[1][0];
	float const t2 = m_pMatrix[2][0];

	m_pMatrix[0][0] = t0 * c + m_pMatrix[0][2] * s;
	m_pMatrix[1][0] = t1 * c + m_pMatrix[1][2] * s;
	m_pMatrix[2][0] = t2 * c + m_pMatrix[2][2] * s;
	m_pMatrix[0][2] = t0 * -s + m_pMatrix[0][2] * c;
	m_pMatrix[1][2] = t1 * -s + m_pMatrix[1][2] * c;
	m_pMatrix[2][2] = t2 * -s + m_pMatrix[2][2] * c;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::PostRotateZ( float fAngle )
{
	float const s = sinf( fAngle );
	float const c = cosf( fAngle );

	float const t0 = m_pMatrix[0][0];
	float const t1 = m_pMatrix[1][0];
	float const t2 = m_pMatrix[2][0];

	m_pMatrix[0][0] = t0 * c - m_pMatrix[0][1] * s;
	m_pMatrix[1][0] = t1 * c - m_pMatrix[1][1] * s;
	m_pMatrix[2][0] = t2 * c - m_pMatrix[2][1] * s;
	m_pMatrix[0][1] = t0 * s + m_pMatrix[0][1] * c;
	m_pMatrix[1][1] = t1 * s + m_pMatrix[1][1] * c;
	m_pMatrix[2][1] = t2 * s + m_pMatrix[2][1] * c;

	return *this;
}

XEngine::XMatrix33 & XEngine::XMatrix33::PostRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	XMatrix33 tmp;

	return Multiply( *this, *this, tmp.MakeRotatePreNormalized( rAxis, fAngle ) );
}

XEngine::XMatrix33 & XEngine::XMatrix33::PostStretchPreNormalized( XVector4 const & rAxis, float fScale )
{
	XMatrix33 tmp;

	return Multiply( *this, *this, tmp.MakeStretchPreNormalized( rAxis, fScale ) );
}

bool XEngine::XMatrix33::operator ==( XMatrix33 const & rOther ) const
{
	if( XUNLIKELY( this == &rOther ) )
	{
		return true;
	}

	bool bEqual = true;
	int i = 0;

	bEqual &= m_pMatrix[0][0] == rOther.m_pMatrix[0][0];
	bEqual &= m_pMatrix[0][1] == rOther.m_pMatrix[0][1];
	bEqual &= m_pMatrix[0][2] == rOther.m_pMatrix[0][2];
	bEqual &= m_pMatrix[1][0] == rOther.m_pMatrix[1][0];
	bEqual &= m_pMatrix[1][1] == rOther.m_pMatrix[1][1];
	bEqual &= m_pMatrix[1][2] == rOther.m_pMatrix[1][2];
	bEqual &= m_pMatrix[2][0] == rOther.m_pMatrix[2][0];
	bEqual &= m_pMatrix[2][1] == rOther.m_pMatrix[2][1];
	bEqual &= m_pMatrix[2][2] == rOther.m_pMatrix[2][2];

	return bEqual;
}

XEngine::XMatrix33 & XEngine::XMatrix33::MultiplyUnsafe( XMatrix33 & rResult, XMatrix33 const & rLeft, XMatrix33 const & rRight )
{
	rResult.m_pMatrix[0][0] = rLeft.m_pMatrix[0][0] * rRight.m_pMatrix[0][0] + rLeft.m_pMatrix[0][1] * rRight.m_pMatrix[1][0] + rLeft.m_pMatrix[0][2] * rRight.m_pMatrix[2][0];
	rResult.m_pMatrix[0][1] = rLeft.m_pMatrix[0][0] * rRight.m_pMatrix[0][1] + rLeft.m_pMatrix[0][1] * rRight.m_pMatrix[1][1] + rLeft.m_pMatrix[0][2] * rRight.m_pMatrix[2][1];
	rResult.m_pMatrix[0][2] = rLeft.m_pMatrix[0][0] * rRight.m_pMatrix[0][2] + rLeft.m_pMatrix[0][1] * rRight.m_pMatrix[1][2] + rLeft.m_pMatrix[0][2] * rRight.m_pMatrix[2][2];
	rResult.m_pMatrix[1][0] = rLeft.m_pMatrix[1][0] * rRight.m_pMatrix[0][0] + rLeft.m_pMatrix[1][1] * rRight.m_pMatrix[1][0] + rLeft.m_pMatrix[1][2] * rRight.m_pMatrix[2][0];
	rResult.m_pMatrix[1][1] = rLeft.m_pMatrix[1][0] * rRight.m_pMatrix[0][1] + rLeft.m_pMatrix[1][1] * rRight.m_pMatrix[1][1] + rLeft.m_pMatrix[1][2] * rRight.m_pMatrix[2][1];
	rResult.m_pMatrix[1][2] = rLeft.m_pMatrix[1][0] * rRight.m_pMatrix[0][2] + rLeft.m_pMatrix[1][1] * rRight.m_pMatrix[1][2] + rLeft.m_pMatrix[1][2] * rRight.m_pMatrix[2][2];
	rResult.m_pMatrix[2][0] = rLeft.m_pMatrix[2][0] * rRight.m_pMatrix[0][0] + rLeft.m_pMatrix[2][1] * rRight.m_pMatrix[1][0] + rLeft.m_pMatrix[2][2] * rRight.m_pMatrix[2][0];
	rResult.m_pMatrix[2][1] = rLeft.m_pMatrix[2][0] * rRight.m_pMatrix[0][1] + rLeft.m_pMatrix[2][1] * rRight.m_pMatrix[1][1] + rLeft.m_pMatrix[2][2] * rRight.m_pMatrix[2][1];
	rResult.m_pMatrix[2][2] = rLeft.m_pMatrix[2][0] * rRight.m_pMatrix[0][2] + rLeft.m_pMatrix[2][1] * rRight.m_pMatrix[1][2] + rLeft.m_pMatrix[2][2] * rRight.m_pMatrix[2][2];

	return rResult;
}

XEngine::XVector4 & XEngine::XMatrix33::TransformVectorUnsafe( XVector4 & rResult, XVector4 const & rVector, XMatrix33 const & rMatrix )
{
	rResult(0) = rVector(0) * rMatrix.m_pMatrix[0][0] + rVector(1) * rMatrix.m_pMatrix[1][0] + rVector(2) * rMatrix.m_pMatrix[2][0];
	rResult(1) = rVector(0) * rMatrix.m_pMatrix[0][1] + rVector(1) * rMatrix.m_pMatrix[1][1] + rVector(2) * rMatrix.m_pMatrix[2][1];
	rResult(2) = rVector(0) * rMatrix.m_pMatrix[0][2] + rVector(1) * rMatrix.m_pMatrix[1][2] + rVector(2) * rMatrix.m_pMatrix[2][2];

	return rResult;
}
