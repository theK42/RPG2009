
#include "engine/core/xcore.h"
#include "engine/math/xvector.h"

#include <cmath>
using std::sqrtf;

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::GetNormalizedXYZ( XEngine::XVector4 & rResult ) const
{
	float const fLengthSqr = m_pVector[0] * m_pVector[0] + m_pVector[1] * m_pVector[1] + m_pVector[2] * m_pVector[2];
	float const fRecipLen = 1.0f / sqrtf( fLengthSqr );

	rResult.m_pVector[0] = fRecipLen * m_pVector[0];
	rResult.m_pVector[1] = fRecipLen * m_pVector[1];
	rResult.m_pVector[2] = fRecipLen * m_pVector[2];

	return rResult;
}

///----------------------------------------------------------------------------

bool XEngine::XVector4::operator ==( XVector4 const & rOther ) const
{
	bool bEqual;
	bEqual = m_pVector[0] == rOther.m_pVector[0];
	bEqual &= m_pVector[1] == rOther.m_pVector[1];
	bEqual &= m_pVector[2] == rOther.m_pVector[2];
	bEqual &= m_pVector[3] == rOther.m_pVector[3];
	return bEqual;
}

///----------------------------------------------------------------------------

float XEngine::XVector4::GetDotProduct( XVector4 const & rLeft, XVector4 const & rRight )
{
	return rLeft.m_pVector[0] * rRight.m_pVector[0] + rLeft.m_pVector[1] * rRight.m_pVector[1] + rLeft.m_pVector[2] * rRight.m_pVector[2] + rLeft.m_pVector[3] * rRight.m_pVector[3];
}

///----------------------------------------------------------------------------

float XEngine::XVector4::GetDotProductXYZ( XVector4 const & rLeft, XVector4 const & rRight )
{
	return rLeft.m_pVector[0] * rRight.m_pVector[0] + rLeft.m_pVector[1] * rRight.m_pVector[1] + rLeft.m_pVector[2] * rRight.m_pVector[2];
}

///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::GetCrossProduct( XVector4 & rResult, XVector4 const & rLeft, XVector4 const & rRight )
{
	float x = rLeft.m_pVector[1] * rRight.m_pVector[2] - rLeft.m_pVector[2] * rRight.m_pVector[1];
	float y = rLeft.m_pVector[2] * rRight.m_pVector[0] - rLeft.m_pVector[0] * rRight.m_pVector[2];
	float z = rLeft.m_pVector[0] * rRight.m_pVector[1] - rLeft.m_pVector[1] * rRight.m_pVector[0];
	rResult.m_pVector[0] = x;
	rResult.m_pVector[1] = y;
	rResult.m_pVector[2] = z;
	rResult.m_pVector[3] = 0.0f;
	return rResult;
}

///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::operator +=( XVector4 const & rOther )
{
	m_pVector[0] += rOther.m_pVector[0];
	m_pVector[1] += rOther.m_pVector[1];
	m_pVector[2] += rOther.m_pVector[2];
	m_pVector[3] += rOther.m_pVector[3];

	return (*this);
}

///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::operator -=( XVector4 const & rOther )
{
	m_pVector[0] -= rOther.m_pVector[0];
	m_pVector[1] -= rOther.m_pVector[1];
	m_pVector[2] -= rOther.m_pVector[2];
	m_pVector[3] -= rOther.m_pVector[3];

	return (*this);
}

///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::operator *=( XVector4 const & rOther )
{
	m_pVector[0] *= rOther.m_pVector[0];
	m_pVector[1] *= rOther.m_pVector[1];
	m_pVector[2] *= rOther.m_pVector[2];
	m_pVector[3] *= rOther.m_pVector[3];

	return (*this);
}

///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::operator /=( XVector4 const & rOther )
{
	m_pVector[0] /= rOther.m_pVector[0];
	m_pVector[1] /= rOther.m_pVector[1];
	m_pVector[2] /= rOther.m_pVector[2];
	m_pVector[3] /= rOther.m_pVector[3];

	return (*this);
}

///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::operator *=( float s )
{
	m_pVector[0] *= s;
	m_pVector[1] *= s;
	m_pVector[2] *= s;
	m_pVector[3] *= s;

	return (*this);
}

///----------------------------------------------------------------------------

XEngine::XVector4 & XEngine::XVector4::operator /=( float s )
{
	m_pVector[0] /= s;
	m_pVector[1] /= s;
	m_pVector[2] /= s;
	m_pVector[3] /= s;

	return (*this);
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------