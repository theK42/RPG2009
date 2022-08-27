
#pragma once

#include "engine/math/xbasevector.h"
#include "engine/math/xconstants.h"
#include "engine/math/xvector.h"

namespace XEngine
{
	class XQuaternion : public XBaseVector4
	{
	public:
		XQuaternion();
		XQuaternion( XBaseVector4 const & rOther );
		XQuaternion( XIdentity );

	public:
		XQuaternion &	MakeRotateX( float fAngle );
		XQuaternion &	MakeRotateY( float fAngle );
		XQuaternion &	MakeRotateZ( float fAngle );
		XQuaternion &	MakeRotate( XVector4 const & rAxis, float fAngle );

		/// Rotations "on the right"
		XQuaternion &	PreRotateX( float fAngle );
		XQuaternion &	PreRotateY( float fAngle );
		XQuaternion &	PreRotateZ( float fAngle );
		XQuaternion &	PreRotate( XVector4 const & rAxis, float fAngle );

		/// Rotations "on the left"
		XQuaternion &	PostRotateX( float fAngle );
		XQuaternion &	PostRotateY( float fAngle );
		XQuaternion &	PostRotateZ( float fAngle );
		XQuaternion &	PostRotate( XVector4 const & rAxis, float fAngle );

		XQuaternion &	MakeRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XQuaternion &	PreRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XQuaternion &	PostRotatePreNormalized( XVector4 const & rAxis, float fAngle );

	public:
		XQuaternion &	GetConjugate( XQuaternion & rResult ) const;

	public:
		/// this = this * rOther
		XQuaternion &	operator *=( XQuaternion const & rOther );

	public:
		static XQuaternion &	Multiply( XQuaternion & rResult, XQuaternion const & rLeft, XQuaternion const & rRight );
		static XVector4 &		TransformVector( XVector4 & rResult, XQuaternion const & rQuaternion, XVector4 const & rVector );
	};

} /// namespace XEngine

inline XEngine::XQuaternion::XQuaternion()
{
}

inline XEngine::XQuaternion::XQuaternion( XBaseVector4 const & rOther )
: XBaseVector4( rOther )
{
}

inline XEngine::XQuaternion::XQuaternion( XIdentity )
{
	m_pVector[0] = 0.0f;
	m_pVector[1] = 0.0f;
	m_pVector[2] = 0.0f;
	m_pVector[3] = 1.0f;
}

inline XEngine::XQuaternion & XEngine::XQuaternion::PreRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	XQuaternion qTemp;
	return Multiply( *this, *this, qTemp.MakeRotatePreNormalized( rAxis, fAngle ) );
}

inline XEngine::XQuaternion & XEngine::XQuaternion::PostRotatePreNormalized( XVector4 const & rAxis, float fAngle )
{
	XQuaternion qTemp;
	return Multiply( *this, qTemp.MakeRotatePreNormalized( rAxis, fAngle ), *this );
}

inline XEngine::XQuaternion & XEngine::XQuaternion::MakeRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 vTemp;
	return MakeRotatePreNormalized( rAxis.GetNormalizedXYZ( vTemp ), fAngle );
}

inline XEngine::XQuaternion & XEngine::XQuaternion::PreRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 vTemp;
	return PreRotatePreNormalized( rAxis.GetNormalizedXYZ( vTemp ), fAngle );
}

inline XEngine::XQuaternion & XEngine::XQuaternion::PostRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 vTemp;
	return PostRotatePreNormalized( rAxis.GetNormalizedXYZ( vTemp ), fAngle );
}

inline XEngine::XQuaternion & XEngine::XQuaternion::operator *=( XQuaternion const & rOther )
{
	return Multiply( *this, *this, rOther );
}
