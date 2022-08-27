
#pragma once

#include "engine/graphics/xindexbuffer.h"
#include "engine/graphics/xvertexbufferset.h"

namespace XEngine
{
	class XGraphics;

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XMesh
	{
	public:
		XIndexBuffer const &		GetIndexBuffer() const;
		XIndexBuffer &				GetIndexBuffer();

		XVertexBufferSet const &	GetVertexBufferSet() const;
		XVertexBufferSet &			GetVertexBufferSet();

		void						Draw( XGraphics * pGraphics ) const;

	private:
		u32					m_nMaterialIndex;
		XIndexBuffer		m_oIndexBuffer;
		XVertexBufferSet	m_oVertexBufferSet;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

inline XEngine::XIndexBuffer const & XEngine::XMesh::GetIndexBuffer() const
{
	return m_oIndexBuffer;
}

inline XEngine::XIndexBuffer & XEngine::XMesh::GetIndexBuffer()
{
	return m_oIndexBuffer;
}

inline XEngine::XVertexBufferSet const & XEngine::XMesh::GetVertexBufferSet() const
{
	return m_oVertexBufferSet;
}

inline XEngine::XVertexBufferSet & XEngine::XMesh::GetVertexBufferSet()
{
	return m_oVertexBufferSet;
}
