#pragma once
#pragma unmanaged
#include "engine/core/xcore.h"
#pragma managed

namespace XEngine
{
	class XColor;
}

namespace XWEngine
{

	public ref class XWColor
	{
	public:
		XWColor();
		XWColor( XEngine::u32 nPacked );
		XWColor( float r, float g, float b, float a );
		~XWColor();

		void	Set( float r, float g, float b, float a );

		property float Red
		{
			float get();
		}
		property float Green
		{
			float get();
		}
		property float Blue
		{
			float get();
		}
		property float Alpha
		{
			float get();
		}

		property XEngine::u32 Packed
		{
			XEngine::u32 get();
			void set(XEngine::u32 nPacked);
		}
		
// 		float	GetRed() const;
// 		float	GetGreen() const;
// 		float	GetBlue() const;
// 		float	GetAlpha() const;
// 
// 		void	SetPacked( u32 nPacked );
// 		u32		GetPacked() const;

	private:
		XEngine::XColor * m_pWrappedColor;
	};
} /// namespace XWEngine
