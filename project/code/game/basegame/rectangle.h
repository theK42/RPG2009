#pragma once

#include <math.h>

///It's just convenient to have a struct to represent a 2D rectangle that multiple unrelated classes can use
struct SRectangle
{
	SRectangle();
	SRectangle(float fX0, float fY0, float fX1, float fY1);
	float GetWidth() const;
	float GetHeight() const;
	float m_fX0;
	float m_fY0;
	float m_fX1;
	float m_fY1;
};

inline SRectangle::SRectangle()
{
	m_fX0 = 0.0f;
	m_fY0 = 0.0f;
	m_fX1 = 0.0f;
	m_fY1 = 0.0f;
}

inline SRectangle::SRectangle(float fX0, float fY0, float fX1, float fY1)
{
	m_fX0 = fX0;
	m_fY0 = fY0;
	m_fX1 = fX1;
	m_fY1 = fY1;
}

inline float SRectangle::GetWidth() const
{
	return abs(m_fX1 - m_fX0);
}

inline float SRectangle::GetHeight() const
{
	return abs(m_fY1 - m_fY0);
}