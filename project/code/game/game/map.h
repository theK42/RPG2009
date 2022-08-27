#pragma once
#include "engine/resource/xfileio.h"
#include "engine/graphics/xmesh.h"
#include "engine/graphics/xvertexbuffer.h"
#include "engine/graphics/xcolor.h"
#include "engine/graphics/xtexture.h"

namespace XEngine
{
	class XAllocator;
	class XVector4;
}

#define NORMALS_TEST

namespace erpg
{
	class CMap
	{
	public:
		CMap();
		~CMap();
		void Load(char const * pFileName);
		void Init(XEngine::XAllocator * pAllocator, XEngine::XGraphics * pGraphics);
		void Deinit();
		void Update(float fTime);
		void Render(XEngine::XGraphics * pGraphics);
	private:
		struct SDimensions
		{
			void			Reset();
			int				m_nWidth;
			int				m_nHeight;
			float			m_fTileSize;
		};

		struct SMapCorner
		{
			float	m_fCornerHeight;
			bool	m_bCornerSharpness;
			void	Reset();
		};

		struct SMapTile
		{	
			void	Reset();
			bool	m_bConvex;
			int		m_nTextureTileHorizontal;
			int		m_nTextureTileVertical;
		};

		///Corner enum needs to wrap in this direction for culling purposes
		enum	ECorner
		{
			eCorner_None = -1,
			eCorner_NorthWest,
			eCorner_NorthEast,
			eCorner_SouthEast,
			eCorner_SouthWest,
			eCorner_Count
		};
		
		//Border enum too, it affects normals
		enum	EBorder
		{
			eBorder_None = -1,
			eBorder_North,
			eBorder_East,
			eBorder_South,
			eBorder_West,
			eBorder_Count
		};

		///SVisual is really quite unsatisfactory at this point.
		struct SVisual
		{
			struct SVertex
			{
				void Reset();
				float	m_fPosX;
				float	m_fPosY;
				float	m_fPosZ;
				float	m_fNormX;
				float	m_fNormY;
				float	m_fNormZ;
				float	m_fTexU;
				float	m_fTexV;
			};	
			
			struct STextureInfo
			{
				int m_nHorizontalTiling;
				int m_nVerticalTiling;
			};

			SVisual();
			~SVisual();
			u16 *					m_pIndices;
			SVertex *				m_pVertices;

			XEngine::XVertexBuffer	m_oVertexBuffer;
			XEngine::XMesh			m_oMesh;

			STextureInfo			m_oTextureInfo;
			XEngine::XTexture		m_oTexture;
			void *					m_pTexels;

			XEngine::XGraphics *	m_pGraphics;

#ifdef NORMALS_TEST
			struct SNormalsTestVertex
			{
				float	m_fPosX;
				float	m_fPosY;
				float	m_fPosZ;
				XEngine::XColor m_oColor;
			};
			u16 *					m_pNormalsTestIndices;
			SNormalsTestVertex *	m_pNormalsTestVertices;
			XEngine::XVertexBuffer	m_oNormalsTestVertexBuffer;
			XEngine::XMesh			m_oNormalsTestMesh;
#endif
		};


		void InitMesh(XEngine::XAllocator * pAllocator, XEngine::XGraphics * pGraphics);

		void CreateTriangle( int &nVertexIndex, int nTileLatitude, int nTileLongitude, ECorner eStartingCorner );
		XEngine::XVector4 CreateTriangleNormal(int nTileLatitude, int nTileLongitude, ECorner eStartingCorner );
		void CreateVertex( int nVertexIndex, int nLatitude, int nLongitude, ECorner eCurrentCorner, XEngine::XVector4 const & vTriangleNormal );

		bool DoesBorderExist( int nLatitude, int nLongitude, EBorder eBorder );
		bool DoesBorderingCornerExist( int nLatitude, int nLongitude, EBorder eBorder, ECorner eCorner );
		int GetBorderingLatitude( int nLatitude, EBorder eBorder );
		int GetBorderingLongitude( int nLongitude, EBorder eBorder );
		enum EBorder GetNextBorder( EBorder eCurrentBorder );
		void DeinitMesh();
		
		ECorner GetNextCorner(ECorner eCorner);
		ECorner GetPreviousCorner(ECorner eCorner);
		ECorner GetOppositeCorner(ECorner eCorner);

		SMapTile * GetMapTile(int nLongitude, int nLatitude);
		SMapCorner * GetMapCorner(int nLatitude, int nLongitude, ECorner eCorner);

		int GetDeltaLambda( ECorner eCorner );
		int GetDeltaPhi( ECorner eCorner );
		int GetDeltaLambda( EBorder eBorder	);
		int GetDeltaPhi( EBorder eBorder );
		float GetMapCornerLongitude(int nLongitude, ECorner eCorner);
		float GetMapCornerLatitude(int nLatitude, ECorner eCorner);
		XEngine::XVector4 GetMapCornerVector(int nLongitude, int nLatitude, ECorner eCorner);
		
		XEngine::XAsyncFile	m_oInputFile;
		SDimensions			m_oDimensions;
		SVisual				m_oVisual;
		SMapCorner *		m_pMapCorners;
		SMapTile *			m_pMapTiles;
	};
}