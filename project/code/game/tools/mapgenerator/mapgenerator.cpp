// mapgenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "engine/core/xcore.h"
#include "engine/resource/xfileio.h"

#include <cmath>
#include <cstdlib>
#include <ctime> 
#include <stdio.h>
#include <string>

struct SMap
{
	SMap()
	{
		sDimensions.nWidth = 10;
		sDimensions.nHeight = 10;
		sDimensions.fScale = 1.0f;
	}
	struct SDimensions
	{
		int nWidth;
		int nHeight;
		float fScale;
	};
	struct SCorner
	{
		SCorner()
		{
			fHeight = 0;
			bSharp = false;
		}
		float fHeight;
		bool bSharp;
	};
	struct STile
	{
		STile()
		{
			bConvex = false;
			nTextureTileHorizontal = 0;
			nTextureTileVertical = 0;
		}
		bool bConvex;
		int nTextureTileHorizontal;
		int nTextureTileVertical;
	};
	struct STexture
	{
		STexture()
		{
			strcpy(pTextureName, "env_textures.tex");
			nHorizontalTiling = 2;
			nVerticalTiling = 2;
		}
		static const int TEXTURE_NAME_LENGTH = 256;
		char pTextureName[TEXTURE_NAME_LENGTH];
		int nHorizontalTiling;
		int nVerticalTiling;
	};

	SDimensions sDimensions;
	SCorner sCorners[11 * 11];
	STile sTiles[10 * 10];
	STexture sTexture;
	
};

void PerturbArea(SMap * pMap, int nXStart, int nYStart, int nXEnd, int nYEnd, float fIntensity)
{
	int nXDivision = ((nXEnd - nXStart) / 2) + nXStart;
	int nYDivision = ((nYEnd - nYStart) / 2) + nYStart;

	if (pMap->sCorners[nXDivision + nYDivision * 11].fHeight == 0.0f)
	{
		float fNorthWestHeight = pMap->sCorners[nXStart + nYStart * 11].fHeight;
		float fNorthEastHeight = pMap->sCorners[nXEnd + nYStart * 11].fHeight;
		float fSouthWestHeight = pMap->sCorners[nXStart + nYEnd * 11].fHeight;
		float fSouthEastHeight = pMap->sCorners[nXEnd + nYEnd * 11].fHeight;

		float fCenterHeight = fNorthWestHeight + fNorthEastHeight + fSouthEastHeight + fSouthWestHeight;
		fCenterHeight /= 4;

		fCenterHeight += fIntensity * ((float)rand() / RAND_MAX);

		pMap->sCorners[nXDivision + nYDivision * 11].fHeight = fCenterHeight;
	}

	if (nXStart < nXDivision && nXDivision < nXEnd && nYStart < nYDivision && nYDivision < nYEnd )
	{
		PerturbArea(pMap, nXStart, nYStart, nXDivision, nYDivision, fIntensity / 2.0f);
		PerturbArea(pMap, nXDivision, nYStart, nXEnd, nYDivision, fIntensity / 2.0f);
		PerturbArea(pMap, nXStart, nYDivision, nXDivision, nYEnd, fIntensity / 2.0f);
		PerturbArea(pMap, nXDivision, nYDivision, nXEnd, nYEnd, fIntensity / 2.0f);
	}
}

void Perturb(SMap * pMap, float fIntensity)
{
	srand((unsigned)time(0));
	PerturbArea(pMap, 0, 0, 10, 10, fIntensity);
}

void AssignTextureTiles(SMap * pMap)
{
	for (int i = 0; i < pMap->sDimensions.nHeight; i++)
	{
		for (int j = 0; j < pMap->sDimensions.nWidth; j++)
		{
			int nTileX = 1.9f * ((float)rand() / RAND_MAX);
			int nTileY = 1.9f * ((float)rand() / RAND_MAX);
			pMap->sTiles[j + i * pMap->sDimensions.nWidth].nTextureTileHorizontal = nTileX;
			pMap->sTiles[j + i * pMap->sDimensions.nWidth].nTextureTileVertical = nTileY;
		}
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	///Create the map data

	SMap oMap;
	Perturb(&oMap, 4.0f);
	AssignTextureTiles(&oMap);


	///Create the map file

	XEngine::XAsyncFileDescriptorIO::InitModule();
	XEngine::XAsyncFile oMapOutputFile;
	oMapOutputFile.Open( "testmap.dat", XEngine::XFileFlags::Write | XEngine::XFileFlags::Truncate, 0644 );
	XEngine::handle hTransfer = oMapOutputFile.BeginAsyncWrite( 0, sizeof(SMap), &oMap );
	XEngine::uaddr nTransferred = oMapOutputFile.FinishAsyncTransfer( hTransfer );
	oMapOutputFile.Close();
	XEngine::XAsyncFileDescriptorIO::DeinitModule();

	return 0;
}

