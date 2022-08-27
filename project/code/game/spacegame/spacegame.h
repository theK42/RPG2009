#pragma once
#include "game/spacegame/parallaxcamera.h"
#include "game/spacegame/parallaxstar.h"
#include "game/spacegame/miniload.h"

#include "game/basegame/resourcecomponent.h"
#include "game/basegame/spriterenderer.h"
#include "game/basegame/inputsystem.h"
#include "game/basegame/screensteward.h"

#include "game/shared/practicallyuselessallocator.h"

#include "engine/graphics/xgraphics.h"
#include "engine/graphics/xtexture.h"


namespace XEngine
{
	class XMatrix44; 
}

namespace SpaceGame
{
	class CKRBSG
	{
	public:
		void Init();
		void Deinit();
		void Update(float fTime);
		void Render();

		void SetViewport(int nX0, int nY0, int nX1, int nY1);

		void SetViewMatrix(XEngine::XMatrix44 & mView);

		void OnClicked();
	private:

		enum EInputHandler
		{
			EInputHandler_Start,
			EInputHandler_Count
		};

		void OnDescriptorLoaded();
		bool IsClicked();

		XEngine::XGraphics				m_oGraphics;
		CPracticallyUselessAllocator	m_oOriginalAllocator;
		CScreenStewardship				m_oFullScreenStewardship;
		CParallaxCamera					m_oParalaxCamera;
		CLoadProcessor					m_oLoadProcesser;
		CSpriteRenderer					m_oSpriteRenderer;
		CMechanicsUpdater				m_oMechanicsUpdater;
		CMeshRenderer					m_oOverlayedMeshRenderer;
		CMiniLoad						m_oMiniLoad;
		///Going to try to get this running before splash load
		//CSplashLoad					m_oSplashLoad;
		CInputHandler					m_pInputHandler[EInputHandler_Count];

//		CParallaxStarSystem				m_oStarSystem;
		XEngine::XMatrix44				m_oViewMatrix;
		XEngine::XMatrix44				m_oOrthographicProjection;

		CResourceComponent				m_oGameDescriptorResource;


		CHeapAllocator *				m_pMainHeap;

		float							m_fLightX;
		float							m_fLightY;
		bool							m_bIsClicked;
	};
}