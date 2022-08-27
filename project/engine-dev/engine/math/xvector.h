
#pragma once

#include "engine/math/xbasevector.h"
#include "engine/math/xconstants.h"

namespace XEngine
{

	class XVector4 : public XBaseVector4
	{
	public:
		XVector4();
		XVector4( XBaseVector4 const & rOther );
		XVector4( float x, float y, float z );
		XVector4( float x, float y, float z, float w );

		XVector4( XZero );
		XVector4( XUnitX );
		XVector4( XUnitY );
		XVector4( XUnitZ );
		XVector4( XUnitW );

	public:
		XVector4 &	MakeZero();
		XVector4 &	MakeUnitX();
		XVector4 &	MakeUnitY();
		XVector4 &	MakeUnitZ();
		XVector4 &	MakeUnitW();

	public:
		XVector4 &	GetNormalizedXYZ( XVector4 & rResult ) const;

		float const *	GetVectorPointer() const;
		float *			GetVectorPointer();

	public:
		bool		operator ==( XVector4 const & rOther ) const;
		bool		operator !=( XVector4 const & rOther ) const;

		float		operator()( int i ) const;
		float &		operator()( int i );

		XVector4 &	operator +=( XVector4 const & rOther );
		XVector4 &	operator -=( XVector4 const & rOther );
		XVector4 &	operator *=( XVector4 const & rOther );
		XVector4 &	operator /=( XVector4 const & rOther );
		XVector4 &	operator *=( float s );
		XVector4 &	operator /=( float s );

		static float		GetDotProduct( XVector4 const & rLeft, XVector4 const & rRight);
		static float		GetDotProductXYZ( XVector4 const & rLeft, XVector4 const & rRight);
		static XVector4 &	GetCrossProduct( XVector4 & rResult, XVector4 const & rLeft, XVector4 const & rRight);
	};

} /// namespace XEngine

inline XEngine::XVector4::XVector4()
{
}

inline XEngine::XVector4::XVector4( XBaseVector4 const & rOther )
: XBaseVector4( rOther )
{
}

inline XEngine::XVector4::XVector4( float x, float y, float z )
{
	m_pVector[0] = x;
	m_pVector[1] = y;
	m_pVector[2] = z;
}

inline XEngine::XVector4::XVector4( float x, float y, float z, float w )
{
	m_pVector[0] = x;
	m_pVector[1] = y;
	m_pVector[2] = z;
	m_pVector[3] = w;
}

inline XEngine::XVector4 & XEngine::XVector4::MakeZero()
{
	m_pVector[0] = 0.0f;
	m_pVector[1] = 0.0f;
	m_pVector[2] = 0.0f;
	m_pVector[3] = 0.0f;

	return *this;
}

inline XEngine::XVector4 & XEngine::XVector4::MakeUnitX()
{
	m_pVector[0] = 1.0f;
	m_pVector[1] = 0.0f;
	m_pVector[2] = 0.0f;
	m_pVector[3] = 0.0f;

	return *this;
}

inline XEngine::XVector4 & XEngine::XVector4::MakeUnitY()
{
	m_pVector[0] = 0.0f;
	m_pVector[1] = 1.0f;
	m_pVector[2] = 0.0f;
	m_pVector[3] = 0.0f;

	return *this;
}

inline XEngine::XVector4 & XEngine::XVector4::MakeUnitZ()
{
	m_pVector[0] = 0.0f;
	m_pVector[1] = 0.0f;
	m_pVector[2] = 1.0f;
	m_pVector[3] = 0.0f;

	return *this;
}

inline XEngine::XVector4 & XEngine::XVector4::MakeUnitW()
{
	m_pVector[0] = 0.0f;
	m_pVector[1] = 0.0f;
	m_pVector[2] = 0.0f;
	m_pVector[3] = 1.0f;

	return *this;
}

inline XEngine::XVector4::XVector4( XZero )
{
	MakeZero();
}

inline XEngine::XVector4::XVector4( XUnitX )
{
	MakeUnitX();
}

inline XEngine::XVector4::XVector4( XUnitY )
{
	MakeUnitY();
}

inline XEngine::XVector4::XVector4( XUnitZ )
{
	MakeUnitZ();
}

inline XEngine::XVector4::XVector4( XUnitW )
{
	MakeUnitW();
}

inline float const * XEngine::XVector4::GetVectorPointer() const
{
	return m_pVector;
}

inline float * XEngine::XVector4::GetVectorPointer()
{
	return m_pVector;
}

inline bool XEngine::XVector4::operator !=( XVector4 const & rOther ) const
{
	return !operator ==( rOther );
}

inline float XEngine::XVector4::operator()( int i ) const
{
	return m_pVector[i];
}

inline float & XEngine::XVector4::operator()( int i )
{
	return m_pVector[i];
}
