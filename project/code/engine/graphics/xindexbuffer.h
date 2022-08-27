
#pragma once

namespace XEngine
{
	class XGraphics;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XIndexType
	{
		enum Type
		{
			U8,
			U16,

			Max
		};
	};

	///------------------------------------------------------------------------

	struct XPrimitiveType
	{
		enum Type
		{
			TriangleList,
			TriangleStrip,
			TriangleFan,
			LineList,
			LineStrip,
			LineLoop,
			Points,

			Max
		};
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XIndexBuffer
	{
	public:
		void	Init(
					XGraphics * pGraphics,
					int nPrimitiveType,
					int nIndexType,
					int nFlags,
					int nNumIndices,
					void * pIndices );
		void	Deinit( XGraphics * pGraphics );

		void	Draw( XGraphics * pGraphics ) const;

	private:
		u8		m_nPrimitiveType;
		u8		m_nIndexType;
		u16		m_nFlags;
		u32		m_nNumIndices;
		u32		m_nId;
		void *	m_pIndices;
	};

} /// namespace XEngine
