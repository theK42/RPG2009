
#include "engine/core/xcore.h"
#include "engine/graphics/xvertexbufferset.h"
#include "engine/graphics/xgraphics.h"
#include "engine/graphics/xvertexbuffer.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XVertexBufferSet::Select( XGraphics * pGraphics ) const
{
	for( unsigned iBuffer = 0; iBuffer < m_nNumVertexBuffers; ++iBuffer )
	{
		XVertexBuffer const * pVertexBuffer = m_ppVertexBuffers[iBuffer];
		XASSERT_PTR( pVertexBuffer );

		XVertexFormat const & rVertexFormat	= pVertexBuffer->GetFormat();
		unsigned const nFlags				= pVertexBuffer->GetFlags();
		int const nBufferObjectId			= pVertexBuffer->GetId();
		uaddr nBufferAddress				= 0;

		if( nFlags & XBufferFlags::DoNotDraw )
		{
			continue;
		}

		if( nBufferObjectId == 0 )
		{
			void const * pVertices = pVertexBuffer->GetVertices();
			XASSERT_PTR( pVertices );

			nBufferAddress = reinterpret_cast< uaddr >( pVertices );
		}

		unsigned const nNumAttributes	= rVertexFormat.GetNumAttributes();
		unsigned const nVertexStride	= rVertexFormat.GetVertexStride();
		for( unsigned iAttribute = 0; iAttribute < nNumAttributes; ++iAttribute )
		{
			int eAttributeId				= rVertexFormat.GetAttributeId( iAttribute );
			int eAttributeType				= rVertexFormat.GetAttributeType( iAttribute );
			unsigned nNumComponents			= rVertexFormat.GetNumComponents( iAttribute );
			unsigned nOffset				= rVertexFormat.GetOffset( iAttribute );
			void const * pPointerOrOffset	= (void const *)(nBufferAddress + nOffset);

			pGraphics->SetVertexAttributePointer(
				eAttributeId,
				eAttributeType,
				nNumComponents,
				nVertexStride,
				nBufferObjectId,
				pPointerOrOffset );
		}
	}
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
