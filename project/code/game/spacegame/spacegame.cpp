#include "game/shared/gamepch.h"
#include "game/spacegame/spacegame.h"
#include "game/basegame/resourcecomponent.h"
#include "game/basegame/spriterenderer.h"
#include "game/basegame/heap.h"

#include "game/spacegame/parallaxcamera.h"
#include "game/spacegame/parallaxstar.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

void SpaceGame::CKRBSG::Init()
{
	m_oViewMatrix.MakeIdentity();		
	m_oOrthographicProjection.MakeOrthographic(0.0f, 160.0f, 90.0f, 0.0f, 0.0f, 1000.0f );

	m_oGraphics.Init();

	m_oMechanicsUpdater.Init();
	m_oOverlayedMeshRenderer.Init(&m_oGraphics);

	m_oMiniLoad.Init(&m_oOverlayedMeshRenderer, &m_oMechanicsUpdater);

	m_oFullScreenStewardship.Init();

	CScreenSteward * pMiniLoadSteward = m_oMiniLoad.GetScreenSteward();
	XASSERT(pMiniLoadSteward->IsReady());
	pMiniLoadSteward->TakeResponsibility(&m_oFullScreenStewardship);

	static const size_t s_nMainHeapSize = KB(1);

	m_pMainHeap = XNEW(&m_oOriginalAllocator) CHeapAllocator(&m_oOriginalAllocator, KB(1), 1);

	m_oSpriteRenderer.Init(&m_oGraphics);

	XEngine::XTestFunctor oClickedFunctor;
	oClickedFunctor.Init(this, &CKRBSG::IsClicked);

	m_pInputHandler[EInputHandler_Start].Init(oClickedFunctor);

	//m_oSplashLoad.Init("krbsgsplash.bin", &m_oLoadProcesser, &m_oSpriteRenderer, &m_oInputSystem);
	//pMiniLoadSteward->PassResponsibility(m_oSplashLoad.GetScreenSteward());
//	m_oLoadProcessor.Init();
// 	XEngine::XSimpleFunctor oDescriptorLoadedCallback;
// 	oDescriptorLoadedCallback.Init(this, &OnDescriptorLoaded);
// 	m_oGameDescriptorResource.Init("krbsgdescriptor.bin", &m_oLoadProcesser, m_pMainHeap, m_pMainHeap, oDescriptorLoadedCallback);
}

///------------------------------------------------------------------------

void SpaceGame::CKRBSG::OnDescriptorLoaded()
{
	CDataTree * pUniverseDescriptor = (CDataTree *)m_oGameDescriptorResource.GetResourceDatabase()->FindObjectByNameAndType(CRC( 0xEA56FC1C, "UniverseDescriptor" ), CRC( 0x83A99FFE, "CDataTree" ));
	//m_oStarSystem.Init( pUniverseDescriptor, 500, m_pMainHeap, &m_oLoadProcesser);
	//m_oSplashLoad.GetScreenSteward()->PassResponsibilityWhenReady(&m_oScreenSteward);

}

///------------------------------------------------------------------------

void SpaceGame::CKRBSG::Deinit()
{
	//m_oGameDescriptorResource.Unload(&m_oLoadProcesser);
	//m_oLoadProcessor.Deinit();
	
	for (int i = EInputHandler_Start; i < EInputHandler_Count; i++)
	{
		m_pInputHandler[i].Deinit();
	}

	m_oSpriteRenderer.Deinit();

	XDELETE(m_pMainHeap);

	m_oFullScreenStewardship.Deinit();

	m_oMiniLoad.Deinit();
	m_oOverlayedMeshRenderer.Deinit();
	m_oMechanicsUpdater.Deinit();

	m_oGraphics.Deinit();
}

///------------------------------------------------------------------------

void SpaceGame::CKRBSG::Update( float fTime )
{
	for (int i = EInputHandler_Start; i < EInputHandler_Count; i++)
	{
		m_pInputHandler[i].Update(fTime);
	}
	m_oFullScreenStewardship.Update(fTime);
	m_oMechanicsUpdater.Update(fTime);
}

///------------------------------------------------------------------------

void SpaceGame::CKRBSG::Render()
{
	m_oGraphics.ClearScreen( XEngine::XScreenClearTarget::Color | XEngine::XScreenClearTarget::Depth );
	
	m_oGraphics.PushRenderState();
	XEngine::XMatrix44 mProjection;
	m_oGraphics.EnableLighting(false);
	m_oGraphics.SetProjectionMatrix(m_oOrthographicProjection);
	m_oGraphics.SetViewMatrix( m_oViewMatrix );
	m_oOverlayedMeshRenderer.Render();
	m_oGraphics.PopRenderState();
	//m_oSpriteRenderer.Render();
}

///------------------------------------------------------------------------

void SpaceGame::CKRBSG::SetViewport( int nX0, int nY0, int nX1, int nY1 )
{
	m_oGraphics.SetViewport( nX0, nY0, nX1, nY1 );
	float nWidth = nX1 - nX0;
	float nHeight = nY1 - nY0;
	float fRatio = nWidth / nHeight;
	XASSERT(fRatio <= 16.0f / 9.0f); ///We don't support anything wider than 16:9

	if (fRatio < 16.0f / 9.0f)
	{
		float fWidthAjustment = 160.0f - (fRatio * 90.0f);
		m_oOrthographicProjection.MakeOrthographic(fWidthAjustment / 2.0f, 160.0f - (fWidthAjustment / 2.0f), 90.0f, 0.0f, 0.0f, 1000.0f );
	}
}

///------------------------------------------------------------------------

bool SpaceGame::CKRBSG::IsClicked()
{
	return m_bIsClicked;
}

void SpaceGame::CKRBSG::SetViewMatrix( XEngine::XMatrix44 & mView )
{
	m_oViewMatrix = mView;
}
///------------------------------------------------------------------------
