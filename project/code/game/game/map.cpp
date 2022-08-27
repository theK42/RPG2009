#include "game/shared/gamepch.h"
#include "game/game/map.h"
#include "engine/resource/xfileio.h"
#include "engine/math/xvector.h"
#include "engine/graphics/xgraphics.h"
#include "engine/graphics/xvertexformat.h"
#include "engine/graphics/xvertexbuffer.h"
#include "engine/graphics/xindexbuffer.h"
#include "engine/graphics/xcolor.h"

#include "stdio.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

erpg::CMap::CMap()
{
	m_oDimensions.Reset();
	m_pMapTiles = nullptr;
}

///----------------------------------------------------------------------------

erpg::CMap::~CMap()
{
}

///----------------------------------------------------------------------------

void erpg::CMap::Load( char const * pFileName )
{
	m_oInputFile.Open( pFileName, XEngine::XFileFlags::Read );
}

///----------------------------------------------------------------------------

void erpg::CMap::Init(XEngine::XAllocator * pAllocator, XEngine::XGraphics * pGraphics)
{
	///Load Dimensions
	u64 nOffset = 0;
	XEngine::handle hTransfer = m_oInputFile.BeginAsyncRead( nOffset, sizeof(SDimensions), &m_oDimensions );
	uaddr nTransferred = m_oInputFile.FinishAsyncTransfer( hTransfer );
	XASSERT(nTransferred == sizeof(SDimensions));
	nOffset += nTransferred;

	///Load Corners
	int nNumCorners = (m_oDimensions.m_nHeight + 1) * (m_oDimensions.m_nWidth + 1);
	m_pMapCorners = XNEW(pAllocator) SMapCorner[nNumCorners];
	hTransfer = m_oInputFile.BeginAsyncRead(nOffset, nNumCorners * sizeof(SMapCorner), m_pMapCorners);
	nTransferred = m_oInputFile.FinishAsyncTransfer( hTransfer );
	XASSERT(nTransferred == nNumCorners * sizeof(SMapCorner));
	nOffset += nTransferred;

	///Load Tiles
	int nNumTiles = m_oDimensions.m_nHeight * m_oDimensions.m_nWidth;
	m_pMapTiles = XNEW(pAllocator) SMapTile[nNumTiles];
	hTransfer = m_oInputFile.BeginAsyncRead(nOffset, nNumTiles * sizeof(SMapTile), m_pMapTiles);
	nTransferred = m_oInputFile.FinishAsyncTransfer( hTransfer );
	XASSERT(nTransferred == nNumTiles * sizeof(SMapTile));
	nOffset += nTransferred;

	///Load Texture Name
	char pTextureName[256];
	hTransfer = m_oInputFile.BeginAsyncRead(nOffset, 256 * sizeof(char), pTextureName);
	nTransferred = m_oInputFile.FinishAsyncTransfer(hTransfer);
	XASSERT(nTransferred == 256 * sizeof(char));
	nOffset += nTransferred;

	///Load Texture Info
	hTransfer = m_oInputFile.BeginAsyncRead(nOffset, sizeof(SVisual::STextureInfo), &m_oVisual.m_oTextureInfo);
	nTransferred = m_oInputFile.FinishAsyncTransfer(hTransfer);
	XASSERT(nTransferred == sizeof(SVisual::STextureInfo));
	nOffset += nTransferred;

	///Close file
	m_oInputFile.Close();

	m_oInputFile.Open(pTextureName, XEngine::XFileFlags::Read);
	struct STextureMetaData
	{
		u16	nFormat;
		u16	nFlags;
		u16	nWidth;
		u16 nHeight;
	};

	STextureMetaData oMetaData;
	XEngine::handle hHandle = m_oInputFile.BeginAsyncRead(0, sizeof(STextureMetaData), & oMetaData);
	m_oInputFile.FinishAsyncTransfer(hHandle);

	m_oVisual.m_pTexels = XNEW(pAllocator) u32[oMetaData.nWidth * oMetaData.nHeight];

	hHandle = m_oInputFile.BeginAsyncRead(sizeof(STextureMetaData), sizeof(u32) * oMetaData.nWidth * oMetaData.nHeight, m_oVisual.m_pTexels);
	m_oInputFile.FinishAsyncTransfer(hHandle);

	m_oInputFile.Close();
	m_oVisual.m_oTexture.Init(pGraphics, oMetaData.nFormat, oMetaData.nWidth, oMetaData.nHeight, oMetaData.nFlags, m_oVisual.m_pTexels);

	///Create Visual
	InitMesh(pAllocator, pGraphics);	
}

///----------------------------------------------------------------------------

void erpg::CMap::Deinit()
{
	m_oVisual.m_oTexture.Deinit(m_oVisual.m_pGraphics);
	XDELETE_ARRAY(m_oVisual.m_pTexels);
	m_oVisual.m_pTexels = nullptr;
	DeinitMesh();
	XDELETE_ARRAY(m_pMapCorners);
	XDELETE_ARRAY(m_pMapTiles);
}

///----------------------------------------------------------------------------

void erpg::CMap::InitMesh(XEngine::XAllocator * pAllocator, XEngine::XGraphics * pGraphics)
{
	int nNumTiles = m_oDimensions.m_nWidth * m_oDimensions.m_nHeight;
	int nNumIndices = nNumTiles * 6;
	int nNumVertices = nNumTiles * 6; ///This is unnecessarily large in many situations, ripe for future optimizations
	m_oVisual.m_pGraphics = pGraphics;
	m_oVisual.m_pIndices = XNEW(pAllocator) u16[nNumIndices];
	m_oVisual.m_pVertices = XNEW(pAllocator) SVisual::SVertex[nNumVertices];

#ifdef NORMALS_TEST
	m_oVisual.m_pNormalsTestIndices = XNEW(pAllocator) u16[nNumVertices * 2];
	m_oVisual.m_pNormalsTestVertices = XNEW(pAllocator) SVisual::SNormalsTestVertex[nNumVertices * 2];
#endif

	int nVertexIndex = 0;
	for (int i = 0; i < m_oDimensions.m_nHeight; i++)
	{
		for (int j = 0; j < m_oDimensions.m_nWidth; j++)
		{
			SMapTile * pMapTile = GetMapTile(i, j);
			
			ECorner eCurrentCorner = eCorner_NorthWest;

			///Find midpoint (average) height of NorthWest/SouthEast Diagonal
			float fDoubleDiagonalHeight1 = GetMapCorner(i, j, eCorner_NorthWest)->m_fCornerHeight + GetMapCorner(i, j, eCorner_SouthEast)->m_fCornerHeight;
			float fDoubleDiagonalHeight2 = GetMapCorner(i, j, eCorner_NorthEast)->m_fCornerHeight + GetMapCorner(i, j, eCorner_SouthWest)->m_fCornerHeight;

			bool bFirstDiagonalHigher = fDoubleDiagonalHeight1 > fDoubleDiagonalHeight2;
			///A logical exclusive or would be...useful
			if ( (bFirstDiagonalHigher && pMapTile->m_bConvex) ||
				(!bFirstDiagonalHigher && !pMapTile->m_bConvex))
			{
				eCurrentCorner = GetNextCorner(eCurrentCorner);
			}

			///Create Triangles
			for (int k = 0; k < 2; k ++)
			{
				CreateTriangle(nVertexIndex, i, j, eCurrentCorner);

				ECorner eCorner = eCurrentCorner;

				eCurrentCorner = GetOppositeCorner(eCorner);
			}
		}
	}

	XEngine::XVertexFormat oVertextFormat;
	oVertextFormat.Add( XEngine::XVertexAttributeId::Position, XEngine::XVertexAttributeType::F32, 3 );
	oVertextFormat.Add( XEngine::XVertexAttributeId::Normal, XEngine::XVertexAttributeType::F32, 3 );
	oVertextFormat.Add( XEngine::XVertexAttributeId::TexCoord0, XEngine::XVertexAttributeType::F32, 2); ///This doesn't work and I'd like to know why
	//oVertextFormat.Add( XVertexAttributeId::TexCoord0, XVertexAttributeType::S16, 2 );
	m_oVisual.m_oVertexBuffer.Init(
		pGraphics,
		oVertextFormat,
		XEngine::XBufferFlags::StaticDraw | XEngine::XBufferFlags::Discardable,
		nNumVertices,
		m_oVisual.m_pVertices );
	m_oVisual.m_oMesh.GetVertexBufferSet().Add( &m_oVisual.m_oVertexBuffer );

	m_oVisual.m_oMesh.GetIndexBuffer().Init(
		pGraphics,
		XEngine::XPrimitiveType::TriangleList,
		XEngine::XIndexType::U16,
		XEngine::XBufferFlags::StaticDraw | XEngine::XBufferFlags::Discardable,
		nNumIndices,
		m_oVisual.m_pIndices );
#ifdef NORMALS_TEST
	XEngine::XVertexFormat oNormalsTestVertextFormat;
	oNormalsTestVertextFormat.Add( XEngine::XVertexAttributeId::Position, XEngine::XVertexAttributeType::F32, 3 );
	oNormalsTestVertextFormat.Add( XEngine::XVertexAttributeId::Color, XEngine::XVertexAttributeType::U8, 4);
	m_oVisual.m_oNormalsTestVertexBuffer.Init(
		pGraphics,
		oNormalsTestVertextFormat,
		XEngine::XBufferFlags::StaticDraw | XEngine::XBufferFlags::Discardable,
		nNumVertices * 2,
		m_oVisual.m_pNormalsTestVertices);
	m_oVisual.m_oNormalsTestMesh.GetVertexBufferSet().Add( &m_oVisual.m_oNormalsTestVertexBuffer );
	
	m_oVisual.m_oNormalsTestMesh.GetIndexBuffer().Init(
		pGraphics,
		XEngine::XPrimitiveType::LineList,
		XEngine::XIndexType::U16,
		XEngine::XBufferFlags::StaticDraw | XEngine::XBufferFlags::Discardable,
		nNumVertices * 2,
		m_oVisual.m_pNormalsTestIndices );
#endif
}

///----------------------------------------------------------------------------

void erpg::CMap::CreateVertex( int nVertexIndex, int nLatitude, int nLongitude, ECorner eCurrentCorner, XEngine::XVector4 const & vTriangleNormal)
{
	m_oVisual.m_pVertices[nVertexIndex].m_fPosX = GetMapCornerLongitude(nLongitude, eCurrentCorner);
	m_oVisual.m_pVertices[nVertexIndex].m_fPosY = GetMapCornerLatitude(nLatitude, eCurrentCorner);
	m_oVisual.m_pVertices[nVertexIndex].m_fPosZ = GetMapCorner(nLatitude, nLongitude, eCurrentCorner)->m_fCornerHeight;

	///Need this corner as a vector for either of the possible normal calculations
	if (GetMapCorner(nLatitude, nLongitude, eCurrentCorner)->m_bCornerSharpness)
	{
		///Use pregenerated normal for current triangle
		m_oVisual.m_pVertices[nVertexIndex].m_fNormX = vTriangleNormal(0);
		m_oVisual.m_pVertices[nVertexIndex].m_fNormY = vTriangleNormal(1);
		m_oVisual.m_pVertices[nVertexIndex].m_fNormZ = vTriangleNormal(2);	
	}
	else
	{

		///Generate normal as average of 4 nearest triangles
		XEngine::XVector4 vCorner = GetMapCornerVector(nLongitude, nLatitude, eCurrentCorner);
		XEngine::XVector4 vNormal;
		vNormal.MakeZero();
		for (int i = 0; i < eBorder_Count; i++)
		{
			EBorder eCurrentBorder = (EBorder)i;
			EBorder eNextBorder = GetNextBorder((EBorder)i);  

			if (DoesBorderingCornerExist(nLatitude, nLongitude, eCurrentBorder, eCurrentCorner) && DoesBorderingCornerExist(nLatitude, nLongitude, eNextBorder, eCurrentCorner))
			{
				XEngine::XVector4 vCurrentBorderingCorner = GetMapCornerVector(GetBorderingLongitude(nLongitude, eCurrentBorder), GetBorderingLatitude(nLatitude, eCurrentBorder), eCurrentCorner);
				XEngine::XVector4 vNextBorderingCorner = GetMapCornerVector(GetBorderingLongitude(nLongitude, eNextBorder), GetBorderingLatitude(nLatitude, eNextBorder), eCurrentCorner);

				XEngine::XVector4 vCornerToBordering = vCurrentBorderingCorner; vCornerToBordering -= vCorner;
				XEngine::XVector4 vCornerToNextBordering = vNextBorderingCorner; vCornerToNextBordering -= vCorner;
				
				XEngine::XVector4 vOneNormal;
				vNormal += XEngine::XVector4::GetCrossProduct(vOneNormal, vCornerToBordering, vCornerToNextBordering);
			}
			else
			{
				vNormal += XEngine::UNIT_Z;
			}
		}
		vNormal.GetNormalizedXYZ(vNormal);


		m_oVisual.m_pVertices[nVertexIndex].m_fNormX = vNormal(0);
		m_oVisual.m_pVertices[nVertexIndex].m_fNormY = vNormal(1);
		m_oVisual.m_pVertices[nVertexIndex].m_fNormZ = vNormal(2);	

		int nTextureTileHorizontal = GetMapTile(nLatitude, nLongitude)->m_nTextureTileHorizontal;
		int nTextureTileVertical = GetMapTile(nLatitude, nLongitude)->m_nTextureTileVertical;

		m_oVisual.m_pVertices[nVertexIndex].m_fTexU = (float)(GetDeltaLambda(eCurrentCorner) + nTextureTileHorizontal) / m_oVisual.m_oTextureInfo.m_nHorizontalTiling;
		m_oVisual.m_pVertices[nVertexIndex].m_fTexV = (float)(GetDeltaPhi(eCurrentCorner) + nTextureTileVertical) / m_oVisual.m_oTextureInfo.m_nVerticalTiling;
	}
#ifdef NORMALS_TEST
	m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2].m_fPosX = m_oVisual.m_pVertices[nVertexIndex].m_fPosX;
	m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2].m_fPosY = m_oVisual.m_pVertices[nVertexIndex].m_fPosY;
	m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2].m_fPosZ = m_oVisual.m_pVertices[nVertexIndex].m_fPosZ;
	if (nLatitude >= 9 && nLongitude >= 9)
	{
		m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2].m_oColor = XEngine::XColor(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2].m_oColor = XEngine::XColor(1.0f, 0.0f, 0.0f, 1.0f);
	}
	m_oVisual.m_pNormalsTestIndices[nVertexIndex * 2] = nVertexIndex * 2;
	m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2 + 1].m_fPosX = m_oVisual.m_pVertices[nVertexIndex].m_fPosX + m_oVisual.m_pVertices[nVertexIndex].m_fNormX;
	m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2 + 1].m_fPosY = m_oVisual.m_pVertices[nVertexIndex].m_fPosY + m_oVisual.m_pVertices[nVertexIndex].m_fNormY;
	m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2 + 1].m_fPosZ = m_oVisual.m_pVertices[nVertexIndex].m_fPosZ + m_oVisual.m_pVertices[nVertexIndex].m_fNormZ;
	m_oVisual.m_pNormalsTestVertices[nVertexIndex * 2 + 1].m_oColor = XEngine::XColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_oVisual.m_pNormalsTestIndices[nVertexIndex * 2 + 1] = nVertexIndex * 2 + 1;
#endif
}
///----------------------------------------------------------------------------

void erpg::CMap::DeinitMesh()
{
	m_oVisual.m_oMesh.GetIndexBuffer().Deinit(m_oVisual.m_pGraphics);	
	XDELETE(m_oVisual.m_pIndices);
	m_oVisual.m_pIndices = nullptr;
	m_oVisual.m_oVertexBuffer.Deinit(m_oVisual.m_pGraphics);
	XDELETE(m_oVisual.m_pVertices);
	m_oVisual.m_pVertices = nullptr;

#ifdef NORMALS_TEST
	m_oVisual.m_oNormalsTestMesh.GetIndexBuffer().Deinit(m_oVisual.m_pGraphics);
	XDELETE(m_oVisual.m_pNormalsTestIndices);
	m_oVisual.m_pNormalsTestIndices = nullptr;
	m_oVisual.m_oNormalsTestVertexBuffer.Deinit(m_oVisual.m_pGraphics);
	XDELETE(m_oVisual.m_pNormalsTestVertices);
	m_oVisual.m_pNormalsTestVertices = nullptr;
#endif

}

///----------------------------------------------------------------------------

erpg::CMap::SMapTile * erpg::CMap::GetMapTile( int nLongitude, int nLatitude )
{
	return &m_pMapTiles[nLongitude + nLatitude * m_oDimensions.m_nWidth];
}

///----------------------------------------------------------------------------

erpg::CMap::SMapCorner * erpg::CMap::GetMapCorner( int nLatitude, int nLongitude, ECorner eCorner )
{
	int nPhi = nLatitude + GetDeltaPhi(eCorner);
	int nLambda = nLongitude + GetDeltaLambda(eCorner);
	XASSERT(IsBounded(nPhi, 0, m_oDimensions.m_nHeight));
	XASSERT(IsBounded(nLambda, 0, m_oDimensions.m_nWidth));
	return &m_pMapCorners[nLambda + nPhi * (m_oDimensions.m_nWidth + 1)]; 
}

///----------------------------------------------------------------------------

float erpg::CMap::GetMapCornerLongitude( int nLongitude, ECorner eCorner )
{
	return (nLongitude + GetDeltaLambda(eCorner)) * m_oDimensions.m_fTileSize;
}

///----------------------------------------------------------------------------

float erpg::CMap::GetMapCornerLatitude( int nLatitude, ECorner eCorner )
{
	return (nLatitude + GetDeltaPhi(eCorner)) * m_oDimensions.m_fTileSize;
}

///----------------------------------------------------------------------------

int erpg::CMap::GetDeltaPhi( ECorner eCorner )
{
	return eCorner == eCorner_SouthWest || eCorner == eCorner_SouthEast ? 1 : 0;
}

///----------------------------------------------------------------------------

int erpg::CMap::GetDeltaLambda( ECorner eCorner )
{
	return eCorner == eCorner_NorthEast || eCorner == eCorner_SouthEast ? 1 : 0;
}

///----------------------------------------------------------------------------

int erpg::CMap::GetDeltaPhi( EBorder eBorder )
{
	return eBorder == eBorder_North ? -1 : eBorder == eBorder_South ? 1 : 0;
}

///----------------------------------------------------------------------------

int erpg::CMap::GetDeltaLambda( EBorder eBorder )
{
	return eBorder == eBorder_West ? -1 : eBorder == eBorder_East ? 1 : 0;
}

///----------------------------------------------------------------------------

XEngine::XVector4 erpg::CMap::GetMapCornerVector( int nLongitude, int nLatitude, ECorner eCorner )
{
	float fX = GetMapCornerLongitude(nLongitude, eCorner);
	float fY = GetMapCornerLatitude(nLatitude, eCorner);
	float fZ = GetMapCorner(nLatitude, nLongitude, eCorner)->m_fCornerHeight;
	float fW = 1.0f;
	return XEngine::XVector4(fX, fY, fZ, fW);
}

///----------------------------------------------------------------------------

erpg::CMap::ECorner erpg::CMap::GetNextCorner( ECorner eCorner )
{
	return (ECorner)((eCorner + 1) % eCorner_Count); 
}

///----------------------------------------------------------------------------

erpg::CMap::ECorner erpg::CMap::GetPreviousCorner( ECorner eCorner )
{
	return (ECorner)((eCorner - 1) % eCorner_Count); 
}

///----------------------------------------------------------------------------

erpg::CMap::ECorner erpg::CMap::GetOppositeCorner( ECorner eCorner )
{
	return (ECorner)((eCorner + 2) % eCorner_Count);
}

///----------------------------------------------------------------------------

erpg::CMap::EBorder erpg::CMap::GetNextBorder( EBorder eCurrentBorder )
{
	return (EBorder)((eCurrentBorder + 1) % eBorder_Count);
}

///----------------------------------------------------------------------------

int erpg::CMap::GetBorderingLatitude( int nLatitude, EBorder eBorder )
{
	return nLatitude + (eBorder == eBorder_North ? -1 : eBorder == eBorder_South ? 1 : 0);
}

///----------------------------------------------------------------------------

int erpg::CMap::GetBorderingLongitude( int nLongitude, EBorder eBorder )
{
	return nLongitude + (eBorder == eBorder_West ? -1 : eBorder == eBorder_East ? 1 : 0);
}

///----------------------------------------------------------------------------

bool erpg::CMap::DoesBorderExist( int nLatitude, int nLongitude, EBorder eBorder )
{
	return	(nLongitude - 1 >= 0 || eBorder != eBorder_West) &&
			(nLongitude + 1 < m_oDimensions.m_nWidth || eBorder != eBorder_East) &&
			(nLatitude - 1 >= 0 || eBorder != eBorder_North) &&
			(nLatitude + 1 < m_oDimensions.m_nHeight || eBorder != eBorder_South);
}

///----------------------------------------------------------------------------

bool erpg::CMap::DoesBorderingCornerExist( int nLatitude, int nLongitude, EBorder eBorder, ECorner eCorner )
{
	int nPhi = nLatitude + GetDeltaPhi(eBorder) + GetDeltaPhi(eCorner);
	int nLambda = nLongitude + GetDeltaLambda(eBorder) + GetDeltaLambda(eCorner);
	return IsBounded(nPhi, 0, m_oDimensions.m_nHeight) && IsBounded(nLambda, 0, m_oDimensions.m_nWidth); ///IsBounded is inclusive, so the increased size of the corners array is cancelled out.
}	

///----------------------------------------------------------------------------

void erpg::CMap::Update( float fTime )
{
	///Does nothing for the moment.
}

///----------------------------------------------------------------------------

void erpg::CMap::Render(XEngine::XGraphics * pGraphics)
{
	pGraphics->PushRenderState();
 	pGraphics->SetColor( XEngine::XColor( 0, 1, 0, 1 ) );
	m_oVisual.m_oTexture.Select(pGraphics, 0);
	pGraphics->EnableTexture(0, true);
 	m_oVisual.m_oMesh.Draw(pGraphics);
	pGraphics->PopRenderState();
#ifdef NORMALS_TEST
	pGraphics->PushRenderState();
	pGraphics->SetColor( XEngine::XColor( 1, 0, 0, 1 ) );
	pGraphics->EnableLighting(false);
	m_oVisual.m_oNormalsTestMesh.Draw(pGraphics);
	pGraphics->PopRenderState();
#endif
}

///----------------------------------------------------------------------------

void erpg::CMap::CreateTriangle( int &nVertexIndex, int nLatitude, int nLongitude, ECorner eStartingCorner )
{
	ECorner eCurrentCorner = eStartingCorner;
	XEngine::XVector4  vTriangleNormal = CreateTriangleNormal(nLatitude, nLongitude, eStartingCorner);
	for (int l = 0; l < 3; l++)	///But I don't think this code is going to work on anything other than squares and triangles anyway.
	{
		CreateVertex(nVertexIndex, nLatitude, nLongitude, eCurrentCorner, vTriangleNormal);
		m_oVisual.m_pIndices[nVertexIndex] = nVertexIndex;
		nVertexIndex++;
		eCurrentCorner = GetNextCorner(eCurrentCorner);
	}
}

///----------------------------------------------------------------------------

XEngine::XVector4 erpg::CMap::CreateTriangleNormal( int nLatitude, int nLongitude, ECorner eStartingCorner )
{
	///Generate normal for current triangle
	XEngine::XVector4 vCorner = GetMapCornerVector(nLongitude, nLatitude, eStartingCorner);
	XEngine::XVector4 vNextCorner = GetMapCornerVector(nLongitude, nLatitude, GetNextCorner(eStartingCorner));
	XEngine::XVector4 vOppositeCorner = GetMapCornerVector(nLongitude, nLatitude, GetOppositeCorner(eStartingCorner));

	XEngine::XVector4 vCornerToNext = vNextCorner; vCornerToNext -= vCorner;
	XEngine::XVector4 vCornerToOpposite = vOppositeCorner; vCornerToOpposite -= vCorner;

	XEngine::XVector4 vNormal;
	XEngine::XVector4::GetCrossProduct(vNormal, vCornerToNext, vCornerToOpposite);
	vNormal.GetNormalizedXYZ(vNormal);
	return vNormal;		
}

///----------------------------------------------------------------------------

void erpg::CMap::SDimensions::Reset()
{
	m_nHeight = 0;
	m_nWidth = 0;
}

///----------------------------------------------------------------------------

void erpg::CMap::SMapCorner::Reset()
{
	m_fCornerHeight = 0.0f;
	m_bCornerSharpness = false;
}

///----------------------------------------------------------------------------

void erpg::CMap::SMapTile::Reset()
{
	m_bConvex = false;
}

///----------------------------------------------------------------------------

erpg::CMap::SVisual::SVisual()
{
	m_pGraphics = nullptr;
	m_pVertices = nullptr;
	m_pIndices = nullptr;
}

///----------------------------------------------------------------------------

erpg::CMap::SVisual::~SVisual()
{
	XASSERT(m_pIndices == nullptr);
	XASSERT(m_pVertices == nullptr);
}

///----------------------------------------------------------------------------

void erpg::CMap::SVisual::SVertex::Reset()
{
	m_fPosX = 0.0f;
	m_fPosY = 0.0f;
	m_fPosZ = 0.0f;
	m_fNormX = 0.0f;
	m_fNormY = 0.0f;
	m_fNormZ = 1.0f;
	m_fTexU = 0.0f;
	m_fTexV = 0.0f;
}