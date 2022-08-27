#include "game/shared/gamepch.h"
#include "game/spacegame/parallaxstar.h"
#include "game/datatree/datatree.h"
#include "engine/algorithms/xrandom.h"
#include "engine/graphics/xtexture.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CParallaxStar::CParallaxStar()
{
	m_nHorizontalSeed = -1;
	m_nVerticalSeed = -1;
	m_nImageSeed = -1;
	m_nHorizontalStride = 0;
	m_nVerticalStride = 0;
	m_nImageStride = 0;
}

///------------------------------------------------------------------------

CParallaxStar::~CParallaxStar()
{
	Deinit();
}

///------------------------------------------------------------------------

void CParallaxStar::Init( CParallaxStarSystem * pStarSystem )
{
	m_nHorizontalSeed = XEngine::XRandom::GetInt(0, CParallaxStarSystem::msc_nPatternLength);
	m_nHorizontalStride = XEngine::XRandom::GetInt(1, CParallaxStarSystem::msc_nMaxStride);

	m_nVerticalSeed = XEngine::XRandom::GetInt(0, CParallaxStarSystem::msc_nPatternLength);
	m_nVerticalStride = XEngine::XRandom::GetInt(1, CParallaxStarSystem::msc_nMaxStride);

	m_nImageSeed = XEngine::XRandom::GetInt(0, CParallaxStarSystem::msc_nPatternLength);
	m_nImageStride = XEngine::XRandom::GetInt(1, CParallaxStarSystem::msc_nMaxStride);

	CDataTree * pStarDescriptor = pStarSystem->m_pUniverseDescriptor->GetDataTree(m_nImageSeed);

	float fXPos = pStarSystem->m_pHorizontalPattern[m_nHorizontalSeed];
	float fYPos = pStarSystem->m_pVerticalPattern[m_nVerticalSeed];
	float fZPos = pStarDescriptor->GetFloat(CRC( 0x3B62336D, "Depth" ));

	XVector4 vStarInitialPosition(fXPos, fYPos, fZPos);

	XChecksum xImage = pStarDescriptor->GetChecksum(CRC( 0x04FC2B5B, "Image" ));
	XEngine::XTexture * pTexture = (XEngine::XTexture *)pStarSystem->m_oResourceComponent.GetResourceDatabase()->FindObjectByNameAndType(xImage, CRC( 0x3356DC8A, "XTexture" ));

	SRectangle oTextureArea(0.0f, 0.0f, 1.0f, 1.0f);
	if (pStarDescriptor->HasFloat(CRC( 0x4185EC4E, "TextureU0" )))  
	{
		oTextureArea.m_fX0 = pStarDescriptor->GetFloat(CRC( 0x4185EC4E, "TextureU0" ));
		oTextureArea.m_fY0 = pStarDescriptor->GetFloat(CRC( 0x6AA8BF8D, "TextureV0" ));
		oTextureArea.m_fX1 = pStarDescriptor->GetFloat(CRC( 0x3682DCD8, "TextureU1" ));
		oTextureArea.m_fY1 = pStarDescriptor->GetFloat(CRC( 0x1DAF8F1B, "TextureV1" ));
	}

	float fScale = pStarDescriptor->GetFloat(CRC( 0x2D870A80, "Scale" ));
	float fWidth = pTexture->GetWidth() * oTextureArea.GetWidth() * fScale;  ///Todo:  incorporate a scaling factor from datatree.
	float fHeight = pTexture->GetHeight() * oTextureArea.GetHeight() * fScale;

	m_oSprite.Init(pStarSystem->m_pSpriteRenderer, vStarInitialPosition, fWidth, fHeight, pTexture, oTextureArea);
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CParallaxStarSystem::CParallaxStarSystem()
{
	Reset();
}

///------------------------------------------------------------------------

CParallaxStarSystem::~CParallaxStarSystem()
{
	Deinit();
}

///------------------------------------------------------------------------

void CParallaxStarSystem::Reset()
{
	m_pCamera = nullptr;
	m_eState = eState_Unloaded;
	m_nNumStars = 0;
	m_pStars = 0;
	m_nNumTextures = 0;
	m_ppStarTextures = nullptr;
	m_pUniverseDescriptor = nullptr;
	m_pSpriteRenderer = nullptr;
}

///------------------------------------------------------------------------