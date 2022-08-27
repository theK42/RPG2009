#include "game/shared/gamepch.h"
#include "game/spacegame/luatester.h"
#include "game/spacegame/miniload.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

float CLuaTester::ms_pVertices[msc_nNumVertices][msc_nVertexWidth] =
{
	{-1.0f, -1.0f, 0.0f},
	{1.0f, -1.0f, 0.0f},
	{1.0f, 1.0f, 0.0f},
	{-1.0f, 1.0f, 0.0f},	
};

///----------------------------------------------------------------------------

u8 CLuaTester::ms_pIndices[msc_nNumVertices] =
{
	0,
	1,
	2,
	3
};

///----------------------------------------------------------------------------

CLuaTester::CLuaTester()
{
	m_bInitialized = false;
}

///----------------------------------------------------------------------------

CLuaTester::~CLuaTester()
{
	Deinit();
}

///----------------------------------------------------------------------------

void CLuaTester::Init( CLoadProcessor * pLoader, CMeshRenderer * pRenderer, CLuaEnvironmentUpdater * pScriptUpdater, CMechanicsUpdater * pMechanicsUpdater )
{
	XASSERT(!m_bInitialized);
	m_oAnimatedTransform.Init(pMechanicsUpdater, XEngine::XMatrix44(XEngine::IDENTITY));

	CMeshSetupFunctor oMeshSetupFunctor;
	oMeshSetupFunctor.Init(this, &CLuaTester::SetupMesh);

	XEngine::XColor oColor;
	oColor.Set(1.0f, 1.0f, 1.0f, 1.0f);  /// Blinding white

	m_oMesh.Init(pRenderer, m_oAnimatedTransform.GetInternalUpdatable().GetMatrix(), oMeshSetupFunctor, oColor);

	XEngine::XTestFunctor oReadyTestFunctor;
	oReadyTestFunctor.Init(this, &CLuaTester::IsReady);

	XEngine::XSimpleFunctor oStartDisplayingAnimationFunctor;
	oStartDisplayingAnimationFunctor.Init(this, &CLuaTester::StartScript);

	XEngine::XSimpleFunctor oStopDisplayingAnimationFunctor;
	oStopDisplayingAnimationFunctor.Init(this, &CLuaTester::StopScript);

	m_oScreenSteward.Init(oReadyTestFunctor, oStartDisplayingAnimationFunctor, oStopDisplayingAnimationFunctor);
	m_bInitialized = true;

}

///----------------------------------------------------------------------------