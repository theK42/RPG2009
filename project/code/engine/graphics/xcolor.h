
#pragma once

namespace XEngine
{
	class XVector4;

	class XColor
	{
	public:
		XColor();
		XColor( u32 nPacked );
		XColor( float r, float g, float b, float a );

		void	Set( float r, float g, float b, float a );

		float	GetRed() const;
		float	GetGreen() const;
		float	GetBlue() const;
		float	GetAlpha() const;

		void	SetPacked( u32 nPacked );
		u32		GetPacked() const;

		XVector4 &	GetAsVector( XVector4 & rResult ) const;

	private:
		enum
		{
#if defined( LITTLE_ENDIAN )
			RED_SHIFT	= 0,
			GREEN_SHIFT	= 8,
			BLUE_SHIFT	= 16,
			ALPHA_SHIFT	= 24,
#else
			RED_SHIFT	= 24,
			GREEN_SHIFT	= 16,
			BLUE_SHIFT	= 8,
			ALPHA_SHIFT	= 0,
#endif
		};

	private:
		u32		m_nPackedColor;
	};

} /// namespace XEngine

inline XEngine::XColor::XColor()
{
}

inline XEngine::XColor::XColor( u32 nPacked )
{
	SetPacked( nPacked );
}

inline XEngine::XColor::XColor( float r, float g, float b, float a )
{
	Set( r, g, b, a );
}

inline float XEngine::XColor::GetRed() const
{
	u32 r = (m_nPackedColor >> RED_SHIFT) & 0xFF;
	return r / 255.0f;
}

inline float XEngine::XColor::GetGreen() const
{
	u32 g = (m_nPackedColor >> GREEN_SHIFT) & 0xFF;
	return g / 255.0f;
}

inline float XEngine::XColor::GetBlue() const
{
	u32 b = (m_nPackedColor >> BLUE_SHIFT) & 0xFF;
	return b / 255.0f;
}

inline float XEngine::XColor::GetAlpha() const
{
	u32 a = (m_nPackedColor >> ALPHA_SHIFT) & 0xFF;
	return a / 255.0f;
}

inline void XEngine::XColor::SetPacked( u32 nPacked )
{
	m_nPackedColor = nPacked;
}

inline XEngine::u32 XEngine::XColor::GetPacked() const
{
	return m_nPackedColor;
}
