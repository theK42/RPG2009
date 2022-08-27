
#pragma once

#include "engine/graphics/xvertexformat.h"

namespace XEngine
{
	class XGraphics;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XVertexBuffer
	{
	public:
		void					Init(
									XGraphics * pGraphics,
									XVertexFormat const & rFormat,
									int nFlags,
									int nNumVertices,
									void * pVertices );
		void					Deinit( XGraphics * pGraphics );

		XVertexFormat const &	GetFormat() const;
		unsigned				GetFlags() const;
		void const *			GetVertices() const;
		int						GetId() const;

	private:
		XVertexFormat	m_oFormat;
		u32				m_nFlags;
		u32				m_nNumVertices;
		u32				m_nId;
		void *			m_pVertices;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::XVertexFormat const & XEngine::XVertexBuffer::GetFormat() const
{
	return m_oFormat;
}

///----------------------------------------------------------------------------

inline unsigned XEngine::XVertexBuffer::GetFlags() const
{
	return m_nFlags;
}

///----------------------------------------------------------------------------

inline void const * XEngine::XVertexBuffer::GetVertices() const
{
	return m_pVertices;
}

///----------------------------------------------------------------------------

inline int XEngine::XVertexBuffer::GetId() const
{
	return m_nId;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
