
#pragma once

namespace XEngine
{
	class XGraphics;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XTextureFlags
	{
		enum Type
		{
			HasMips			= 0x04,
			GenerateMips	= 0x08,
			Discardable		= 0x10,
		};
	};

	///------------------------------------------------------------------------

	struct XTextureFormat
	{
		enum Type
		{
			L_8,
			LA_88,
			RGB_565,
			RGB_888,
			RGBA_5551,
			RGBA_4444,
			RGBA_8888,
			RGB_PVRTC2,
			RGB_PVRTC4,
			RGBA_PVRTC2,
			RGBA_PVRTC4,

			Max
		};
	};

	///------------------------------------------------------------------------

	class XTexture
	{
	public:
		void	Init(
					XGraphics * pGraphics,
					int nFormat,
					int nWidth,
					int nHeight,
					int nFlags,
					void * pTexels );
		void	Deinit( XGraphics * pGraphics );

		void	Select( XGraphics * pGraphics, int nTextureIndex ) const;

		inline u16	GetFormat() const;
		inline u16	GetFlags() const;
		inline u16	GetWidth() const;
		inline u16	GetHeight() const;

	protected:
		u16		m_nFormat;
		u16		m_nWidth;
		u16		m_nHeight;
		u16		m_nFlags;
		u32		m_nId;
		void *	m_pTexels;
	};

	///------------------------------------------------------------------------

	inline u16 XEngine::XTexture::GetFormat() const
	{
		return m_nFormat;
	}

	///------------------------------------------------------------------------

	inline u16 XEngine::XTexture::GetFlags() const
	{
		return m_nFlags;
	}

	///------------------------------------------------------------------------

	inline u16 XEngine::XTexture::GetWidth() const
	{
		return m_nWidth;
	}

	///------------------------------------------------------------------------

	inline u16 XEngine::XTexture::GetHeight() const
	{
		return m_nHeight;
	}

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------


} /// namespace XEngine
