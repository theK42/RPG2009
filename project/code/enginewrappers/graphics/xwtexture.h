#pragma once

#pragma unmanaged
#include "engine/core/xcore.h"
#include "engine/graphics/xtexture.h"
#pragma managed 



using namespace XEngine;
using namespace System;
using namespace System::Drawing;


namespace XWEngine
{
	ref class XWGraphics;
	ref class XWAsyncFile;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	public enum class XWTextureFlags
	{
		None			= 0,
		HasMips			= XTextureFlags::HasMips,
		GenerateMips	= XTextureFlags::GenerateMips,
		Discardable		= XTextureFlags::Discardable,
	};

	///------------------------------------------------------------------------

	public enum class XWTextureFormat
	{
		L_8			= XTextureFormat::L_8,
		LA_88		= XTextureFormat::LA_88,
		RGB_565		= XTextureFormat::RGB_565,
		RGB_888		= XTextureFormat::RGB_888,
		RGBA_5551	= XTextureFormat::RGBA_5551,
		RGBA_4444	= XTextureFormat::RGBA_4444,
		RGBA_8888	= XTextureFormat::RGBA_8888,
		RGB_PVRTC2	= XTextureFormat::RGB_PVRTC2,
		RGB_PVRTC4	= XTextureFormat::RGB_PVRTC4,
		RGBA_PVRTC2	= XTextureFormat::RGBA_PVRTC2,
		RGBA_PVRTC4	= XTextureFormat::RGBA_PVRTC4,
		Max			= XTextureFormat::Max
	};

	///------------------------------------------------------------------------

	public ref class XWTexture
	{
	public:
		XWTexture();
		~XWTexture();
		void	Init(
			XWGraphics ^ rGraphics,
			XWTextureFlags eFlags,
			System::Drawing::Bitmap ^ rBitmap );
		void	Deinit( XWGraphics ^ rGraphics );

		void	Select( XWGraphics ^ rGraphics, int nTextureIndex );

		array<u8> ^	Serialize();

	private:
		XTexture * m_pWrappedTexture;
		void *	m_pTexels;
		size_t	m_nTexelsSize;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine
