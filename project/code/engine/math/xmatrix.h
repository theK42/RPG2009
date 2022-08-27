
#pragma once

#include "engine/math/xconstants.h"
#include "engine/math/xvector.h"

namespace XEngine
{
	class XVector4;
	class XMatrix33;
	class XQuaternion;

	class XMatrix44
	{
	public:
		XMatrix44();
		XMatrix44( XIdentity );

		float const *		GetMatrixPointer() const;

		XMatrix33 &			GetUpper3x3( XMatrix33 & rResult ) const;

	public:
		XMatrix44 &			MakeIdentity();

		XMatrix44 &			MakeRotateX( float fAngle );
		XMatrix44 &			MakeRotateY( float fAngle );
		XMatrix44 &			MakeRotateZ( float fAngle );
		XMatrix44 &			MakeRotate( XVector4 const & rAxis, float fAngle );
		XMatrix44 &			MakeRotate( XQuaternion const & rRotation );
		XMatrix44 &			MakeTranslate( XVector4 const & rDisplacement );
		XMatrix44 &			MakeScale( XVector4 const & rScaleVector );
		XMatrix44 &			MakeStretch( XVector4 const & rAxis, float fScale );

		/// Transforms "on the left"
		XMatrix44 &			PreRotateX( float fAngle );
		XMatrix44 &			PreRotateY( float fAngle );
		XMatrix44 &			PreRotateZ( float fAngle );
		XMatrix44 &			PreRotate( XVector4 const & rAxis, float fAngle );
		XMatrix44 &			PreTranslate( XVector4 const & rDisplacement );
		XMatrix44 &			PreScale( XVector4 const & rScaleVector );
		XMatrix44 &			PreStretch( XVector4 const & rVector, float fScale );

		/// Transforms "on the right"
		XMatrix44 &			PostRotateX( float fAngle );
		XMatrix44 &			PostRotateY( float fAngle );
		XMatrix44 &			PostRotateZ( float fAngle );
		XMatrix44 &			PostRotate( XVector4 const & rAxis, float fAngle );
		XMatrix44 &			PostTranslate( XVector4 const & rDisplacement );
		XMatrix44 &			PostScale( XVector4 const & rScaleVector );
		XMatrix44 &			PostStretch( XVector4 const & rVector, float fScale );

		/// Only call these if you know your axis is already normalized
		XMatrix44 &			MakeRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XMatrix44 &			PreRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XMatrix44 &			PostRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XMatrix44 &			MakeStretchPreNormalized( XVector4 const & rAxis, float fScale );
		XMatrix44 &			PreStretchPreNormalized( XVector4 const & rVector, float fScale );
		XMatrix44 &			PostStretchPreNormalized( XVector4 const & rVector, float fScale );

		/// Projection transforms
		XMatrix44 &			MakeOrthographic(
								float fLeft, float fRight,
								float fBottom, float fTop,
								float fNear, float fFar );
		XMatrix44 &			MakeFrustum(
								float fLeft, float fRight,
								float fBottom, float fTop,
								float fNear, float fFar );
		XMatrix44 &			MakePerspective(
								float fNear, float fFar,
								float fFovY, float fAspect );

	public:
		bool				operator ==( XMatrix44 const & rOther ) const;
		bool				operator !=( XMatrix44 const & rOther ) const;

		/// this = this * other
		XMatrix44 &			operator *=( XMatrix44 const & rOther );

		float				operator()( int i, int j ) const;
		float &				operator()( int i, int j );

	public:
		static XMatrix44 &	Multiply( XMatrix44 & rResult, XMatrix44 const & rLeft, XMatrix44 const & rRight );
		static XVector4 &	TransformVector( XVector4 & rResult, XVector4 const & rVector, XMatrix44 const & rMatrix );

		/// Only call these if rResult is unaliased
		static XMatrix44 &	MultiplyUnsafe( XMatrix44 & rResult, XMatrix44 const & rLeft, XMatrix44 const & rRight );
		static XVector4 &	TransformVectorUnsafe( XVector4 & rResult, XVector4 const & rVector, XMatrix44 const & rMatrix );

	private:
		XALIGNED(16) float	m_pMatrix[4][4];
	};

	class XMatrix33
	{
	public:
		XMatrix33();
		XMatrix33( XIdentity );

		XMatrix33 &			GetInverseTransposed( XMatrix33 & rResult ) const;

	public:
		XMatrix33 &			MakeIdentity();
		XMatrix33 &			MakeRotateX( float fAngle );
		XMatrix33 &			MakeRotateY( float fAngle );
		XMatrix33 &			MakeRotateZ( float fAngle );
		XMatrix33 &			MakeRotate( XVector4 const & rAxis, float fAngle );
		XMatrix33 &			MakeScale( XVector4 const & rScaleVector );
		XMatrix33 &			MakeStretch( XVector4 const & rAxis, float fScale );

		/// Transforms "on the left"
		XMatrix33 &			PreRotateX( float fAngle );
		XMatrix33 &			PreRotateY( float fAngle );
		XMatrix33 &			PreRotateZ( float fAngle );
		XMatrix33 &			PreRotate( XVector4 const & rAxis, float fAngle );
		XMatrix33 &			PreScale( XVector4 const & rScaleVector );
		XMatrix33 &			PreStretch( XVector4 const & rVector, float fScale );

		/// Transforms "on the right"
		XMatrix33 &			PostRotateX( float fAngle );
		XMatrix33 &			PostRotateY( float fAngle );
		XMatrix33 &			PostRotateZ( float fAngle );
		XMatrix33 &			PostRotate( XVector4 const & rAxis, float fAngle );
		XMatrix33 &			PostScale( XVector4 const & rScaleVector );
		XMatrix33 &			PostStretch( XVector4 const & rVector, float fScale );

		/// Only call these if you know your axis is already normalized
		XMatrix33 &			MakeRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XMatrix33 &			PreRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XMatrix33 &			PostRotatePreNormalized( XVector4 const & rAxis, float fAngle );
		XMatrix33 &			MakeStretchPreNormalized( XVector4 const & rAxis, float fScale );
		XMatrix33 &			PreStretchPreNormalized( XVector4 const & rVector, float fScale );
		XMatrix33 &			PostStretchPreNormalized( XVector4 const & rVector, float fScale );

	public:
		bool				operator ==( XMatrix33 const & rOther ) const;
		bool				operator !=( XMatrix33 const & rOther ) const;
		XMatrix33 &			operator *=( XMatrix33 const & rOther );

		float				operator()( int i, int j ) const;
		float &				operator()( int i, int j );

	public:
		static XMatrix33 &	Multiply( XMatrix33 & rResult, XMatrix33 const & rLeft, XMatrix33 const & rRight );
		static XVector4 &	TransformVector( XVector4 & rResult, XVector4 const & rVector, XMatrix33 const & rMatrix );

		/// Only call these if rResult is unaliased
		static XMatrix33 &	MultiplyUnsafe( XMatrix33 & rResult, XMatrix33 const & rLeft, XMatrix33 const & rRight );
		static XVector4 &	TransformVectorUnsafe( XVector4 & rResult, XVector4 const & rVector, XMatrix33 const & rMatrix );

	private:
		/// This is not a typo! There's an extra float in each row for alignment reasons.
		XALIGNED(16) float	m_pMatrix[3][4];
	};

}

inline XEngine::XMatrix44::XMatrix44()
{
}

inline XEngine::XMatrix44::XMatrix44( XIdentity )
{
	MakeIdentity();
}

inline float const * XEngine::XMatrix44::GetMatrixPointer() const
{
	return (float const *)m_pMatrix;
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::Multiply( XMatrix44 & rResult, XMatrix44 const & rLeft, XMatrix44 const & rRight )
{
	XMatrix44 tmp;

	return rResult = MultiplyUnsafe( tmp, rLeft, rRight );
}

inline XEngine::XVector4 & XEngine::XMatrix44::TransformVector( XVector4 & rResult, XVector4 const & rVector, XMatrix44 const & rMatrix )
{
	XVector4 tmp;

	return rResult = TransformVectorUnsafe( tmp, rVector, rMatrix );
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::MakeRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 tmp;

	return MakeRotatePreNormalized( rAxis.GetNormalizedXYZ( tmp ), fAngle );
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::PreRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 tmp;

	return PreRotatePreNormalized( rAxis.GetNormalizedXYZ( tmp ), fAngle );
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::PostRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 tmp;

	return PostRotatePreNormalized( rAxis.GetNormalizedXYZ( tmp ), fAngle );
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::MakeStretch( XVector4 const & rAxis, float fScale )
{
	XVector4 tmp;

	return MakeStretchPreNormalized( rAxis.GetNormalizedXYZ( tmp ), fScale );
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::PreStretch( XVector4 const & rAxis, float fScale )
{
	XVector4 tmp;

	return PreStretchPreNormalized( rAxis.GetNormalizedXYZ( tmp ), fScale );
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::PostStretch( XVector4 const & rAxis, float fScale )
{
	XVector4 tmp;

	return PostStretchPreNormalized( rAxis.GetNormalizedXYZ( tmp ), fScale );
}

inline bool XEngine::XMatrix44::operator !=( XMatrix44 const & rOther ) const
{
	return !operator ==( rOther );
}

inline XEngine::XMatrix44 & XEngine::XMatrix44::operator *=( XMatrix44 const & rOther )
{
	return Multiply( *this, *this, rOther );
}

inline float XEngine::XMatrix44::operator()( int i, int j ) const
{
	return m_pMatrix[i][j];
}

inline float & XEngine::XMatrix44::operator()( int i, int j )
{
	return m_pMatrix[i][j];
}

inline XEngine::XMatrix33::XMatrix33()
{
}

inline XEngine::XMatrix33::XMatrix33( XIdentity )
{
	MakeIdentity();
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::Multiply( XMatrix33 & rResult, XMatrix33 const & rLeft, XMatrix33 const & rRight )
{
	XMatrix33 tmp;

	return rResult = MultiplyUnsafe( tmp, rLeft, rRight );
}

inline XEngine::XVector4 & XEngine::XMatrix33::TransformVector( XVector4 & rResult, XVector4 const & rVector, XMatrix33 const & rMatrix )
{
	XVector4 tmp;

	return rResult = TransformVectorUnsafe( tmp, rVector, rMatrix );
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::MakeRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 tmp;

	return MakeRotatePreNormalized( rAxis.GetNormalizedXYZ( tmp ), fAngle );
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::PreRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 tmp;

	return PreRotatePreNormalized( rAxis.GetNormalizedXYZ( tmp ), fAngle );
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::PostRotate( XVector4 const & rAxis, float fAngle )
{
	XVector4 tmp;

	return PostRotatePreNormalized( rAxis.GetNormalizedXYZ( tmp ), fAngle );
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::MakeStretch( XVector4 const & rAxis, float fScale )
{
	XVector4 tmp;

	return MakeStretchPreNormalized( rAxis.GetNormalizedXYZ( tmp ), fScale );
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::PreStretch( XVector4 const & rAxis, float fScale )
{
	XVector4 tmp;

	return PreStretchPreNormalized( rAxis.GetNormalizedXYZ( tmp ), fScale );
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::PostStretch( XVector4 const & rAxis, float fScale )
{
	XVector4 tmp;

	return PostStretchPreNormalized( rAxis.GetNormalizedXYZ( tmp ), fScale );
}

inline bool XEngine::XMatrix33::operator !=( XMatrix33 const & rOther ) const
{
	return !operator ==( rOther );
}

inline XEngine::XMatrix33 & XEngine::XMatrix33::operator *=( XMatrix33 const & rOther )
{
	return Multiply( *this, *this, rOther );
}

inline float XEngine::XMatrix33::operator()( int i, int j ) const
{
	return m_pMatrix[i][j];
}

inline float & XEngine::XMatrix33::operator()( int i, int j )
{
	return m_pMatrix[i][j];
}

inline XEngine::XVector4 & operator *=( XEngine::XVector4 & rVector, XEngine::XMatrix44 const & rMatrix )
{
	return XEngine::XMatrix44::TransformVector( rVector, rVector, rMatrix );
}

inline XEngine::XVector4 & operator *=( XEngine::XVector4 & rVector, XEngine::XMatrix33 const & rMatrix )
{
	return XEngine::XMatrix33::TransformVector( rVector, rVector, rMatrix );
}
