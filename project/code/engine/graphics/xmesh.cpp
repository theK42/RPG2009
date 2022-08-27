
#include "engine/core/xcore.h"
#include "engine/graphics/xmesh.h"
#include "engine/graphics/xgraphics.h"

void XEngine::XMesh::Draw( XGraphics * pGraphics ) const
{
	m_oVertexBufferSet.Select( pGraphics );
	m_oIndexBuffer.Draw( pGraphics );
}
