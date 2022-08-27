#pragma once

#include "game/basegame/spriterenderer.h"
#include "game/basegame/resourcecomponent.h"
#include "game/basegame/rectangle.h"
#include "game/spacegame/parallaxcamera.h"

class CParallaxCamera;
class CParallaxStarSystem;
class CDataTree;
class CHeapAllocator;

class CParallaxStar
{
private:   
	CParallaxStar();
	~CParallaxStar();
	
	void Init(CParallaxStarSystem * pStarSystem);
	void Deinit();
	void Update(float fTime);
	CSprite			m_oSprite;
	int				m_nHorizontalSeed;
	int				m_nVerticalSeed;
	int				m_nHorizontalStride;
	int				m_nVerticalStride;
	int				m_nImageSeed;
	int				m_nImageStride;
	
	friend class CParallaxStarSystem;
};

class CParallaxStarSystem
{
public:
	CParallaxStarSystem();
	~CParallaxStarSystem();
	
	void Init(CDataTree * pUniverseDescriptor, int nNumStars, CHeapAllocator * pHeap, CLoadProcessor * pLoadProcessor);
	void Deinit();

	void BeginRendering(CSpriteRenderer * pRenderer);
	void StopRendering();

	void Update(float fTime);
private:

	enum EState
	{
		eState_Unloaded,
		eState_Loading,
		eState_Loaded,
		eState_Rendering,
		eState_Unloading
	};
	
	void Reset();

	CParallaxCamera *					m_pCamera;
	EState								m_eState;
	int									m_nNumStars;
	CParallaxStar *						m_pStars;
	CResourceComponent					m_oResourceComponent;
	static const int					msc_nPatternLength = 100;
	static const int					msc_nMaxStride = 10;
	float								m_pHorizontalPattern[msc_nPatternLength];
	float								m_pVerticalPattern[msc_nPatternLength];
	float								m_pSpritePattern[msc_nPatternLength];
	int									m_nNumTextures;
	XEngine::XTexture **				m_ppStarTextures;
	CDataTree *							m_pUniverseDescriptor;
	CSpriteRenderer *					m_pSpriteRenderer;

	friend class CParallaxStar;
};