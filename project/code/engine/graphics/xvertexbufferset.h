
#pragma once

#include "engine/graphics/xvertexformat.h"

namespace XEngine
{
	class XVertexBuffer;
	class XGraphics;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XVertexBufferSet
	{
	public:
		XVertexBufferSet();

		void	Add( XVertexBuffer const * pVertexBuffer );

		void	Select( XGraphics * pGraphics ) const;

	private:
		u32						m_nNumVertexBuffers;
		XVertexBuffer const *	m_ppVertexBuffers[ XVertexAttributeId::Max ];
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

inline XEngine::XVertexBufferSet::XVertexBufferSet()
: m_nNumVertexBuffers( 0 )
{
}

inline void XEngine::XVertexBufferSet::Add( XVertexBuffer const * pVertexBuffer )
{
	XASSERT( m_nNumVertexBuffers < XVertexAttributeId::Max );
	m_ppVertexBuffers[ m_nNumVertexBuffers++ ] = pVertexBuffer;
}
